/* ========================================================================= */
/* File Name : source/devices/portio.c                                       */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* port io interface, inline assembly unavoidable.                           */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"


char inportb(int port) {
	char data;

#ifdef NDEBUG
	_asm mov edx, dword ptr [port]
	_asm in  al, dx
	_asm mov byte ptr [data], al
#else
	data = 0;
#endif
	return data;
}


short inportw(int port) {
	short data;

#ifdef NDEBUG
	_asm mov edx, dword ptr [port]
	_asm in  ax, dx
	_asm mov word ptr [data], ax
#else
	data = 0;
#endif

	return data;
}


int inportd(int port) {
	int data;

#ifdef NDEBUG
	_asm mov edx, dword ptr [port]
	_asm in  eax, dx
	_asm mov dword ptr [data], eax
#else
	data = 0;
#endif

	return data;
}


void outportb(int port, int data) {
#ifdef NDEBUG
	_asm mov edx, dword ptr [port]
	_asm mov eax, dword ptr [data]
	_asm out dx, al
#endif
}


void outportw(int port, int data) {
#ifdef NDEBUG
	_asm mov edx, dword ptr [port]
	_asm mov eax, dword ptr [data]
	_asm out dx, ax
#endif
}


void outportd(int port, int data) {
#ifdef NDEBUG
	_asm mov edx, dword ptr [port]
	_asm mov eax, dword ptr [data]
	_asm out dx, eax
#endif
}