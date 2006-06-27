#include <mint/mintbind.h>
#include <fcntl.h>

#include "drivers/generic/8b_generic.h"
#include "modinf.h"
#include "ovdi_lib.h"
#include "ovdi_defs.h"
#include "includes/pf_15b_intel.h"
#include "includes/pf_16b_intel.h"
#include "includes/pf_24b_intel.h"
#include "includes/pf_32b_intel.h"

#include "xcb.h"
#include "acl.h"
#include "inlines.h"

void  _cdecl init (OVDI_LIB *, struct module_desc *ret, char *p, char *f);

static OVDI_DRIVER *	_cdecl dev_open			(OVDI_DEVICE *dev);
static long		_cdecl dev_close		(OVDI_DRIVER *drv);
static short		_cdecl dev_set_vdi_res		(OVDI_DRIVER *drv, short scrndev_id);
static short		_cdecl dev_get_res_info		(OVDI_DRIVER *drv);
static unsigned char *	_cdecl dev_setpscreen		(OVDI_DRIVER *drv, unsigned char *scrnadr);
static unsigned char *	_cdecl dev_setlscreen		(OVDI_DRIVER *drv, unsigned char *scrnadr);
static void		_cdecl dev_setcolor		(OVDI_DRIVER *drv, short pen, RGB_LIST *colors);
static void		_cdecl dev_vsync		(OVDI_DRIVER *drv);
static void		_cdecl dev_vreschk		(short x, short y);
static long		_cdecl et6k_sync		(void);

static short	Load_Resolution(char *fname, short index, RESOLUTION *res);
static void	do_set_res(OVDI_DRIVER *drv, RESOLUTION *res);

static short	boot_drive;
static OVDI_LIB	*lib;
static XCB	*xcb;

static char sname[] =	"Nova ET6000";
static char lname[] = 	"Nova ET6000 (using Nova emulator.prg\n" \
			"for ET6000)\n graphics driver for oVDI.\n" \
			"Copyright 2004 Odd Skancke for\n" \
			" AssemSoft Productions";

static char pn[128] = { "0" };
static char fn[64] = { "0" };

static OVDI_DEVICE et6k_device =
{
	NULL,
	0x00000001,
	sname,
	lname,
	pn,
	fn,
	dev_open,
	dev_close,
	dev_set_vdi_res,
	dev_get_res_info,
	dev_setpscreen,
	dev_setlscreen,
	dev_setcolor,
	dev_vsync,
	dev_vreschk,
	et6k_sync
};

static OVDI_DRIVER et6k_driver;

static OVDI_DRAWERS drw_1b;
static OVDI_DRAWERS drw_2b;
static OVDI_DRAWERS drw_4b;
static OVDI_DRAWERS drw_8b;
static OVDI_DRAWERS drw_15b;
static OVDI_DRAWERS drw_16b;
static OVDI_DRAWERS drw_24b;
static OVDI_DRAWERS drw_32b;

/* pf_xxx tables are used by vq_extend() to describe different pixel formats */
#if 0
static char pf_et6k[] =
{
	  6,  12,   1,
	 10, 255,   0,

	  6,   6,   1,
	 10, 255,   0,

	  6,   0,   1,
	 10, 255,   0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,

	/* Unused bits */
	 32, 255,  0,

	  0
};
#endif

static char pf_nova[] =
{
	  8,  16,   1,
	  8, 255,   0,

	  8,   8,   1,
	  8, 255,   0,

	  8,   0,   1,
	  8, 255,   0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,

	/* Unused bits */
	 32, 255,  0,

	  0
};

