/* ========================================================================= */
/* File Name : source/library/malloc.c                                       */
/* File Date : 5 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* basic memory management system using a first fit algorithm.               */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"

extern MEMMAP memmap[MEMMAPSIZE];
#ifndef NDEBUG
extern E820 e820debug[5];
#endif


/* --------------------------------------------------------- */
/* Allocate size units  from the given map.  Return the base */
/* of the allocated space.                                   */
/* Algorithm is first fit.                                   */                                      
/* --------------------------------------------------------- */

void *kalloc(int size) {
    register char  *a;
    register struct MEMMAP *bp;

    for (bp = memmap; bp->size; bp++) {
        if (bp->size >= size) {
            a = bp->addr;
            bp->addr += size;
            if ((bp->size -= size) == 0) do {
                bp++;
                (bp-1)->addr = bp->addr;
            } while ((bp-1)->size = bp->size);
            return a;
        }
    }
    return 0;
}



/* --------------------------------------------------------- */
/* Free the  previously allocated  space addr of  size units */
/* into the specified map. Sort addr into map and combine on */
/* one or both ends if possible.                             */
/* --------------------------------------------------------- */

void kfree(void *addr, int size) {
	register struct MEMMAP *bp;
	register int    s;
	register char  *t;
	register char  *a = addr;

	for (bp = memmap; bp->addr <= a && bp->size != 0; bp++);

	if (bp > memmap && (bp-1)->addr + (bp-1)->size == a) {
		(bp-1)->size += size;
		if (a + size == bp->addr) {
			(bp-1)->size += bp->size;
			while (bp->size) {
				bp++;
				(bp-1)->addr = bp->addr;
				(bp-1)->size = bp->size;
			}
		}
	} else {
		if (a + size == bp->addr && bp->size) {
			bp->addr -= size;
			bp->size += size;
		} else if (size) do {
			t = bp->addr;
			bp->addr = a;
			a = t;
			s = bp->size;
			bp->size = size;
			bp++;
		} while (size = s);
	}
}



void kmemset(void *buffer, int value, int size) {
	volatile char *data = (char*)buffer;
	while (--size >= 0) data[size] = value;
}



void kmemcpy(void *dst, void *src, int size) {
	volatile char *d = (char*)dst;
	volatile char *s = (char*)src;
	while (--size >= 0) *d++ = *s++;
}



int kmemcmp(void *dst, void *src, int size) {
	int i;
	unsigned char *d = (unsigned char*)dst;
	unsigned char *s = (unsigned char*)src;
	for (i = 0; i < size; i++) {
		unsigned char c = (*d++) - (*s++);
		if (c < 0) return -(i+1);
		else if (c > 0) return (i+1);
	}
	return 0;
}


int memmap_init(void) {
    MEMMAP *s;    /* source memory map */
    MEMMAP *d;    /* destination memory map */
    int     m;    /* e820 memory map index */
	E820   *e820; /* points to the memory map our msdos stub built */

#ifdef NDEBUG
    _asm mov dword ptr [e820], 0x00010200
#else
    e820 = e820debug;
#endif

    /* Build memmap from E820 Memory Map */
	for (m = 0; e820->meta; e820++) {
        unsigned __int32 *d;
        unsigned __int32 *s;
        if (e820->flag != 1) continue; 
        s = (unsigned __int32*)&memmap[m];
        d = (unsigned __int32*)&e820->base;
        *d++ = *s++;
        *d++ = *s++;
        m++;
    }
    m--; /* discard the null memory map terminator */
    
    /* Bubble Sort the MemMap by address (ascending) */
    /* Bubble Sort is used as these E820 memory maps */
    /* are always nearly sorted.                     */
    for (s = memmap; s < &memmap[m]; s++) {
        for (d = &memmap[m]; d > s; d--) {
			if (!d->size) continue;
            if (d->addr < s->addr) {
				swap(char*, d->addr, s->addr);
                swap(int,   d->size, s->size);
            }
        }
    }
    
    /* Treat everything below 128kb as reserved. */
	for (m = 0; (int)memmap[m].addr < 0x00020000; m++) {
		int size = 0x00020000 - (int)memmap[m].addr;
		kalloc(min(size, memmap[m].size));
	}
	return 1;
}