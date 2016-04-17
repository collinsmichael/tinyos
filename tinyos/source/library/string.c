/* ========================================================================= */
/* File Name : source/library/string.c                                       */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* string manipulation functionality.                                        */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"



int kstrcpy(char *dst, char *src) {
	while (*src) *dst++ = *src++;
	return 1;
}



int kstrcat(char *dst, char *src) {
	while (*dst++);
	while (*src) *dst++ = *src++;
	return 1;
}



int kstrlen(char *text) {
	int i = 0;
	while (text[i++]);
	return i;
}



int kstrcmp(char *dst, char *src) {
	int i;
	for (i = 0; (*dst && *src); i++) {
		char c = (*dst++) - (*src++);
		if      (c < 0) return -(i+1);
		else if (c > 0) return (i+1);
	}
	return 0;
}