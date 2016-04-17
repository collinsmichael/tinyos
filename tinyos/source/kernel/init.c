/* ========================================================================= */
/* File Name : source/kernel/init.c                                          */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* startup initializations, and throw away code.                             */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"


int init(void) {
	memmap_init(); /* source/library/memory.c */
	return 1;
}