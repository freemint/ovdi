#include <osbind.h>
#include <mintbind.h>

#include "display.h"
#include "console.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "ovdi_types.h"
#include "vdi_defs.h"
#include "xbios.h"
#include "v_attribs.h"
#include "../../sys/mint/arch/asm_spl.h"

#define	vec_trap14	0xb8
extern short logit;
extern void	new_xbioswr(void);
extern unsigned long old_trap14;

long new_xbios		(short *p);

static long oPhysbase		(VIRTUAL *v, short *p);
static long oLogbase		(VIRTUAL *v, short *p);
static long oGetrez		(VIRTUAL *v, short *p);
static long oSetscreen		(VIRTUAL *v, short *p);
static long oSetcolor		(VIRTUAL *v, short *p);
static long oSetpalette	(VIRTUAL *v, short *p);
static long oVsync		(VIRTUAL *v, short *p);
static long oCursconf		(VIRTUAL *v, short *p);

static long oEsetcolor		(VIRTUAL *v, short *p);
static long oEsetpalette	(VIRTUAL *v, short *p);
static long oEgetpalette	(VIRTUAL *v, short *p);

static short rel_16col_tab[] =
{
	0, 133, 267, 400, 533, 667, 800, 933,
	67, 200, 333, 467, 600, 733, 867, 1000
};


static VIRTUAL *V = 0;

static void *xbios_2thru7[] = 
{
	oPhysbase,
	oLogbase,
	oGetrez,
	oSetscreen,
	oSetpalette,
	oSetcolor
};

static void *xbios_53thru55[] =
{
	oEsetcolor,
	oEsetpalette,
	oEgetpalette
};

void
install_xbios(void)
{
	if (!old_trap14)
	{
		old_trap14 = (long)Setexc(0x2e, new_xbioswr);
	//	sr = spl7();
	//	old_trap14 = *(long *)vec_trap14;
	//	*(unsigned long *)vec_trap14 = (unsigned long)&new_xbioswr;
	//	spl(sr);
	}
	return;
}

void
enable_xbios(VIRTUAL *v)
{
	V = v;
	return;
}
void
disable_xbios(void)
{
	V = 0;
	return;
}

void
uninstall_xbios(void)
{
	//short sr;

	if (old_trap14)
	{
		old_trap14 = (long)Setexc(0x2e, (long)old_trap14);
		old_trap14 = 0;
	//	sr = spl7();
	//	*(unsigned long *)vec_trap14 = old_trap14;
	//	old_trap14 = 0;
	//	spl(sr);
	}
	return;
}

long
new_xbios(short *p)
{
	long ret;
	short oc;
	short pid;
	long (*f)(VIRTUAL *, short *);

	ret = 0xfacedaceL;

	if (V)
	{
//		pid = Pgetpid();
//		log("XBIOS: pid %d ", pid);
	
		oc = *p++;

		if (oc == 0x15)
			ret = oCursconf(V, p);

		if (oc == 0x25)
			ret = oVsync(V, p);

		if (oc >= 0x02 && oc <= 0x07)
		{
			f = xbios_2thru7[oc - 0x2];
			ret = (*f)(V, p);
		}
		if (oc >= 0x53 && oc <= 0x55)
		{
			f = xbios_53thru55[oc - 0x53];
			ret = (*f)(V, p);
		}

#if 1
		if (ret != 0xfacedaceL && logit)
		{
			pid = Pgetpid();
			p--;
			log("XBIOS: (%d) - fc %x, p1 %x, p2 %x, p3 %x, p4 %x .. return %lx\n", pid, p[0], p[1], p[2], p[3], p[4], ret);
		}
#endif
	}
		
	return ret;
}

static long
oCursconf(VIRTUAL *v, short *p)
{
	short mode;
	long ret;

	mode = *p++;
	ret = 0L;

	switch (mode)
	{
		case 0:
		{
			hide_text_cursor(v->con);
			break;
		}
		case 1:
		{
			show_text_cursor(v->con);
			break;
		}
		case 2:
		{
			(void)conf_textcursor_blink(v->con, 1, 0xffff);
			break;
		}
		case 3:
		{
			(void)conf_textcursor_blink(v->con, 0, 0xffff);
			break;
		}
		case 4:
		{
			(void)conf_textcursor_blink(v->con, 0xffff, *p);
			break;
		}
		case 5:
		{
			ret = (long)conf_textcursor_blink(v->con, 0xffff, 0xffff);
			break;
		}
	}
	return ret;
}

static long
oVsync(VIRTUAL *v, short *p)
{
	(*v->driver->dev->vsync)(v->driver);
	return 0L;
}
	
static long
oPhysbase(VIRTUAL *v, short *p)
{
	return (long)v->driver->r.base;
}

static long
oLogbase(VIRTUAL *v, short *p)
{
	return (long)v->driver->log_base;
}

static long
oGetrez(VIRTUAL *v, short *p)
{
	return 2L;
}

