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

void	init	(OVDI_LIB *l, struct module_desc *ret, char *p, char *f);

extern void new_vbi_wrapper(void);
void new_vbi (void);

static O_Int	install(void);
static O_Int	get_vbitics(void);
static O_Int	add_vbi_function(O_u32 function, O_u32 tics);
static void	remove_vbi_function(O_u32 function);
static void	reset_vbi(void);
static void	enable_vbi(void);
static void	disable_vbi(void);

static char sname[] = "VBI driver (Atari standard)";
static char lname[] = "VBI driver for oVDI installing in the\nstandard '_vblqueue'";
static char fpath[128] = { "0" };
static char fname[64] = { "0" };

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
	fpath,
	fname,
	install,
	get_vbitics,
	add_vbi_function,
	remove_vbi_function,
	reset_vbi,
	enable_vbi,
	disable_vbi,
};

static O_u16 flags = 0;

static O_u32 vbiints[] = 
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
init(OVDI_LIB *l, struct module_desc *ret, char *path, char *file)
{
	VBIAPI *v = &vbiapi;
	char *t;

	L = l;
	ret->types = D_VBI;
	ret->vbi = (void *)v;

	t = v->pathname;
	while (*path)
		*t++ = *path++;
	*t = 0;
	t = v->filename;
	while (*file)
		*t++ = *file++;
	*t = 0;
}

/*
 * Install .. yes, we know what that does?
*/
static void I(void);

static O_Int
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
	O_16 nvbi;
	O_u32 *VBI_entry;

	if (!(flags & VBI_INST))
	{
		nvbi = *(O_16 *)nvbls;
		VBI_entry = (O_u32 *) *(O_u32 *)_vblqueue;

		for (i = 0; i < nvbi; i++)
		{
			if (*VBI_entry == 0)
			{
				O_16 sr;

				sr = spl7();
				*VBI_entry++ = (O_u32)&new_vbi_wrapper;
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
static O_Int
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
	register O_u32 *vints = (O_u32 *)&vbiints;
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

static O_Int
add_vbi_function(O_u32 function, O_u32 tics)
{
	int i;
	O_u32 *vints = (O_u32 *)&vbiints;

	for (i = 0; i < MAX_VINTS; i++)
	{
		if (!vints[2])
		{
			O_16 sr;

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
remove_vbi_function(O_u32 function)
{
	short sr;
	int i;
	O_u32 *vints = (O_u32 *)&vbiints;

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