#if 0
static char pf_16b_et6k[] =
{
	  5,  11,  1,
	 11, 255,  0,

	  6,   5,  1,
	 11, 255,  0,

	  5,   0,  1,
	 11, 255,  0,

	 32, 255,  0,
	 32, 255,  0,

	/* Unused bits */
	 32, 255,  0,
	 
	  0
};
#endif
static RESFMT et6k_1b =
{
	1,
	1,
	PF_ATARI,
	-1,
	pf_nova,
};
static RESFMT et6k_8b =
{
	8,
	1,
	PF_PACKED,
	1,
	pf_nova,
};
static RESFMT et6k_15b =
{
	15,
	0,
	PF_PACKED,
	2,
	pf_15bI,
};
static RESFMT et6k_16b =
{
	16,
	0,
	PF_PACKED,
	2,
	pf_16bI,
};

static RESFMT et6k_24b = 
{
	24,
	0,
	PF_PACKED,
	3,
	pf_24bI,
};

static RESFMT et6k_32b =
{
	32,
	0,
	PF_PACKED,
	4,
	pf_32bI,
};


static long _cdecl
et6k_sync(void)
{
	volatile unsigned char *et6k = et6kptr;

	return inb(et6k, ACL_ACCELERATOR_STATUS) & 2 ? 0 : 1;
}

