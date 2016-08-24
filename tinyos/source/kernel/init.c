/* ========================================================================= */
/* File Name : source/kernel/init.c                                          */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* startup initializations, and throw away code.                             */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"


unsigned char datetime[8];


int init(void) {
    clock(datetime);
	memmap_init(); /* source/library/memory.c */
	return 1;
}