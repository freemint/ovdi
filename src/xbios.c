/*
* Not sure about this, but the color index passed to the xbios setcolor
* functions is the hardware register index and not vdi-pen index of the
* color to set, right?
*/

#include <osbind.h>
#include <mintbind.h>

#include "display.h"
#include "console.h"
#include "ovdi_defs.h"
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

static long oPhysbase		(OVDI_HWAPI *hw, short *p);
static long oLogbase		(OVDI_HWAPI *hw, short *p);
static long oGetrez		(OVDI_HWAPI *hw, short *p);
static long oSetscreen		(OVDI_HWAPI *hw, short *p);
static long oSetcolor		(OVDI_HWAPI *hw, short *p);
static long oSetpalette		(OVDI_HWAPI *hw, short *p);
static long oVsync		(OVDI_HWAPI *hw, short *p);
static long oCursconf		(OVDI_HWAPI *hw, short *p);

static long oEsetcolor		(OVDI_HWAPI *hw, short *p);
static long oEsetpalette	(OVDI_HWAPI *hw, short *p);
static long oEgetpalette	(OVDI_HWAPI *hw, short *p);

static short rel_16col_tab[] =
{
	0, 133, 267, 400, 533, 667, 800, 933,
	67, 200, 333, 467, 600, 733, 867, 1000
};

static OVDI_HWAPI *HW;

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
		old_trap14 = (long)Setexc(0x2e, new_xbioswr);
}

void
enable_xbios(OVDI_HWAPI *hw)
{
	HW = hw;
	return;
}
void
disable_xbios(void)
{
	HW = 0;
	return;
}

void
uninstall_xbios(void)
{
	if (old_trap14)
	{
		old_trap14 = (long)Setexc(0x2e, (long)old_trap14);
		old_trap14 = 0;
		HW = 0;
	}
	return;
}

long
new_xbios(short *p)
{
	long ret;
	short oc;
	long (*f)(OVDI_HWAPI *, short *);

	ret = 0xfacedaceL;

	if (HW)
	{
		oc = *p++;

		if (oc == 0x15)
			ret = oCursconf(HW, p);

		if (oc == 0x25)
			ret = oVsync(HW, p);

		if (oc >= 0x02 && oc <= 0x07)
		{
			f = xbios_2thru7[oc - 0x2];
			ret = (*f)(HW, p);
		}
		if (oc >= 0x53 && oc <= 0x55)
		{
			f = xbios_53thru55[oc - 0x53];
			ret = (*f)(HW, p);
		}
	}
	return ret;
}

static long
oCursconf(OVDI_HWAPI *hw, short *p)
{
	short mode;
	long ret;

	mode = *p++;
	ret = 0L;

	switch (mode)
	{
		case 0:
		{
			hide_text_cursor(hw->console);
			break;
		}
		case 1:
		{
			show_text_cursor(hw->console);
			break;
		}
		case 2:
		{
			(void)conf_textcursor_blink(hw->console, 1, 0xffff);
			break;
		}
		case 3:
		{
			(void)conf_textcursor_blink(hw->console, 0, 0xffff);
			break;
		}
		case 4:
		{
			(void)conf_textcursor_blink(hw->console, 0xffff, *p);
			break;
		}
		case 5:
		{
			ret = (long)conf_textcursor_blink(hw->console, 0xffff, 0xffff);
			break;
		}
	}
	return ret;
}

static long
oVsync(OVDI_HWAPI *hw, short *p)
{
	(*hw->driver->dev->vsync)(hw->driver);
	return 0L;
}
	
static long
oPhysbase(OVDI_HWAPI *hw, short *p)
{
	return (long)hw->driver->r.base;
}

static long
oLogbase(OVDI_HWAPI *hw, short *p)
{
	return (long)hw->driver->log_base;
}

static long
oGetrez(OVDI_HWAPI *hw, short *p)
{
	return 2L;
}

