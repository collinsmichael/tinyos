/* ========================================================================= */
/* File Name : source/devices/fdd.c                                          */
/* File Date : 5 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* floppy disk drive interface.                                              */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"


static char *drive_type[16] = {
	"no floppy drive",
	"360kb 5.25in floppy drive",
	"1.2mb 5.25in floppy drive",
	"720kb 3.5in",
	"1.44mb 3.5in",
	"2.88mb 3.5in"
};


int detectfdd(void) {
	unsigned char c;
	unsigned char a, b;

	outportb(0x70, 0x10);
	c = inportb(0x71);
	a = c >> 4;
	b = c & 0xF;

	kfprintf(stdout, " A: %s \n", drive_type[a]);
	kfprintf(stdout, " B: %s \n", drive_type[b]);
	kfprintf(stdout, "\n");
	return c;
}