static void _cdecl
et6k_S_ONLY_8b(ROP_PB *rpb)
{
	if (!rpb->d_is_scrn || !rpb->s_is_scrn)
		(*rops_8b[3])(rpb);
	else
	{
		register volatile unsigned char *et6k;
		char *src, *dst;
		long w, h;
	
		et6k = et6kptr;

		w = rpb->sx2 - rpb->sx1;
		h = rpb->sy2 - rpb->sy1;

		src = (unsigned char *)rpb->s_addr + (long)(((long)rpb->sy1 * rpb->s_bypl) + (long)rpb->sx1);
		dst = (unsigned char *)rpb->d_addr + (long)(((long)rpb->dy1 * rpb->d_bypl) + (long)rpb->dx1);

		outb( et6k, ACL_FOREGROUND_RASTER_OPERATION,	204	);
		outb( et6k, ACL_STEPPING_INHIBIT, 		0	);
		outw( et6k, ACL_X_COUNT, 			w	);
		outw( et6k, ACL_Y_COUNT, 			h	);
		outw( et6k, ACL_DESTINATION_Y_OFFSET,		rpb->d_bypl - 1);
		outw( et6k, ACL_SOURCE_Y_OFFSET,		rpb->s_bypl - 1);

		if (src < dst)
		{
			outb( et6k, ACL_XY_DIRECTION, 3 );
			src += w + (h * rpb->s_bypl);
			dst += w + (h * rpb->d_bypl);
		}
		else
			outb( et6k, ACL_XY_DIRECTION, 0 );

		outl( et6k, ACL_SOURCE_ADDRESS, src );
		outb( et6k, ACL_SOURCE_WRAP, 0x77 );
		outl( et6k, ACL_DESTINATION_ADDRESS, dst );
	}
}
#if 0
static void _cdecl
et6k_filled_rect_16( RASTER *r, COLINF *c, VDIRECT *corners, VDIRECT *clip, PatAttr *ptrn)
{
	VDIRECT clipped = *corners;

	if ((*r->utils->clipbox)(&clipped, clip))
		return;
	else
	{
		unsigned char *addr, *patrn, *a;
		register long fcol, bcol;
		int left, right, groups;
		short x;
		int dx, xinc, dy;
		int planes, bypl;
		int bgcol, fgcol;
		int wrmode;
		unsigned short pattern, bit;
		int i, j;

		if (clipped.x1 > clipped.x2)
		{
			x = clipped.x1;
			clipped.x1 = clipped.x2;
			clipped.x2 = x;
		}

		x = clipped.x1;
		dx = clipped.x2 - clipped.x1 + 1;
		dy = clipped.y2 - clipped.y1 + 1;

		wrmode = ptrn->wrmode;
		fgcol = ptrn->color[wrmode];
		bgcol	= ptrn->bgcol[wrmode];
		bypl	= r->bypl;

		SYNC_RASTER(r);

		xinc = 2;
		a = (unsigned char *)r->base + ((long)x * xinc) + ((long)y1 * r->bypl);

		fcol = c->pixelvalues[fgcol];
		bcol = c->pixelvalues[bgcol];

		left = 16 - (x1 & 0xf);
		dx -= left;

		if ( dx <= 0 )
		{
			left = left + dx;
			groups = 0;
			right = 0;
		}
		else if (dx > 15)
		{
			right = (x2 & 0xf) + 1;
			groups = (dx - right) >> 4;
		}
		else
		{
			groups = 0;
			right = dx;
		}

		for (; dy > 0; dy--)
		{
			patrn = (unsigned char *)(long)ptrn->data + ((y1 % ptrn->height) * (ptrn->wwidth << 1));
			addr = a;

			if (left)
			{
				pattern = x1 & 0xf ? (*(unsigned short *)patrn << ((x1 & 0xf))) | ( *(unsigned short *)patrn >> (16 - (x1 & 0xf)) ) : *(unsigned short *)patrn;

				for (i = 0; i < left; i++)
				{
					if (pattern & 0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(addr, fcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(addr, bcol);
					addr += xinc;
					pattern <<= 1;
				}
			}

			if (groups)
			{
				bit = *(unsigned short *)patrn;

				for (i = 0; i < groups; i++)
				{
					pattern = bit;
					for (j = 0; j < 16; j++)
					{
						if (pattern & 0x8000)
						{
							if (dpf_fg)
								(*dpf_fg)(addr, fcol);
						}
						else if (dpf_bg)
							(*dpf_bg)(addr, bcol);
						addr += xinc;
						pattern <<= 1;
					}
				}
			}
			if (right)
			{
				pattern = *(unsigned short *)patrn;
				for (i = 0; i < right; i++)
				{
					if (pattern & 0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(addr, fcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(addr, bcol);
					addr += xinc;
					pattern <<= 1;
				}
			}
			y1++;
			a += bypl;
		}
	}
}
#endif

static short
Load_Resolution(char *fname, short res_index, RESOLUTION *res)
{
	short fd;
	long r;

	fd = Fopen(fname, O_RDONLY);
	if (fd < 0)
		return 0;

	if ((Fseek((sizeof(RESOLUTION) * res_index), fd, 0)) < 0)
	{
		Fclose(fd);
		return 0;
	}

	r = Fread( fd, sizeof(RESOLUTION), res);

	Fclose(fd);

	if (r == sizeof(RESOLUTION))
		return 1;
	else
		return 0;
}

extern void rb_S_ONLY_1b(ROP_PB *);

void _cdecl
init(OVDI_LIB *l, struct module_desc *ret, char *path, char *file)
{
	OVDI_DRIVER *drv = &et6k_driver;
	OVDI_DEVICE *dev = &et6k_device;

	if (!(*l->getcookie)((long)0x4e4f5641 /*"NOVA"*/, (long *)&xcb))
	{
		Cconws(" et6k_device_INIT: Could not locate NOVA cookie\n");
		return;
	}

	boot_drive = Dgetdrv() + 'a';

	lib	= l;
	{
		char *t;

		t = dev->pathname;
		while (*path)
			*t++ = *path++;
		*t = 0;
		t = dev->filename;
		while (*file)
			*t++ = *file++;
		*t = 0;
	}
	
/* Since things not implemented or provided are NULL, we clear all our structures */
	(*l->bzero)(&et6k_driver, sizeof(OVDI_DRIVER));

	(*l->bzero)(&drw_1b, sizeof(OVDI_DRAWERS));
	drv->drawers_1b = &drw_1b;
	drv->drawers_1b->raster_blits[3] = rb_S_ONLY_1b;
	drv->drawers_1b->resfmt = &et6k_1b;

	(*l->bzero)(&drw_2b, sizeof(OVDI_DRAWERS));
	drv->drawers_2b = &drw_2b;

	(*l->bzero)(&drw_4b, sizeof(OVDI_DRAWERS));
	drv->drawers_4b = &drw_4b;

	(*l->bzero)(&drw_8b, sizeof(OVDI_DRAWERS));
	drv->drawers_8b = &drw_8b;
	drv->drawers_8b->raster_blits[3] = et6k_S_ONLY_8b;
	drv->drawers_8b->resfmt = &et6k_8b;

	(*l->bzero)(&drw_15b, sizeof(OVDI_DRAWERS));
	drv->drawers_15b = &drw_15b;
	drv->drawers_16b->resfmt = &et6k_15b;

	(*l->bzero)(&drw_16b, sizeof(OVDI_DRAWERS));
	drv->drawers_16b = &drw_16b;
	drv->drawers_16b->resfmt = &et6k_16b;

	(*l->bzero)(&drw_24b, sizeof(OVDI_DRAWERS));
	drv->drawers_24b = &drw_24b;
	drv->drawers_24b->resfmt = &et6k_24b;

	(*l->bzero)(&drw_32b, sizeof(OVDI_DRAWERS));
	drv->drawers_32b = &drw_32b;
	drv->drawers_32b->resfmt = &et6k_32b;

	ret->types	= D_VHW;
	ret->vhw	= dev;
};

static OVDI_DRIVER * _cdecl
dev_open(OVDI_DEVICE *dev)
{
	OVDI_DRIVER *drv = &et6k_driver;
	RESOLUTION res;
	char fname[] = "c:\\auto\\ovdiboot.bib\0";

	drv->dev	= dev;

	if ( Load_Resolution((char *)&fname, 0, &res) )
		do_set_res(drv, &res);

	(void)dev_get_res_info(drv);

	return drv;
}

static long _cdecl
dev_close(OVDI_DRIVER *drv)
{
	RESOLUTION res;
	char fnam0[] = "c:\\auto\\ovdiboot.bib\0";
	char fnam1[] = "c:\\auto\\emulator.bib\0";

	if ( !(Load_Resolution((char *)&fnam0, 0, &res)) )
	{
		if ( !(Load_Resolution((char *)&fnam1, 0, &res)) )
			return 0;
	}

	do_set_res(drv, &res);
	(void)dev_get_res_info(drv);

	return (long)drv;
}

static short _cdecl
dev_set_vdi_res(OVDI_DRIVER *drv, short res_id)
{
	XCB *x = xcb;
	short res_index;
	RESOLUTION res;
	char fname[] = "c:\\auto\\sta_vdi.bib\0";

	if (res_id <= 0)
	{
		dev_get_res_info(drv);
		return 0;
	}
	else if (res_id == 1)
		res_index = (unsigned char)x->resolution;
	else
		res_index = res_id - 2;

	if ( !(Load_Resolution((char *)&fname, res_index, &res)) )
	{
		res_index = 0;
		if ( !(Load_Resolution((char *)&fname, res_index, &res)) )
			return -1;
	}

	do_set_res(drv, &res);

	(void)dev_get_res_info(drv);

	return res_index;
}

static void
do_set_res(OVDI_DRIVER *drv, RESOLUTION *res)
{
	XCB *x = xcb;
	long tmp;

	tmp	= (long)x->scr_base;
	tmp	-= (long)x->base;
	do_p_chres((long)x->p_chres, res, tmp);
}

/* WOOAHH - Fix the 'fmt' issue!! */
static short _cdecl
dev_get_res_info(OVDI_DRIVER *drv)
{
	short fmt, planes;
	XCB *x = xcb;

	/*
	 * Get information from the XCB cookie installed by Nova's EMULATOR.PRG
	*/
	drv->r.sync		= et6k_sync;
	planes			= x->planes;
	//drv->r.planes		= x->planes;
	drv->r.bypl		= x->bypl;
	drv->palette		= x->colors;
	drv->r.x1 = drv->r.y1	= 0;
	drv->r.x2		= x->max_x;
	drv->r.y2		= x->max_y;
	drv->r.w		= x->max_x + 1;
	drv->r.h		= x->max_y + 1;
	drv->v_top		= x->v_top;
	drv->v_bottom		= x->v_bottom;
	drv->v_left		= x->v_left;
	drv->v_right		= x->v_right;
	drv->vram_start		= x->base;
	drv->vram_size		= x->mem_size;
	drv->scr_size		= x->scrn_siz;
	drv->r.lenght		= x->scrn_siz;
	drv->r.base		= x->scr_base;
	drv->r.flags = drv->r.realflags = R_IS_SCREEN | R_IN_VRAM;

	fmt = *(short *)x->hw_flags;

	switch (planes) //(drv->r.planes)
	{
		case 1:
		{
			drv->r.resfmt		= *drv->drawers_1b->resfmt;
			break;
		}
#if 0
		case 4:
		{
			drv->r.resfmt		= *drv->drawers_b4->resfmt;
			drv->r.format		= PF_ATARI;
			drv->r.pixelformat	= pf_nova;
			drv->r.clut		= 1;
			drv->r.pixlen		= -4;
			break;
		}
#endif
		case 8:
		{
			drv->r.resfmt		= *drv->drawers_8b->resfmt;
			break;
		}
		case 15:
		{
			drv->r.resfmt		= *drv->drawers_15b->resfmt;
			break;
		}
		case 16:
		{
			drv->r.resfmt		= *drv->drawers_16b->resfmt;
			break;
		}
#if 0
		case 24:
		{
			drv->r.clut	= 0;

			if (!fmt & 1)
			{
				drv->r.format		= PF_PACKED | PF_BE;
				drv->r.pixelformat	= pf_24bM;
			}
			else
			{
				drv->r.format		= PF_PACKED;
				drv->r.pixelformat	= pf_24bI;
			}
			drv->r.pixlen	= 3;
			break;
		}
		case 32:
		{
			short format = ((fmt >> 1) & 3);

			drv->r.clut		= 0;

			if (!format)
			{
				drv->r.format		= PF_PACKED;
				drv->r.pixelformat	= pf_32bI;
			}
			else if (fmt == 1)
			{
				drv->r.format		= PF_PACKED | PF_BE;
				drv->r.pixelformat	= pf_32bM;
			}
			else
			{
				drv->r.format		= PF_PACKED | PF_BS;
				drv->r.pixelformat	= pf_32bIbs;
			}
			drv->r.pixlen	= 4;
			break;
		}
#endif
		default:
		{
			return -1;
		}
	}
	return 0;
}

static unsigned char * _cdecl
dev_setpscreen(OVDI_DRIVER *drv, unsigned char *scradr)
{
	if (scradr < (unsigned char *)drv->vram_start)
		return xcb->scr_base;
	else if (scradr > (unsigned char *)((long)drv->vram_start + drv->vram_size))
		return xcb->scr_base;

	do_p_setscr((long)xcb->p_setscr, scradr);
	return	xcb->scr_base;
}

static unsigned char * _cdecl
dev_setlscreen(OVDI_DRIVER *drv, unsigned char *logscr)
{
	v_bas_ad = (long)logscr;
	return logscr;
}

static void _cdecl
dev_setcolor(OVDI_DRIVER *drv, short pen, RGB_LIST *colors)
{
	unsigned char bcols[4];

	bcols[0] = (unsigned char)colors->red;
	bcols[1] = (unsigned char)colors->green;
	bcols[2] = (unsigned char)colors->blue;
	bcols[3] = 0;

	do_p_setcol((long)xcb->p_setcol, pen, (unsigned char *)&bcols[0]);
}

static void _cdecl
dev_vsync(OVDI_DRIVER *drv)
{
	do_p_vsync((long)xcb->p_vsync);
}

/* 
 * This will be called by the mouse-drivers (layer 1) mouse-interrupt whenever
 * the mouse moves.
*/
static void _cdecl
dev_vreschk(short x, short y)
{
	do_p_chng_vrt((long)xcb->p_chng_vrt, x, y);
}
