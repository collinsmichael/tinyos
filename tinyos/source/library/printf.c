/* ========================================================================= */
/* File Name : source/library/printf.c                                       */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* basic printing functionality. kfprintf supports %c %s %d %l %x and uses   */
/* stardard io to direct its output to the target device.                    */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"

extern FILE devices[3];


/* ------------------------------------------------------------------------- */
/* prints a number Dennis Ritchie style supports bases from 2 ... 16         */
/* ------------------------------------------------------------------------- */
void kputn(FILE *file, int n, int b) {
	static char t[] = "0123456789ABCDEF";
	register int a;

	if (!file) return;
	if (b && b < sizeof(t)) {
		if (a = n/b) kputn(file, a, b);
		file->putc(t[n%b]);
	}
}


/* ------------------------------------------------------------------------- */
/* formatted printing. supports %s %c %l %d %x                               */
/* ------------------------------------------------------------------------- */
int kfprintf(FILE *file, char *fmt, ...) {
	register char  c;
	register char *s;
	register int  *x = (int*)&fmt;

	if (!file) return 0;
	if (file >= stdin && file <= stderr) file = &devices[(int)file];
	x++; /* now x is pointing at the variable args list */

	for (;;) {
		/* unformatted text */
		while ((c = *fmt++) != '%') {
			if (c == '\0') return 1;
			else file->putc(c);
		}

		/* formatting */
		c = *fmt++;
		if (c >= 'A' && c <= 'Z') {
			c = c - 'A' + 'a';
		}

		if (c == 'd' || c == 'l') {
			kputn(file, *x++, 10);
		} else if (c == 'x' || c == 'o') {
			kputn(file, *x++, (c == 'o') ? (8) : (16));
		} else if (c == 's') {
			for (s = (char*)*x++; c = *s++; file->putc(c));
		} else if (c == 'c') {
			file->putc((char)*x++);
		}
	}
}