/* ========================================================================= */
/* File Name : source/kernel/logon.c                                         */
/* File Date : 3 February 2015                                               */
/* Author(s) : Michael Collins                                               */
/* ------------------------------------------------------------------------- */
/* user logon routine. Uses TEA (Tiny Encryption Algorithm) for checking the */
/* password. We do not store the password directly, but rather we encrypt    */
/* the username with the password. If the encrypted username matches the one */
/* we associate with the user, then we can assume that the correct password  */
/* was enterred.                                                             */
/* ========================================================================= */

#include "../tinyos.h"
#include "../internal.h"

/* logon: "root"         */
/* password: ""          */
/*                       */
/* logon: "mike"         */
/* password: "letmein"   */

int curuser = -1;
USER users[256] = {
	/* name    pass     home    userid     */
	{ "root", "Ç–~4w", "/root", 0x00000001 },
	{ "mike", "áÚÀ> ", "/mike", 0x00000002 },
};


USER *logon(void) {
	char user[256];
	char pass[256];
	char hash[256];
	int  i;

	FILE *terminal = kopen("tty", "rw");
	if (!terminal) return 0;

	kmemset(user, 0, sizeof(user));
	kmemset(pass, 0, sizeof(pass));
	kmemset(hash, 0, sizeof(hash));

	kfprintf(stdout, "logon: ");
	kfgets(stdin, user, sizeof(user));

	kfprintf(stdout, "password: ");
	kfgets(stdin, pass, sizeof(pass));

	curuser = -1;
	for (i = 0; i < elementsof(users); i++) {
		if (kmemcmp(user, users[i].name, kstrlen(users[i].name)+1) == 0) {
			USER *usr;
			kmemcpy(hash, user, sizeof(hash));
			tea_encrypt(hash, pass);
			if (kstrcmp(hash, users[i].pass) != 0) break;
			usr = (USER*)kalloc(sizeof(USER));
			if (!usr) return 0;
			kmemcpy(usr, &users[i], sizeof(USER));
			curuser = i;
			return usr;
		}
	}
	return 0;
}


USER *kgetuser(void) {
	USER *usr;
	if (curuser < 0 || curuser >= elementsof(users)) return 0;
	usr = (USER*)kalloc(sizeof(USER));
	if (!usr) return 0;
	kmemcpy(usr, &users[curuser], sizeof(USER));
	usr->pass = 0;
	return usr;
}