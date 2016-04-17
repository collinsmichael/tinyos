/* ========================================================================= */
/* File Name : source/devices/terminal.c                                     */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* text mode ascii output. Once graphic modes are supported this will get an */
/* overhaul. currently supports scrolling text.                              */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"


#ifndef NDEBUG
	static char screen[80*25*4];
#endif

static char  bg = 0x02; // teletype color
static int   cursor = 0;
static int   screensize = 80*25;
static int   linesize   = 80;
static FILE *keyboard;


/* ------------------------------------------------------------------------- */
/* puts an ascii character on screen at the cursor position.                 */
/* ------------------------------------------------------------------------- */
void put_ascii(int position, char ascii, char color) {
#ifdef NDEBUG
	register char *scr = (char*)0x000B8000;
#else
	register char *scr = screen;
#endif
	scr[2*position  ] = ascii;
	scr[2*position+1] = color;
}


/* ------------------------------------------------------------------------- */
/* set up the terminal.                                                      */
/* ------------------------------------------------------------------------- */
int _cdecl tty_open(char *path, char *mode) {
#ifdef NDEBUG
	register char *scr = (char*)0x000B8000;
#else
	register char *scr = screen;
#endif
	keyboard = kopen("kbrd", "rw");
	kmemset(scr, 0, 2*screensize);
	cursor = 0;
	kshowcursor();
	return 1;
}


/* ------------------------------------------------------------------------- */
/* shut down the terminal.                                                   */
/* ------------------------------------------------------------------------- */
int _cdecl tty_close(void) {
	tty_open(0,0); /* resets and clears the screen */
	return 1;
}


/* ------------------------------------------------------------------------- */
/* displays a solid block cursor.                                            */
/* ------------------------------------------------------------------------- */
void kshowcursor(void) {
	put_ascii(cursor, 0xDB, bg);
}


/* ------------------------------------------------------------------------- */
/* displays ascii text on screen. handling special printable characters and  */
/* scrolling window contents as necessary.                                   */
/* ------------------------------------------------------------------------- */
void tty_putc(char c) {
	/* newline */
	if (c == '\n') {
		do put_ascii(cursor++, ' ', bg); while (cursor % 80 != 0);
	/* tab */
	} else if (c == '\t') {
		do put_ascii(cursor++, ' ', bg); while (cursor%4);
	/* carriage return */
	} else if (c == '\r') {
		cursor -= cursor % 80;
	/* backspace */
	} else if (c == 0x08) {
		put_ascii(cursor,   ' ', bg); /* wipe out cursor             */
		put_ascii(--cursor, ' ', bg); /* wipe out previous character */
	/* ascii */
	} else {
		put_ascii(cursor++, c, bg);
	}

	/* scroll text buffer if necessary */
	if (cursor >= screensize-linesize) {
#ifdef NDEBUG
		register char *scr = (char*)0x000B8000;
#else
		register char *scr = screen;
#endif
		kmemcpy(scr, &scr[2*linesize], 2*screensize - 2*linesize);
		cursor -= linesize;
	}

	/* redisplay the cursor */
	kshowcursor();
}


/* ------------------------------------------------------------------------- */
/* returns the character under the cursor.                                   */
/* ------------------------------------------------------------------------- */
char tty_getc(void) {
	char c = (keyboard) ? keyboard->getc() : 0;
	if (c) tty_putc(c);
	return c;
}