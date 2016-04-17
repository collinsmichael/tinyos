/* ========================================================================= */
/* File Name : source/internal.h                                             */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* Internal type definitions.                                                */
/* ========================================================================= */

#ifndef internal_h
#define internal_h


/* ------------------------------------------------------------------------- */
/* source/library/malloc.c                                                   */
/* ------------------------------------------------------------------------- */
typedef struct E820 {
    unsigned __int64 base;
    unsigned __int64 size;
    unsigned __int32 flag;
    unsigned __int32 meta;
} E820;

#define MEMMAPSIZE (100)
typedef struct MEMMAP {
	char *addr;
	int   size;
} MEMMAP;

int memmap_init(void);


/* ------------------------------------------------------------------------- */
/* source/devices/terminal.c                                                 */
/* ------------------------------------------------------------------------- */
char _cdecl tty_getc(void);
void _cdecl tty_putc(char c);
int  _cdecl tty_open(char *path, char *access);
int  _cdecl tty_close(void);


/* ------------------------------------------------------------------------- */
/* source/devices/keyboard.c                                                 */
/* ------------------------------------------------------------------------- */
char _cdecl kbrd_getc(void);
void _cdecl kbrd_putc(char c);
int  _cdecl kbrd_open(char *path, char *access);
int  _cdecl kbrd_close(void);


/* ------------------------------------------------------------------------- */
/* source/devices/serial.c                                                   */
/* ------------------------------------------------------------------------- */
char _cdecl s0_getc(void);
void _cdecl s0_putc(char c);
int  _cdecl s0_open(char *path, char *access);
int  _cdecl s0_close(void);


/* ------------------------------------------------------------------------- */
/* source/kernel/init.c                                                      */
/* ------------------------------------------------------------------------- */
int init(void);


/* ------------------------------------------------------------------------- */
/* source/kernel/shell.c                                                     */
/* ------------------------------------------------------------------------- */
int shell_main(USER *user);


#endif