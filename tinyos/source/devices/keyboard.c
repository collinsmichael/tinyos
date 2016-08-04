/* ========================================================================= */
/* File Name : source/devices/keyboard.c                                     */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* ps2 keyboard interface. polls the keyboard for keydown keyup scan codes   */
/* passes the scan codes through a keymap lookup (qwerty and dvorak support) */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"


static unsigned char keyboard[128];

static char qwerty[256] = 
"\x00\x00\x1B\x1B" "1!2@3#4$5%6^7&8*9(0)-_=+"  "\x08\x08"
"\x09\x09"         "qQwWeErRtTyYuUiIoOpP[{]}"      "\n\n"
"\x00\x00"         "aAsSdDfFgGhHjJkKlL;:'\"`~"
"\x00\x00"        "\\|zZxXcCvVbBnNmM,<.>/?"    "\x00\x00"
"\x00\x00\x00\x00"          "  "               "\x00\x00";

static char dvorak[256] =
"\x00\x00\x1B\x1B"  "1!2@3#4$5%6^7&8*9(0)[{]}"  "\x08\x08"
"\x09\x09"         "'\",<.>pPyYfFgGcCrRlL/?=+"      "\n\n"
"\x00\x00"          "aAoOeEuUiIdDhHtTnNsS-_\\|"
"\x00\x00"         "\\|;:qQjJkKxXbBmMwWvVzZ"    "\x00\x00"
"\x00\x00\x00\x00"         "  "                 "\x00\x00";


char LookUp[256] = 
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\xC0\x00\x00\xC0\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x80\x10\x10\x10\x10\x10\x10\x10\x30\x30\x10\x10\x10\x10\x10\x10"
"\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x10\x10\x30\x30\x10\x10"
"\x10\x0A\x0A\x0A\x0A\x0A\x0A\x02\x02\x02\x02\x02\x02\x02\x02\x02"
"\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x30\x10\x30\x10\x10"
"\x10\x04\x04\x04\x04\x04\x04\x04\x04\x04\x04\x04\x04\x04\x04\x04"
"\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x04\x04\x04\x04\x30\x10\x30\x10\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

#define NUMBER  0x01
#define UPPER   0x02
#define LOWER   0x04
#define HEXNUM  0x08
#define GRAMMAR 0x10
#define PARENS  0x20
#define NEWLINE 0x40
#define SPACE   0x80


char *keymap = qwerty;
static int capslock = 0;


int _cdecl kbrd_open(char *path, char *access) {
	return 1;
}


int _cdecl kbrd_close(void) {
	return 1;
}


void _cdecl kbrd_putc(char c) {
	return;
}


#ifndef NDEBUG
// when building in debug mode we simulate keyboard input by sourcing bytes from this array
// here every keystroke is stored and played back, from login and password, commandline entry
// even human errors and corrections are embedded within this array
static int getci = 0;
static char getcbuffer[] = "mike\nletmein\necho hellp\x08o wok\x08rld\nchat\nhello\n0\nexit\nexit\n\n";
char _cdecl kbrd_getc(void) {
	char c = getcbuffer[getci];
	getci = min(getci+1, sizeof(getcbuffer)-2);
	return c;
}
#else
// when building in release mode we acquire keyboard input by polling port 0x60 when port 0x64
// indicates that there is data waiting to be read.
char _cdecl kbrd_getc(void) {
	unsigned char k;
	unsigned char c;
	unsigned char s;
	unsigned char b;

	c = inportb(0x64);
	if ((c & 1) == 0) return 0;

	// read the next packet...
	k = inportb(0x60);
	if (k == 0xE0) {
		k = inportb(0x60);
		return 0;
	}

	b = k & 0x80;
	k = k & 0x7F;

	if (b) {
		keyboard[k] = 0;
		return 0;
	}
	keyboard[k] = 1;

	if (k == 0x3A) {
		capslock = 1 - capslock;
	}

	s = keyboard[0x2A] | keyboard[0x36];
	c = keymap[2*k + s];
	if (capslock && (LookUp[c] & (UPPER|LOWER))) {
		s ^= capslock;
		c = keymap[2*k + s];
	}
	return c;
}
#endif