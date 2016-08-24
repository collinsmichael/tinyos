/* ========================================================================= */
/* File Name : source/userland/time.c                                        */
/* File Date : 7 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* this program displays the current time and date as read from cmos memory. */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"


#define second   0x00
#define minute   0x01
#define hour     0x02
#define day      0x03
#define month    0x04
#define year     0x05
#define century  0x06


int time(int argc, char **argv) {
    unsigned char datetime[8];
    clock(datetime);
    int yyyy = datetime[century]*100 + datetime[year];

	kfprintf(stdout, "%d-%d-%d ",  datetime[day], datetime[month], yyyy);
	kfprintf(stdout, "%d:%d:%d\n", datetime[hour], datetime[minute], datetime[second]);
	return 0;
}