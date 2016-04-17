/* ========================================================================= */
/* File Name : source/userland/chat.c                                        */
/* File Date : 7 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* serial port based instant messaging program.                              */
/* type some text and it will be echoed in the peers terminal.               */
/* type "exit" to exit this program.                                         */
/* ========================================================================= */

#include "../tinyos.h"


int chat(int argc, char **argv) {
	USER *user;
	FILE *serial;
	char *recv;
	char *send;
	int   r = 0;
	int   s = 0;
	int   size = 80;
	int   quit = 0;

	user = kgetuser();
	if (!user) return kfprintf(stdout, " user error!\n");

	serial = kopen("s0", "rw");
	if (!serial) return kfprintf(stdout, " serial port error!\n");

	recv = kalloc(size);
	if (!recv) return kfprintf(stdout, " out of memory error!\n");
	send = kalloc(size);
	if (!send) return kfprintf(stdout, " out of memory error!\n");
	kmemset(recv, 0, size);
	kmemset(send, 0, size);

	for (kfprintf(stdout, "%s : ", user->name); !quit;) {
		char c;

		/* recv input from serial, send it to terminal */
		while (c = kgetc(serial)) {
			recv[r++] = c;                                       /* save the char    */
			r = min(r, size-2);                                  /* stay in bounds   */
			if (c == '\n') {                                     /* time to print it */
				kfprintf(stdout, "\npeer : %s", recv, send);     /* print recv       */
				kfprintf(stdout, "\n%s : %s", user->name, send); /* restore send     */
				kmemset(recv, r = 0, size);                      /* clear the buffer */
			}
		}

		/* recv input from terminal, send it to serial */
		while (c = kgetc(stdin)) {
			if (c == 0x08) { /* backspace */
				send[s--] = 0;
				send[s = max(s, 0)] = 0;
			} else { /* otherwise char */
				send[s++] = c;
				s = min(s, size-2);
			}
			kputc(serial, c); /* send the char */
			if (c == '\n') {
				if (kstrcmp(send, "exit\n") == 0) quit = 1;
				kmemset(send, s = 0, size); /* clear buffer */
				kfprintf(stdout, "%s : ", user->name);
			}
		}
	}
	kfree(recv, size);
	kfree(send, size);
	kfree(user, sizeof(USER));
	kclose(serial);
	return 0;
}