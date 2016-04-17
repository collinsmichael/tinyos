/* ========================================================================= */
/* File Name : source/shell/shell.c                                          */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* shell command line interpreter.                                           */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"
#include "../userland.h"


/* a list is used in case of collisions in the hash table */
typedef struct list {
	void *next;
	char  size;
	char *name;
	int (__cdecl *proc)(int argc, char **argv);
} list;

/* list of known programs */
list program[] = {
	{ 0, 4, "echo", echo },
	{ 0, 4, "chat", chat }
};

/* function names are used as the hash key */
list *hash_table[256] = {
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	&program[1], /* 0x48 = chat */
	0,0,0, 0,0,0,0,

	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,
	&program[0], /* 0x5D = echo */
	0,0,
};


/* ------------------------------------------------------------------------- */
/* given a command from the terminal take the name of the proc and use it as */
/* a key into the hash table. this produces an 8 bit hash.                   */
/* ------------------------------------------------------------------------- */
unsigned char hash(char *text) {
	unsigned char h = 0;
	while (*text > ' ') {        /* stop on whitespace */
		h = (h << 5) | (h >> 3); /* rol 5              */
		h = h ^ *text++;         /* xor                */
	}
	return h;
}


/* ------------------------------------------------------------------------- */
/* get the number of commandline arguments.                                  */
/* ------------------------------------------------------------------------- */
int shell_argc(char *text) {
	int argc = 0;
	while (*text >= ' ' ) {
		char c;

		while (*text == ' ' ) text++; /* skip white space */
		c = *text;

		/* grab quoted arguments */
		if (c == '"') {
			text++;
			argc++;
			while (*text != c) if (*text++ < ' ') break;

		/* grab non quoted arguments */
		} else if (*text > ' ' ) {
			argc++;
			while (*text++ > ' ');
		}
	}
	return argc;
}


/* ------------------------------------------------------------------------- */
/* convert commandline arguments to an array of pointers.                    */
/* ------------------------------------------------------------------------- */
char **shell_argv(int argc, char *text) {
	int arg = 0;

	char **argv = (char**)kalloc((argc + 1) * sizeof(char**));
	if (!argv) return 0;

	while (*text >= ' ') {
		char c;

		while (*text == ' ') text++; /* skip white space */

		c = *text;
		if (c == '"') { /* grab quoted arguments */
			argv[arg++] = ++text;
			while (*text != c) {
				if (*text < ' ') break;
				text++;
			}
			*text++ = 0;

		} else if (*text > ' ') { /* grab non quoted arguments */
			argv[arg++] = text;
			while (*text > ' ') text++;
			*text++ = 0;
		}
	}
	argv[argc] = "";
	return argv;
}


/* ------------------------------------------------------------------------- */
/* recognise and execute shell commands.                                     */
/* ------------------------------------------------------------------------- */
void shell_exec(char *text) {
	list *l;
	unsigned char h = hash(text);                    /* index the hash table */
	for (l = hash_table[h]; l; l = l->next) {        /* scan the list        */
		unsigned char s = l->size;                   /* length of the name   */
		if (kmemcmp(text, l->name, s) == 0) {        /* match proc name      */
			int    argc = shell_argc(text);          /* get num arguments    */
			char **argv = shell_argv(argc, text);    /* get num arguments    */
			l->proc(argc, argv);                     /* execute proc         */
			kfree(argv, (argc+1) * sizeof(char**));  /* clean up             */
		}
	}
}


int shell_main(USER *user) {
	char cmd[80*25*2];
	char c = 0;
	int  cptr = 0;
	if (!user) return 0;

	kmemset(cmd, 0, sizeof(cmd));
	for (kfprintf(stdout, "%s > ", user->name);;) {
		/* get keyboard input */
		if (c = kgetc(stdin)) {                           /* read the char  */
			if (c == 0x08) {                              /* backspace      */
				cptr = max(cptr - 1, 0);                  /* backup cursor  */
				cmd[cptr] = 0;                            /* clear char     */
			} else {
				cmd[cptr] = c;                            /* save the char  */
				cptr = min(cptr + 1, sizeof(cmd)-1);      /* advance cursor */
			}
		}

		if (c == '\n') {
			if (kstrcmp(cmd, "logout\n") == 0) break;/* time to exit        */
			if (kstrcmp(cmd, "exit\n") == 0) break;  /* time to exit        */
			shell_exec(cmd);                         /* execute the command */
			kmemset(cmd, cptr = 0, sizeof(cmd));     /* clear the buffer    */
			kfprintf(stdout, "\n%s > ", user->name);
		}
	}
	return 0;
}