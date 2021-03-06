#include <osbind.h>
#include <stddef.h>

#include "kbdapi.h"
#include "modinf.h"
#include "ovdi_lib.h"

void _cdecl init	(OVDI_LIB *l, struct module_desc *ret, char *p, char *f);

static short _cdecl	install(void);
static short _cdecl	key_available(void);
static short _cdecl	get_key(short *asci, short *scan, long *state);
static void  _cdecl	wait_key(short *asci, short *scan, long *state);
static long  _cdecl	get_kbd_state(void);

static char sname[] = 	"Atari BIOS keyboard";
static char lname[] =	"Atari Bios keyboard device driver for\n" \
			"oVDI, using standard Atari BIOS for\n" \
			"keyboard input."; 
static char fpath[128] = { "0" };
static char fname[64] = { "0" };

static KBDAPI	kapi =
{
	0,
	0x00000001,
	sname,
	lname,
	fpath,
	fname,
	install,
	key_available,
	get_key,
	wait_key,
	get_kbd_state,
};

void _cdecl
init(OVDI_LIB *l, struct module_desc *ret, char *path, char *file)
{
	KBDAPI *k = &kapi;

	ret->types	= D_KBD;
	ret->kbd	= (void *)k;
	{
		char *t;

		t = k->pathname;
		while (*path)
			*t++ = *path++;
		*t = 0;
		t = k->filename;
		while (*file)
			*t++ = *file++;
		*t = 0;
	}
}

static short _cdecl
install(void)
{
	return 0;
}

static short _cdecl
key_available(void)
{
	return Bconstat(2);
}

static short _cdecl
get_key(short *a, short *s, long *sft)
{
	if ( (key_available()) == 0 )
		return 0;
	else
	{
		long key;

		key	= Bconin(2);
		*a	= key & 0xff;
		*s	= (key >> 16) & 0xff;
		*sft	= Kbshift(-1);
	}
	return 1;
}

static void _cdecl
wait_key(short *a, short *s, long *sft)
{
	long key;

	do {}
	while ((key_available()) == 0);

	key	= Bconin(2);
	*a	= key & 0xff;
	*s	= (key >> 16) & 0xff;
	*sft	= Kbshift(-1);
}

static long _cdecl
get_kbd_state(void)
{
	return (long)Kbshift(-1);
}
