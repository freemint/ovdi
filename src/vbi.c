#include <osbind.h>

#include "display.h"
#include "libkern.h"
#include "ovdi_types.h"
#include "vbi.h"
#include "../../sys/mint/arch/asm_spl.h"

#define	_vblqueue	0x456L
#define nvbls		0x454L

#define MAX_VINTS	10
#define VBI_INST	1
#define VBI_ENABLE	2

extern void new_vbi_wrapper(void);
void new_vbi (void);

static short	get_vbitics(void);
static short	add_vbi_function(unsigned long function, unsigned long tics);
static void	remove_vbi_function(unsigned long function);
static void	reset_vbi(void);
static void	enable_vbi(void);
static void	disable_vbi(void);

static VBIAPI vbiapi =
{
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

VBIAPI *
init_vbi(void)
{
	int i;
	short nvbi;
	unsigned long *VBI_entry;
	long usp;

	reset_vbi();

	usp	= Super(1);
	if (!usp)
		usp = Super(0);
	else
		usp = 0;

	if (!(flags & VBI_INST))
	{
		nvbi = *(short *)nvbls;
		VBI_entry = (unsigned long *) *(unsigned long *)_vblqueue;

#if 0
		*VBI_entry = (unsigned long)&new_vbi_wrapper;

#else
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
#endif
		flags |= VBI_INST;
	}

	if (usp)
		Super(usp);

	return &vbiapi;
}

static void
reset_vbi(void)
{

	disable_vbi();
	bzero(vbiints, sizeof(vbiints));
	return;
}

static void
enable_vbi(void)
{
	if (!(flags & VBI_INST))
		return;

	flags |= VBI_ENABLE;
	return;
}

static void
disable_vbi(void)
{
	flags &= ~VBI_ENABLE;
	return;
}

static short
get_vbitics(void)
{
	return (1000/50);
}

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
	return;
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
	return;
}
