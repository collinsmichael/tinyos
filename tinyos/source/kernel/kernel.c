/* ========================================================================= */
/* File Name : source/kernel/kernel.c                                        */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* 32bit protected mode unix like kernel entry point.                        */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"


#ifdef  NDEBUG
#undef  main
#define main mainCRTStartup
#endif


void main(void) {
	// if we cannot initialize the system, hang forever in shame
	if (!init()) for (;;);

	// infinite loop
	for (;;) {
		USER *user = logon();
		if (user) {
		    shell_main(user);
		    kfree(user, sizeof(USER));
		}
	}
}