static long
oSetscreen(VIRTUAL *v, short *p)
{
	short mode;
	unsigned long logic, phys;
	OVDI_DEVICE *d;

	logic = (unsigned long)(((unsigned long)p[0] << 16) | (unsigned short)p[1]);
	phys = (unsigned long)(((unsigned long)p[2] << 16) | (unsigned short)p[3]);
	mode = p[4];
	d = v->driver->dev;

	if (logic != 0xffffffffUL)
		v->driver->log_base = (*d->setlscr)(v->driver, (unsigned char *)logic);

	if (phys != 0xffffffffUL)
	{
		if ((phys < (unsigned long)v->driver->vram_start) || (phys > (unsigned long)((unsigned long)v->driver->vram_start + v->driver->vram_size)))
			return 0;

		v->driver->r.base = (*d->setpscr)(v->driver, (unsigned char *)phys);
	}
	return 0;
}

static long
oSetpalette(VIRTUAL *v, short *p)
{
	short *pal;
	short red, green, blue, i;
	RGB_LIST relative;

	relative.alpha = 0;
	relative.ovl = 0;

	pal = (short *)(((unsigned long)p[0] << 16) | (unsigned short)p[1]);

	for (i = 0; i < 16; i++)
	{
		red	= ((*pal >> 7) & 0xe) | ((*pal >> 11) & 1);
		green	= ((*pal >> 3) & 0xe) | ((*pal >> 7) & 1);
		blue	= ((*pal << 1) & 0xe) | ((*pal >> 3) & 1);

		relative.red = rel_16col_tab[red];
		relative.green = rel_16col_tab[green];
		relative.blue = rel_16col_tab[blue];

		lvs_color(v, i, &relative);
		pal++;
	}
	return 0L;
}

long
oSetcolor(VIRTUAL *v, short *p)
{
	short red, green, blue, col, idx, old;
	RGB_LIST relative;

	idx = *p++;
	col = *p;

	/* Calculate the old color value */
	red = ((long)v->request_rgb[idx].red * 16) / 1000;
	green = ((long)v->request_rgb[idx].green * 16) / 1000;
	blue = ((long)v->request_rgb[idx].blue * 16) / 1000;
	old = ((red & 0xe) << 7) | ((red & 1) << 11);
	old |= ((green & 0xe) << 3) | ((green & 1) << 7);
	old |= ((blue & 0xe) >> 1) | ((blue & 1) << 3);

	/* Then the new value */
	red	= ((col >> 7) & 0xe) | ((col >> 11) & 1);
	green	= ((col >> 3) & 0xe) | ((col >> 7) & 1);
	blue	= ((col << 1) & 0xe) | ((col >> 3) & 1);

	relative.red = rel_16col_tab[red];
	relative.green = rel_16col_tab[green];
	relative.blue = rel_16col_tab[blue];

	lvs_color(v, idx, &relative);

	return (long)old;
	return 0L;
}

static long
oEsetcolor(VIRTUAL *v, short *p)
{
	short red, green, blue, col, idx, old;
	RGB_LIST relative;

	idx = *p++;
	col = *p;

	/* Calculate the old color value */
	red = ((long)v->request_rgb[idx].red * 16) / 1000;
	green = ((long)v->request_rgb[idx].green * 16) / 1000;
	blue = ((long)v->request_rgb[idx].blue * 16) / 1000;
	old = (red & 0xf) << 8;
	old |= (green & 0xf) << 4;
	old |= blue & 0xf;

	/* Then the new value */
	red	= (col >> 8) & 0xf;
	green	= (col >> 4) & 0xf;
	blue	= col & 0xf;

	relative.red = rel_16col_tab[red];
	relative.green = rel_16col_tab[green];
	relative.blue = rel_16col_tab[blue];

	lvs_color(v, idx, &relative);

	return (long)old;
}

static long
oEsetpalette(VIRTUAL *v, short *p)
{
	short *pal;
	short red, green, blue, i, cnt, idx, col;
	RGB_LIST relative;

	relative.alpha = 0;
	relative.ovl = 0;

	idx = *p++;			/* start index */
	cnt = *p++;			/* count */

	if ((idx + cnt) > 255)
	{
		cnt = 255 - idx;
		if (idx < 0)
			return 0L;
	}

	pal = (short *)(((unsigned long)p[0] << 16) | (unsigned short)p[1]);

	for (i = 0; i < cnt; i++)
	{
		col	= *pal++;
		red	= (col >> 8) & 0xf;
		green	= (col >> 4) & 0xf;
		blue	= col & 0xf;

		relative.red = rel_16col_tab[red];
		relative.green = rel_16col_tab[green];
		relative.blue = rel_16col_tab[blue];

		lvs_color(v, idx, &relative);
		idx++;
	}
	return 0L;
}

static long
oEgetpalette(VIRTUAL *v, short *p)
{
	short red, green, blue, i, idx, cnt, old;
	short *pal;

	idx = *p++;
	cnt = *p++;
	pal = (short *)(((unsigned long)p[0] << 16) | (unsigned short)p[1]);

	if ((idx + cnt) > 255)
	{
		cnt = 255 - idx;
		if (idx < 0)
			return 0L;
	}

	for (i = 0; i < cnt; i++)
	{
		/* Calculate the old color value */
		red = ((long)v->request_rgb[idx].red * 16) / 1000;
		green = ((long)v->request_rgb[idx].green * 16) / 1000;
		blue = ((long)v->request_rgb[idx].blue * 16) / 1000;
		old = (red & 0xf) << 8;
		old |= (green & 0xf) << 4;
		old |= blue & 0xf;

		*pal++ = old;
		idx++;
	}
	return 0L;
}
