/* ========================================================================= */
/* File Name : source/userland/echo.c                                        */
/* File Date : 7 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* type some text and it will be echoed in the terminal.                     */
/* ========================================================================= */

#include "../tinyos.h"


int echo(int argc, char **argv) {
	int arg;
	for (arg = 1; arg < argc; arg++) {
		kfprintf(stdout, "%s ", argv[arg]);
	}
	return 0;
}