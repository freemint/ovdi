#include <osbind.h>

#include "vbiapi.h"

#include "ovdi_lib.h"
#include "modinf.h"
#include "ovdi_types.h"
#include "../../sys/mint/arch/asm_spl.h"

#define	_vblqueue	0x456L
#define nvbls		0x454L

#define MAX_VINTS	10
#define VBI_INST	1
#define VBI_ENABLE	2

void	init	(OVDI_LIB *l, struct module_desc *ret);

extern void new_vbi_wrapper(void);
void new_vbi (void);

static short	install(void);
static short	get_vbitics(void);
static short	add_vbi_function(unsigned long function, unsigned long tics);
static void	remove_vbi_function(unsigned long function);
static void	reset_vbi(void);
static void	enable_vbi(void);
static void	disable_vbi(void);

static char sname[] = "VBI driver (Atari standard)";
static char lname[] = "VBI driver for oVDI installing in the\nstandard '_vblqueue'";

static OVDI_LIB	*L = 0;

/*
 * VBIAPI - oVDI access us via this API
*/
static VBIAPI vbiapi =
{
	0,
	0x00000001,
	sname,
	lname,
	install,
	get_vbitics,
	add_vbi_function,
	remove_vbi_function,
	reset_vbi,
	enable_vbi,
	disable_vbi,
};

static short flags = 0;

static unsigned long vbiints[] = 
{
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0
};

/*
 * oVDI calls us here right after we've been loaded.
*/
void
init(OVDI_LIB *l, struct module_desc *ret)
{
	L = l;
	ret->types = D_VBI;
	ret->vbi = (void *)&vbiapi;
}

/*
 * Install .. yes, we know what that does?
*/
static void I(void);
static short
install(void)
{
	reset_vbi();
	(void)(*L->supexec)(I);
	return 0;
}

static void
I(void)
{
	int i;
	short nvbi;
	unsigned long *VBI_entry;

	if (!(flags & VBI_INST))
	{
		nvbi = *(short *)nvbls;
		VBI_entry = (unsigned long *) *(unsigned long *)_vblqueue;

		for (i = 0; i < nvbi; i++)
		{
			if (*VBI_entry == 0)
			{
				short sr;

				sr = spl7();
				*VBI_entry++ = (unsigned long)&new_vbi_wrapper;
				*VBI_entry = 0L;
				spl(sr);
				break;
			}
			VBI_entry++;
		}
		flags |= VBI_INST;
	}
}

/*
 * Disable VBI and clear all installed VBI functions
*/
static void
reset_vbi(void)
{
	disable_vbi();
	(*L->bzero)(vbiints, sizeof(vbiints));
}

/*
 * Enable the VB interrupts
*/
static void
enable_vbi(void)
{
	if ((flags & VBI_INST))
		flags |= VBI_ENABLE;
}

/*
 * Turn off VB interrupts
*/
static void
disable_vbi(void)
{
	flags &= ~VBI_ENABLE;
}

/*
 * return
*/
static short
get_vbitics(void)
{
	return (1000/50);
}

/*
 * High level VBI handler. This calls out the installed functions.
*/
void
new_vbi(void)
{
	register unsigned long *vints = (unsigned long *)&vbiints;
	register void (*func)(void);

	if (!(flags & VBI_ENABLE))
		return;

	while (vints[2])
	{
		if (!vints[0])
		{
			vints[0] = vints[1];
			func = (void (*))vints[2];
			(*func)();
		}
		else
			vints[0]--;

		vints += 3;
	}
}

static short
add_vbi_function(unsigned long function, unsigned long tics)
{
	int i;
	unsigned long *vints = (unsigned long *)&vbiints;

	for (i = 0; i < MAX_VINTS; i++)
	{
		if (!vints[2])
		{
			short sr;

			sr = spl7();
			vints[0] = 0;
			vints[1] = tics;
			vints[2] = function;
			spl(sr);
			return 0;
		}
		else if (vints[2] == function)
			return 0;

		vints += 3;
	}
	return -1;
}

static void
remove_vbi_function(unsigned long function)
{
	short sr;
	int i;
	unsigned long *vints = (unsigned long *)&vbiints;

	for (i = 0; i < MAX_VINTS; i++)
	{
		if (vints[2] == function)
		{
			sr = spl7();
			while (i < MAX_VINTS)
			{
				vints[0] = vints[0+3];
				vints[1] = vints[1+3];
				vints[2] = vints[2+3];
				vints += 3;
				i++;
			}

			spl(sr);
			return;
		}
		vints += 3;
	}
}