static long
oSetscreen(OVDI_HWAPI *hw, short *p)
{
	short mode;
	unsigned long logic, phys;
	OVDI_DEVICE *d;

	logic	= (unsigned long)(((unsigned long)p[0] << 16) | (unsigned short)p[1]);
	phys	= (unsigned long)(((unsigned long)p[2] << 16) | (unsigned short)p[3]);
	mode	= p[4];
	d	= hw->driver->dev;

	if (logic != 0xffffffffUL)
		hw->driver->log_base = (*d->setlscr)(hw->driver, (unsigned char *)logic);

	if (phys != 0xffffffffUL)
	{
		if ((phys < (unsigned long)hw->driver->vram_start) || (phys > (unsigned long)((unsigned long)hw->driver->vram_start + hw->driver->vram_size)))
			return 0;

		hw->driver->r.base = (*d->setpscr)(hw->driver, (unsigned char *)phys);
	}
	
	return 0;
}

static long
oSetpalette(OVDI_HWAPI *hw, short *p)
{
	short *pal;
	short red, green, blue, i, hwpen;
	COLINF *c;
	RASTER *r;
	OVDI_DEVICE *dev;
	RGB_LIST relative;

	c	= hw->colinf;
	r	= &hw->driver->r;
	dev	= hw->driver->dev;

	relative.alpha = relative.ovl = 0;

	pal	= (short *)(((unsigned long)p[0] << 16) | (unsigned short)p[1]);

	for (i = 0; i < 16; i++)
	{
		red	= ((*pal >> 7) & 0xe) | ((*pal >> 11) & 1);
		green	= ((*pal >> 3) & 0xe) | ((*pal >> 7) & 1);
		blue	= ((*pal << 1) & 0xe) | ((*pal >> 3) & 1);

		relative.red	= rel_16col_tab[red];
		relative.green	= rel_16col_tab[green];
		relative.blue	= rel_16col_tab[blue];

		hwpen	= calc_vdicolor(r, c, c->color_hw2vdi[i], &relative);

		if (hwpen >= 0 && r->clut)
			(*dev->setcol)(hw->driver, hwpen, &c->actual_rgb[hwpen]);

		pal++;
	}
	return 0L;
}

long
oSetcolor(OVDI_HWAPI *hw, short *p)
{
	short red, green, blue, col, idx, old;
	COLINF *c;
	RASTER *r;
	RGB_LIST relative;
	
	idx = *p++;

	/* ozk: I dont know how to react to an 'out of bounds' color index...
	 * .. so I just make it return -1 for now.
	*/
	if (idx < 0 || idx > 255)
		return -1;

	c	= hw->colinf;
	r	= &hw->driver->r;
	col	= *p;

	/* Calculate the old color value */
	red	=  ((long)c->request_rgb[idx].red * 16) / 1000;
	green	=  ((long)c->request_rgb[idx].green * 16) / 1000;
	blue	=  ((long)c->request_rgb[idx].blue * 16) / 1000;
	old	=  ((red & 0xe) << 7) | ((red & 1) << 11);
	old	|= ((green & 0xe) << 3) | ((green & 1) << 7);
	old	|= ((blue & 0xe) >> 1) | ((blue & 1) << 3);

	if (col >= 0) /* Do we set a new value? (Negative value == NO) */
	{
		/* Calculate  the new value */
		red	= ((col >> 7) & 0xe) | ((col >> 11) & 1);
		green	= ((col >> 3) & 0xe) | ((col >> 7) & 1);
		blue	= ((col << 1) & 0xe) | ((col >> 3) & 1);

		relative.red	= rel_16col_tab[red];
		relative.green	= rel_16col_tab[green];
		relative.blue	= rel_16col_tab[blue];

		idx	= calc_vdicolor(r, c, c->color_hw2vdi[idx], &relative);

		if (idx >= 0 && r->clut)
			(*hw->driver->dev->setcol)(hw->driver, idx, &c->actual_rgb[idx]);
	}

	return (long)old;
}

