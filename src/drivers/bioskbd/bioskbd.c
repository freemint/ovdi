#include <osbind.h>

#include "kbdapi.h"
#include "modinf.h"
#include "ovdi_lib.h"

void	init	(OVDI_LIB *l, struct module_desc *ret);

static short		install(void);
static short		key_available(void);
static short		get_key(short *asci, short *scan, long *state);
static void		wait_key(short *asci, short *scan, long *state);
static long		get_kbd_state(void);

static char sname[] = 	"Atari BIOS keyboard";
static char lname[] =	"Atari Bios keyboard device driver for\n" \
			"oVDI, using standard Atari BIOS for\n" \
			"keyboard input."; 

static KBDAPI	kapi =
{
	0,
	0x00000001,
	sname,
	lname,
	install,
	key_available,
	get_key,
	wait_key,
	get_kbd_state,
};

void
init(OVDI_LIB *l, struct module_desc *ret)
{
	ret->types	= D_KBD;
	ret->kbd	= (void *)&kapi;
}
	

static short
install(void)
{
	return 0;
}

static short
key_available(void)
{
	return Bconstat(2);
}

static short
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

static void
wait_key(short *a, short *s, long *sft)
{
	long key;

	do {}
	while ((key_available()) == 0);

	key	= Bconin(2);
	*a	= key & 0xff;
	*s	= (key >> 16) & 0xff;
	*sft	= Kbshift(-1);
	return;
}

static long
get_kbd_state(void)
{
	return (long)Kbshift(-1);
}