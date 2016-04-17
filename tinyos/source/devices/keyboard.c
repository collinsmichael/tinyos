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

static char qwerty[256] = {
	0x00, 0x00, /* 00 (Error) */
	0x1B, 0x1B, /* 01 (Esc)   */
	'1',  '!',  /* 02 */
	'2',  '@',  /* 03 */
	'3',  '#',  /* 04 */
	'4',  '$',  /* 05 */
	'5',  '%',  /* 06 */
	'6',  '^',  /* 07 */
	'7',  '&',  /* 08 */
	'8',  '*',  /* 09 */
	'9',  '(',  /* 0A */
	'0',  ')',  /* 0B */
	'-',  '_',  /* 0C */
	'=',  '+',  /* 0D */
	0x08, 0x08, /* 0E (Back Space) */
	0x09, 0x09, /* 0F (Tab)        */
	'q',  'Q',  /* 10 */
	'w',  'W',  /* 11 */
	'e',  'E',  /* 12 */
	'r',  'R',  /* 13 */
	't',  'T',  /* 14 */
	'y',  'Y',  /* 15 */
	'u',  'U',  /* 16 */
	'i',  'I',  /* 17 */
	'o',  'O',  /* 18 */
	'p',  'P',  /* 19 */
	'[',  '{',  /* 1A */
	']',  '}',  /* 1B */
	'\n', '\n', /* 1C (Enter)  E0 1C (Enter KP) */
	0x00, 0x00, /* 1D (Ctrl L) E0 1D (Ctrl R)   */
	'a',  'A',  /* 1E */
	's',  'S',  /* 1F */
	'd',  'D',  /* 20 */
	'f',  'F',  /* 21 */
	'g',  'G',  /* 22 */
	'h',  'H',  /* 23 */
	'j',  'J',  /* 24 */
	'k',  'K',  /* 25 */
	'l',  'L',  /* 26 */
	';',  ':',  /* 27 */
	'\'', '"',  /* 28 */
	'`',  '~',  /* 29 */
	0x00, 0x00, /* 2A (Shift L) */
	'\\', '|',  /* 2B */
	'z',  'Z',  /* 2C */
	'x',  'X',  /* 2D */
	'c',  'C',  /* 2E */
	'v',  'V',  /* 2F */
	'b',  'B',  /* 30 */
	'n',  'N',  /* 31 */
	'm',  'M',  /* 32 */
	',', '<',   /* 33 */
	'.', '>',   /* 34 */
	'/', '?',   /* 35 35+ / KP  */ 
	0x00, 0x00, /* 36 (Shift R) */
	0x00, 0x00, /* 37 PrtSc     37+ * KP  37/54+ PrtSc */ 
	0x00, 0x00, /* 38 Alt L     E0 38     Alt R        */
	' ',  ' ',  /* 39 (Space) */
};

static char dvorak[256] = {
	0x00, 0x00, /* 00 (Error) */
	0x1B, 0x1B, /* 01 (Esc)   */
	'1',  '!',  /* 02 */
	'2',  '@',  /* 03 */
	'3',  '#',  /* 04 */
	'4',  '$',  /* 05 */
	'5',  '%',  /* 06 */
	'6',  '^',  /* 07 */
	'7',  '&',  /* 08 */
	'8',  '*',  /* 09 */
	'9',  '(',  /* 0A */
	'0',  ')',  /* 0B */
	'[',  '{',  /* 0C */
	']',  '}',  /* 0D */
	0x08, 0x08, /* 0E (Back Space) */
	0x09, 0x09, /* 0F (Tab)        */
	'\'', '"',  /* 10 */
	',',  '<',  /* 11 */
	'.',  '>',  /* 12 */
	'p',  'P',  /* 13 */
	'y',  'Y',  /* 14 */
	'f',  'F',  /* 15 */
	'g',  'G',  /* 16 */
	'c',  'C',  /* 17 */
	'r',  'R',  /* 18 */
	'l',  'L',  /* 19 */
	'/',  '?',  /* 1A */
	'=',  '+',  /* 1B */
	'\n', '\n', /* 1C (Enter)   E0 1C  (Enter KP) */
	0x00, 0x00, /* 1D (Ctrl L)  E0 1D  (Ctrl R)   */
	'a',  'A',  /* 1E */
	'o',  'O',  /* 1F */
	'e',  'E',  /* 20 */
	'u',  'U',  /* 21 */
	'i',  'I',  /* 22 */
	'd',  'D',  /* 23 */
	'h',  'H',  /* 24 */
	't',  'T',  /* 25 */
	'n',  'N',  /* 26 */
	's',  'S',  /* 27 */
	'-',  '_',  /* 28 */
	'\\', '|',  /* 29 */
	0x00, 0x00, /* 2A (Shift L) */
	'\\', '|',  /* 2B */
	';',  ':',  /* 2C */
	'q',  'Q',  /* 2D */
	'j',  'J',  /* 2E */
	'k',  'K',  /* 2F */
	'x',  'X',  /* 30 */
	'b',  'B',  /* 31 */
	'm',  'M',  /* 32 */
	'w',  'W',  /* 33 */
	'v',  'V',  /* 34 */
	'z',  'Z',  /* 35  35+ / KP */   
	0x00, 0x00, /* 36  ???      (Shift R)               */
	0x00, 0x00, /* 37  * PrtSc  37+ * KP   37/54+ PrtSc */ 
	0x00, 0x00, /* 38  Alt L    E0 38      Alt R        */
	' ',  ' ',  /* 39  (Space)                          */
};


char IsLetter[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	/*A B C D E F G H I J K L M N O P Q R S T U V W X Y Z*/
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
	/*a b c d e f g h i j k l m n o p q r s t u v w x y z*/
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


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
static int getci = 0;
static char getcbuffer[] = "mike\nletmein\necho hellp\x08o wok\x08rld\nchat\nhello\n0\nexit\nexit\n\n";
char _cdecl kbrd_getc(void) {
	char c = getcbuffer[getci];
	getci = min(getci+1, sizeof(getcbuffer)-2);
	return c;
}
#else
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
	if (capslock && IsLetter[c]) {
		s ^= capslock;
		c = keymap[2*k + s];
	}
	return c;
}
#endif