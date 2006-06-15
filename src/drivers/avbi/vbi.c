#include <osbind.h>
#include <stddef.h>

#include "vbiapi.h"

#include "ovdi_lib.h"
#include "modinf.h"
#include "ovdi_types.h"
#include "../../sys/mint/arch/asm_spl.h"

#define	_vblqueue	0x456L
#define nvbls		0x454L

#define MAX_VBIINTS 10
#define VBI_INST	1
#define VBI_ENABLE	2

void _cdecl init	(OVDI_LIB *l, struct module_desc *ret, char *p, char *f);

extern void new_vbi_wrapper(void);
void new_vbi (void);

static short _cdecl	install(void);
static short _cdecl	get_vbitics(void);
static short _cdecl	add_vbi_function(unsigned long function, unsigned long tics, long arg);
static void _cdecl	remove_vbi_function(unsigned long function, long arg);
static void _cdecl	reset_vbi(void);
static void _cdecl	enable_vbi(void);
static void _cdecl	disable_vbi(void);

static char sname[] = "VBI driver (Atari standard)";
static char lname[] = "VBI driver for oVDI installing in the\nstandard '_vblqueue'";
static char fpath[128] = { "0" };
static char fname[64] = { "0" };

static OVDI_LIB	*L = NULL;

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


static unsigned short flags = 0;
struct vbi_entry
{
	long ticks;
	long rticks;
	void _cdecl (*func)(long arg);
	long arg;
};
static struct vbi_entry VBIints[] =
{
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
};
/*
 * oVDI calls us here right after we've been loaded.
*/
void _cdecl
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

static short _cdecl
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
static void _cdecl
reset_vbi(void)
{
	disable_vbi();
	(*L->bzero)(VBIints, sizeof(VBIints)); //(vbiints, sizeof(vbiints));
}

/*
 * Enable the VB interrupts
*/
static void _cdecl
enable_vbi(void)
{
	if ((flags & VBI_INST))
		flags |= VBI_ENABLE;
}

/*
 * Turn off VB interrupts
*/
static void _cdecl
disable_vbi(void)
{
	flags &= ~VBI_ENABLE;
}

/*
 * return
*/
static short _cdecl
get_vbitics(void)
{
	return (1000/50);
}

/*
 * High level VBI handler. This calls out the installed functions.
*/
void _cdecl
new_vbi(void)
{
	register int i;
	register struct vbi_entry *vints;

	if (!(flags & VBI_ENABLE))
		return;

	vints = VBIints;

	for (i = 0; i < MAX_VBIINTS; i++) {
		if (vints[i].func) {
			if (!vints[i].rticks) {
				vints[i].rticks = vints[i].ticks;
				(*vints[i].func)(vints[i].arg);
			}
			else
				vints[i].rticks--;
		}
		else
			break;
	}
}

static short _cdecl
add_vbi_function(unsigned long function, unsigned long tics, long arg)
{
	int i;
	struct vbi_entry *vints = VBIints;

	for (i = 0; i < MAX_VBIINTS; i++) {
		if (!vints[i].func) {
			short sr;
			sr = spl7();
			vints[i].rticks = 0;
			vints[i].ticks = tics;
			vints[i].func = (void *)function;
			vints[i].arg = arg;
			spl(sr);
			return 0;
		}
	}
	return -1;
}

static void _cdecl
remove_vbi_function(unsigned long function, long arg)
{
	short sr;
	int i;
	struct vbi_entry *vints = VBIints;

	for (i = 0; i < MAX_VBIINTS; i++) {
		if (vints[i].func == (void *)function && vints[i].arg == arg) {
			sr = spl7();
			while (i < MAX_VBIINTS) {
				vints[i] = vints[i + 1];
				i++;
			}
			spl(sr);
			break;
		}
	}
}
