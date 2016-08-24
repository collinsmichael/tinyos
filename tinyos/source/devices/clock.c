/* ========================================================================= */
/* File Name : source/devices/clock.c                                        */
/* File Date : 5 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* reads time and date from cmos registers.                                  */
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
#define statusa  0x07
#define statusb  0x08


void clock(unsigned char datetime[8]) {
    static unsigned char cmos[] = "\x00\x02\x04\x07\x08\x09\x32\x0A\x0B";
    unsigned char older[16];
    unsigned char newer[16];
    unsigned char cent = 0;
    int i;

    kmemset(newer, 0, sizeof(newer));
    do {
        kmemcpy(older, newer, sizeof(newer));
        for (i = 0; i < 8; i++) {
            outportb(0x70, cmos[i]);
            newer[i] = inportb(0x71);
        }

        outportb(0x70, 108);
        cent = inportb(0x71);
        if (cent == 0) continue;

        outportb(0x70, cent);
        newer[century] = inportb(0x71);
    } while (kmemcmp(older, newer, sizeof(newer)));

    // deal with binary coded decimal bioses
    if (!(newer[statusb] & 0x04)) {
        for (i = 0; i < 7; i++) {
            newer[i] = (newer[i] % 16) + (newer[i]/16)*10;
        }
    }

    // deal with 12 hour / 24 hour bioses
    if (!(newer[statusb] & 0x02) && (newer[hour] & 0x80)) {
        newer[hour] = ((newer[hour] & 0x7F) + 12) % 24;
    }

    // deal with centuary wrap around 1990-1999 ... 2000-2089
    if (cent == 0) {
        if (newer[year] > 90) newer[century] = 19;
        else                  newer[century] = 20;
    }
    kmemcpy(datetime, newer, 7);
}