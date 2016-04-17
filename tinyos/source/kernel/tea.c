/* ========================================================================= */
/* File Name : source/kernel/tea.c                                           */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* TEA (Tiny Encryption Algorithm)                                           */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"


#define MAGIC 0x9E3779B9
#define MIX   (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p%k)^e] ^ z)))

int tea_encrypt(char *text, char *key) {
	unsigned int y, p, e;

	unsigned int n = kstrlen(text);
	unsigned int k = kstrlen(key);
	unsigned int rounds = 6 + 64/k + 64/n;
	unsigned int sum = 0;
	unsigned int z = text[n-1];

	do {
		sum += MAGIC;
		e = (sum >> 2) & 3;
		for (p = 0; p < n-1; p++) {
			y = text[p + 1];
			z = text[p] += MIX;
		}
		y = text[0];
		z = text[n - 1] += MIX;
	} while (--rounds);
	return 1;
}


int tea_decrypt(char *text, char *key) {
	unsigned int z, p, e;

	unsigned int n = kstrlen(text);
	unsigned int k = kstrlen(key);
	unsigned int rounds = 6 + 64/k + 64/n;
	unsigned int sum = rounds*MAGIC;
	unsigned int y = text[0];

	do {
		e = (sum >> 2) & 3;
		for (p = n - 1; p > 0; p--) {
			z = text[p - 1];
			y = text[p] -= MIX;
		}
		z = text[n - 1];
		y = text[0] -= MIX;
	} while ((sum -= MAGIC) != 0);

	return 1;
}