static long
oEsetcolor(OVDI_HWAPI *hw, short *p)
{
	short red, green, blue, col, idx, old;
	COLINF *c;
	RASTER *r;
	RGB_LIST relative;

	idx = *p++;

	c	= hw->colinf;
	r	= &hw->driver->r;
	col	= *p;

	/* ozk: I dont know how to react to an 'out of bounds' color index...
	 * .. so I just make it return -1 for now.
	*/
	if (idx < 0 || idx > 255)
		return -1;

	/* Calculate the old color value */
	red	=  ((long)c->request_rgb[idx].red * 16) / 1000;
	green	=  ((long)c->request_rgb[idx].green * 16) / 1000;
	blue	=  ((long)c->request_rgb[idx].blue * 16) / 1000;
	old	=  (red & 0xf) << 8;
	old	|= (green & 0xf) << 4;
	old	|= blue & 0xf;

	if (col >= 0)
	{
		/* Then the new value */
		red	= (col >> 8) & 0xf;
		green	= (col >> 4) & 0xf;
		blue	= col & 0xf;

		relative.red	= rel_16col_tab[red];
		relative.green	= rel_16col_tab[green];
		relative.blue	= rel_16col_tab[blue];

		idx = calc_vdicolor(r, c, c->color_hw2vdi[idx], &relative);

		if (idx >= 0 && r->clut)
			(*hw->driver->dev->setcol)(hw->driver, idx, &c->actual_rgb[idx]);
	}

	return (long)old;
}

static long
oEsetpalette(OVDI_HWAPI *hw, short *p)
{
	short *pal;
	COLINF *c;
	RASTER *r;
	OVDI_DEVICE *dev;
	short red, green, blue, i, cnt, idx, col, hwpen;
	RGB_LIST relative;

	idx = *p++;			/* start index */
	cnt = *p++;			/* count */

	if ((idx + cnt) > 255)
	{
		cnt = 255 - idx;
		if (idx < 0)
			return 0L;
	}

	c	= hw->colinf;
	r	= &hw->driver->r;
	dev	= hw->driver->dev;

	relative.alpha = relative.ovl = 0;
	pal = (short *)(((unsigned long)p[0] << 16) | (unsigned short)p[1]);

	for (i = 0; i < cnt; i++)
	{
		col	= *pal++;
		red	= (col >> 8) & 0xf;
		green	= (col >> 4) & 0xf;
		blue	= col & 0xf;

		relative.red	= rel_16col_tab[red];
		relative.green	= rel_16col_tab[green];
		relative.blue	= rel_16col_tab[blue];

		hwpen = calc_vdicolor(r, c, c->color_hw2vdi[idx], &relative);

		if (hwpen >= 0 && r->clut)
			(*dev->setcol)(hw->driver, hwpen, &c->actual_rgb[hwpen]);
		idx++;
	}
	return 0L;
}

static long
oEgetpalette(OVDI_HWAPI *hw, short *p)
{
	COLINF *c;
	short red, green, blue, i, idx, cnt, old;
	short *pal;

	idx = *p++;
	cnt = *p++;

	if ((idx + cnt) > 255)
	{
		cnt = 255 - idx;
		if (idx < 0)
			return 0L;
	}

	c = hw->colinf;
	pal = (short *)(((unsigned long)p[0] << 16) | (unsigned short)p[1]);

	for (i = 0; i < cnt; i++)
	{
		/* Calculate the old color value */
		red	=  ((long)c->request_rgb[idx].red * 16) / 1000;
		green	=  ((long)c->request_rgb[idx].green * 16) / 1000;
		blue	=  ((long)c->request_rgb[idx].blue * 16) / 1000;
		old	=  (red & 0xf) << 8;
		old	|= (green & 0xf) << 4;
		old	|= blue & 0xf;

		*pal++ = old;
		idx++;
	}
	return 0L;
}
