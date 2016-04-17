/* ========================================================================= */
/* File Name : source/devices/serial.c                                       */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* serial communications port interface.                                     */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"

#define PORT1 0x3F8
/* Defines Serial Ports Base Address */
/* COM1 0x3F8                        */
/* COM2 0x2F8                        */
/* COM3 0x3E8                        */
/* COM4 0x2E8                        */
/* BaudRate Divisor Values           */
/* Default 0x03 =  38,400 BPS        */
/*         0x01 = 115,200 BPS        */
/*         0x02 =  57,600 BPS        */
/*         0x06 =  19,200 BPS        */
/*         0x0C =   9,600 BPS        */
/*         0x18 =   4,800 BPS        */
/*         0x30 =   2,400 BPS        */


/* ---------------------------------------------------------------------------- */
/*	initialize serial port 1                                                    */
/* ---------------------------------------------------------------------------- */
int _cdecl s0_open(char *path, char *mode) {
	outportb(PORT1 + 1, 0x00);   /* Turn off interrupts - Port1                 */
	outportb(PORT1 + 3, 0x80);   /* SET DLAB ON                                 */
	outportb(PORT1 + 0, 0x01);   /* Set Baud rate - Divisor Latch Low Byte      */
	outportb(PORT1 + 1, 0x00);   /* Set Baud rate - Divisor Latch High Byte     */
	outportb(PORT1 + 3, 0x03);   /* 8 Bits, No Parity, 1 Stop Bit               */
	outportb(PORT1 + 2, 0xC7);   /* FIFO Control Register                       */
	outportb(PORT1 + 4, 0x0B);   /* Turn on DTR, RTS, and OUT2                  */
	return 1;
}



int _cdecl s0_close(void) {
	return 1;
}


#ifndef NDEBUG
static int getci = 0;
static char getcbuffer[] = "\0\0world\n\0";
char _cdecl s0_getc(void) {
	char c = getcbuffer[getci];
	getci = min(getci+1, sizeof(getcbuffer));
	return c;
}
#else
char _cdecl s0_getc(void) {
	char c = inportb(PORT1 + 5);      /* check to see if char has been received */
	if (c & 1) return inportb(PORT1); /* if so, then get char                   */
	return 0;
}
#endif


void _cdecl s0_putc(char c) {
#ifdef NDEBUG
	while ((inportb(PORT1 + 5) & 0x20) == 0);  /* wait for byte to be sent */
	outportb(PORT1, c);                        /* send byte to serial port */
#endif
}