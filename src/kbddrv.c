#include <osbind.h>

#include "display.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "kbddrv.h"

short key_available(void);
unsigned long get_key(void);
unsigned long wait_key(void);
unsigned long get_kbd_state(void);

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
	


short
key_available(void)
{
	return Bconstat(2);
}

unsigned long
get_key(void)
{
	if ( (key_available()) == 0 )
		return -1;
	else
		return Bconin(2);
}

unsigned long
wait_key(void)
{

	do {}
	while ((key_available()) == 0);

	return Bconin(2);
}

unsigned long
get_kbd_state(void)
{
	return (unsigned long)Kbshift(-1);
}


	