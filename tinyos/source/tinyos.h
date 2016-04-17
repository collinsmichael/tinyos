/* ========================================================================= */
/* File Name : source/tinyos.h                                               */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* UNIX type operating system.                                               */
/* ========================================================================= */

#ifndef tinyos_h
#define tinyos_h

#define elementsof(a)     (sizeof(a)/sizeof(a[0]))
#define min(a, b)         (((a) < (b)) ? (a) : (b))
#define max(a, b)         (((a) > (b)) ? (a) : (b))
#define swap(type, a, b)  do { type c = a; a = b; b = c; } while(0)

typedef struct USER {
	char *name;
	char *pass;
	char *home;
	int   userid;
} USER;

typedef struct FILE { 
    char *path;
    int   size;
    char *access;
    int   flags;
    int   ownID;
    int   grpID;
	char  (_cdecl *getc)(void);
	void  (_cdecl *putc)(char c);
	int   (_cdecl *open)(char *path, char *access);
	int   (_cdecl *close)(void);
} FILE;

#define stdin  ((FILE*)0x00000001)
#define stdout ((FILE*)0x00000002)
#define stderr ((FILE*)0x00000003)



void  kputchar(char c);
void  kprintn(int n, int base);
void  kprintf(char *fmt, ...);
void  kvprintf(char *Text, char *fmt, ...);

int   kdelete(char *Path);
int   kcopy(char *Destination, char *Source);
int   kmove(char *Destination, char *Source);
int   kchmod(char *Path, int Access);
int   kchown(char *NewOwner, char *Path);

int   kbhit(void);
char  getch(void);
void  putch(char c);
void  kshowcursor(void);


/* ------------------------------------------------------------------------- */
/* source/library/stdio.c                                                    */
/* ------------------------------------------------------------------------- */
FILE *kopen(char *path, char *access);
void  kclose(FILE *File);
int   kseek(FILE *file, int offset, int origin);
int   ktell(FILE *file);
int   kread(FILE *file, char *buffer, int size);
int   kwrite(FILE *file, char *buffer, int size);
char  kgetc(FILE *file);
void  kputc(FILE *file, char c);
int   kfgets(FILE *file, char *buffer, int size);


/* ------------------------------------------------------------------------- */
/* source/library/malloc.c                                                   */
/* ------------------------------------------------------------------------- */
void *kalloc(int size);
void  kfree(void *addr, int size);
void  kmemset(void *buffer, int value, int size);
void  kmemcpy(void *dst, void *src, int size);
int   kmemcmp(void *dst, void *src, int size);


/* ------------------------------------------------------------------------- */
/* source/library/string.c                                                   */
/* ------------------------------------------------------------------------- */
int kstrcpy(char *dst, char *src);
int kstrcat(char *dst, char *src);
int kstrlen(char *text);
int kstrcmp(char *dst, char *src);


/* ------------------------------------------------------------------------- */
/* source/devices/printf.c                                                   */
/* ------------------------------------------------------------------------- */
void kputn(FILE *file, int n, int b);
int  kfprintf(FILE *File, char *fmt, ...);


/* ------------------------------------------------------------------------- */
/* source/devices/portio.c                                                   */
/* ------------------------------------------------------------------------- */
char  inportb(int port);
short inportw(int port);
int   inportd(int port);
void  outportb(int port, int data);
void  outportw(int port, int data);
void  outportd(int port, int data);


/* ------------------------------------------------------------------------- */
/* source/kernel/tea.c                                                       */
/* ------------------------------------------------------------------------- */
int tea_encrypt(char *text, char *key);
int tea_decrypt(char *text, char *key);


/* ------------------------------------------------------------------------- */
/* source/kernel/logon.c                                                     */
/* ------------------------------------------------------------------------- */
USER *logon(void);
USER *kgetuser(void);


/* ------------------------------------------------------------------------- */
/* source/devices/shell.c                                                    */
/* ------------------------------------------------------------------------- */
void shell(char *text);


#endif