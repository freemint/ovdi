

#include "display.h"
#include "libkern.h"
#include "ovdi_types.h"
#include "vbi.h"
#include "../../sys/mint/arch/asm_spl.h"

#define	_vblqueue	0x456L
#define nvbls		0x454L

#define MAX_VINTS	10

extern void new_vbi_wrapper(void);
void new_vbi (void);

unsigned long vbiints[] = 
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

void
init_vbi(void)
{
	int i;
	short nvbi;
	unsigned long *VBI_entry;

	bzero(vbiints, sizeof (vbiints));

#ifndef PRG_TEST
	nvbi = *(short *)nvbls;
	VBI_entry = (unsigned long *) *(unsigned long *)_vblqueue;

	*VBI_entry = (unsigned long)&new_vbi_wrapper;

#if 0
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

#else
	display("init_vbi() ignored - PRG_TEST\n");
#endif
	return;
}

short
get_vbitics(void)
{
	return (1000/50);
}

void
new_vbi(void)
{
	register unsigned long *vints = (unsigned long *)&vbiints;
	register void (*func)(void);

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

short
add_vbi_function(unsigned long function, unsigned long tics)
{
	int i;
	unsigned long *vints = (unsigned long *)&vbiints;

	for (i = 0; i < MAX_VINTS; i++)
	{
		if (!vints[2])
		{
#ifndef PRG_TEST
			short sr;

			sr = spl7();
#endif
			vints[0] = 0;
			vints[1] = tics;
			vints[2] = function;
#ifndef PRG_TEST
			spl(sr);
#endif
			return 0;
		}
		vints += 3;
	}
	return -1;
}

void
remove_vbi_function(unsigned long function)
{
#ifndef PRG_TEST
	short sr;
#endif
	int i;
	unsigned long *vints = (unsigned long *)&vbiints;

	for (i = 0; i < MAX_VINTS; i++)
	{
		if (vints[2] == function)
		{
#ifndef PRG_TEST
			sr = spl7();
#endif
			while (i < MAX_VINTS)
			{
				vints[0] = vints[0+3];
				vints[1] = vints[1+3];
				vints[2] = vints[2+3];
				vints++;
				i++;
			}

#ifndef PRG_TEST
			spl(sr);
#endif
			return;
		}
	}
	return;
}
