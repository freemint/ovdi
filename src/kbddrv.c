#include <osbind.h>

#include "display.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "kbddrv.h"

static short		key_available(void);
static short		get_key(short *asci, short *scan, long *state);
static void		wait_key(short *asci, short *scan, long *state);
static long		get_kbd_state(void);

KBDAPI	kapi;

KBDAPI *
init_keyboard( VIRTUAL *v )
{
	KBDAPI *k = &kapi;

	k->keywaiting = key_available;
	k->getkey = get_key;
	k->waitkey = wait_key;
	k->getks = get_kbd_state;
	
	return k;
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
