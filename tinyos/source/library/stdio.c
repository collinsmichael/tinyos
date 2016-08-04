/* ========================================================================= */
/* File Name : source/library/stdio.c                                        */
/* File Date : 4 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* standard input output interface.                                          */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"

#define standard(file) (((int)file)-1) /* coverts stdin stdout stderr to index */

FILE devices[] = {
    /* path      size     access flags ownID grpID getc       putc       open,      close      */
	{  "stdin",  80*25*2, "rw",  0666, 0,    0,    tty_getc,  tty_putc,  tty_open,  tty_close  },
	{  "stout",  80*25*2, "rw",  0666, 0,    0,    tty_getc,  tty_putc,  tty_open,  tty_close  },
	{  "stderr", 80*25*2, "rw",  0666, 0,    0,    tty_getc,  tty_putc,  tty_open,  tty_close  },
	{  "tty",    80*25*2, "rw",  0666, 0,    0,    tty_getc,  tty_putc,  tty_open,  tty_close  },
	{  "kbrd",   256,     "rw",  0666, 0,    0,    kbrd_getc, kbrd_putc, kbrd_open, kbrd_close },
	{  "s0",     512,     "rw",  0666, 0,    0,    s0_getc,   s0_putc,   s0_open,   s0_close   }
};


int access_mode(char *access) {
	int c;
	int mode = 0;
	for (c = 0; access[c]; c++) {
		if      (access[c] == 'r') mode |= 4;
		else if (access[c] == 'w') mode |= 2;
		else if (access[c] == 'x') mode |= 1;
	}
	return mode;
}


FILE *match_device(char *path, char *access) {
	int d;
	int mode = access_mode(access);
	for (d = 0; d < elementsof(devices); d++) {
		if (kstrcmp(path, devices[d].path) != 0) continue;
		if ((mode | devices[d].flags) != devices[d].flags) continue;
		return &devices[d];
	}
	return 0;
}


FILE *kopen(char *path, char *access) {
	FILE *device = 0;
	FILE *file   = 0;

	/* recognized devices */
	device = match_device(path, access);
	if (!device) return 0;
	if (!device->open) return 0;
	if (!device->open(path, access)) return 0;

	/* cache a user copy */
	file = (FILE*)kalloc(sizeof(FILE));
	if (!file) return 0;
	kmemcpy(file, device, sizeof(FILE));
	file->access = access;
	return file;
}


char kgetc(FILE *file) {
	if (!file) return 0;
	if (file >= stdin && file <= stderr) return devices[standard(file)].getc();
	return file->getc();
}


void kputc(FILE *file, char c) {
	if (!file) return;
	if (file >= stdin && file <= stderr) devices[standard(file)].putc(c);
	else file->putc(c);
}


void kclose(FILE *file) {
	if (!file) return;
	if (file >= stdin && file <= stderr) devices[standard(file)].close();
	else file->close();
	kfree(file, sizeof(FILE));
}


int kfgets(FILE *file, char *buffer, int size) {
	char c;
	int  i;
	if (!file) return 0;
	if (file >= stdin && file <= stderr) file = &devices[standard(file)];
	for (i = c = 0; c != '\n'; c = kgetc(file)) {
		if (!c) continue;
		if (c == 0x08) { /* backspace */
			buffer[i--] = 0;
			buffer[i = max(i, 0)] = 0;
		} else {
			buffer[i++] = c;
			i = min(i, size-2);
		}
	}
	return i;
}