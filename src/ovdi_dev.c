#include <mint/mintbind.h>
#include <fcntl.h>

#include "display.h"
#include "libkern.h"
#include "vdi_defs.h"
#include "ovdi_dev.h"
#include "xcb.h"
#include "std_driver.h"

static OVDI_DRIVER *	ovdidev_open		(OVDI_DEVICE *dev, short dev_id);

static long		ovdidev_close		(OVDI_DRIVER *drv);
static unsigned char *	ovdidev_setpscreen	(OVDI_DRIVER *drv, unsigned char *scrnadr);
static unsigned char *	ovdidev_setlscreen	(OVDI_DRIVER *drv, unsigned char *scrnadr);
static void		ovdidev_setcolor	(OVDI_DRIVER *drv, short pen, RGB_LIST *colors);
static void		ovdidev_vsync		(OVDI_DRIVER *drv);
static void		ovdidev_vreschk		(short x, short y);

static short	Load_Resolution(short index, RESOLUTION *res);
static short	boot_drive;
int get_cookie(long tag, long *ret);

static OVDI_LIB	*lib;

static XCB	*xcb;

static char ovdidev_name[20] = {"oVDI Device Driver"};

static OVDI_DEVICE ovdidev =
{
	(long)"0.01",
	ovdidev_name,
	ovdidev_open,
	ovdidev_close,
	ovdidev_setpscreen,
	ovdidev_setlscreen,
	ovdidev_setcolor,
	ovdidev_vsync,
	ovdidev_vreschk,
};

static OVDI_DRIVER driver;

static OVDI_DRAWERS drw_1b;
static OVDI_DRAWERS drw_2b;
static OVDI_DRAWERS drw_4b;
static OVDI_DRAWERS drw_8b;
static OVDI_DRAWERS drw_15b;
static OVDI_DRAWERS drw_16b;
static OVDI_DRAWERS drw_24b;
static OVDI_DRAWERS drw_32b;

/* pf_xxx tables are used by vq_extend() to describe different pixel formats */

/* ST palette register layout */
static char pf_st[] =
{
	/* red bits */
	  3,   8,  1,
	 13, 255,  0,

	/* green bits */
	  3,   4,  1,
	 13, 255,  0,

	/* blue bits */
	  3,   0,  1,
	 13, 255,  0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,
	/* Unused bits */
	  1,   3,  1,
	  1,   7,  1,
	  5,  11,  1,
	 25, 255,  0,

	  0
};

/* STe palette register layout */
static char pf_ste[] =
{
	/* red bits */
	  1,  11,  1,
	  3,   8,  1,
	 12, 255,  0,

	/* green bits */
	  1,   7,  1,
	  3,   4,  1,
	 12, 255,  0,

	/* blue bits */
	  1,   3,  1,
	  3,   0,  1,
	 12, 255,  0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,
	/* Unused bits */
	  5,  11,  1,
	 27, 255,  0,

	  0
};

/* TT palette register layout */
static char pf_tt[] =
{
	/* red bits */
	  4,   8,  1,
	 12, 255,  0,

	/* green bits */
	  4,   4,  1,
	 12, 255,  0,

	/* blue bits */
	  4,   0,  1,
	 12, 255,  0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,
	/* Unused bits */
	  5,  11,  1,
	 27, 255,  0,
	  0
};

/* Falcon palette register layout */
static char pf_falcon[] =
{
	/* red bits */
	  6,  26,  1,
	 10, 255,  0,

	/* green bits */
	  6,  18,  1,
	 10, 255,  0,

	/* blue bits */
	  6,   2,  1,
	 10, 255,  0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,

	/* Unused bits */
	  2,   0,  1,
	 10,   8,  1,
	  2,  24,  1,
	 18, 255,  0,

	  0
};


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

/* Falcon's 15-bit pixelformat bit layout */
static char pf_15b_falc[] =
{
	  5,  11,   1,		/*  5 red bits start at bit 11. Add one to get to next red bit */
	 11, 255,   0,		/* 11 unused red bits followin the above red bits */

	  5,   6,   1,		/* 5 green bits start at bit 6 */
	 11, 255,   0,		/* 11 unused red bits */
#if 0
	  3,   8,   1,		/* 3 green bits start at bit 8 */
	  2,   6,   1,		/* 2 green bits start at bit 6 */
	 11, 255,   0,		/* 11 unused green bits */
#endif

	  5,   0,   1,		/* 5 blue bits start at bit 0 */
	 11, 255,   0,		/* 11 unused blue bits */

	 32, 255,   0,		/* 32 unused alpha channel bits */

	  1,   5,   1,		/* 1 overlay bit starting at bit 5 */
	 31, 255,   0,		/* 31 unused genlock/overlay bits */

	 32, 255,   0,

	 0			/* end of table */
};

/* 15-bit Intel pixelformat layout */
static char pf_15bI[] =
{
	/* red bits */
	  5,   2,  1,
	 11, 255,  0,

	/* green bits */
	  3,  13,  1,
	  2,   0,  1,
	 11, 255,  0,

	/* blue bits */
	  5,   8,  1,
	 11, 255,  0,

	/* Alpha bits */
	 32, 255,  0,

	/* Genlock/overlay bits */
	  1,   7,  1,
	 31, 255,  0,

	/* Unused bits */
	 32, 255,  0,

	  0
};

/* 16-bit Intel pixelformat layout */
static char pf_16bI[] =
{
	/* red bits */
	  5,   3,  1,
	 11, 255,  0,

	/* green bits */
	  3,  13,  1,
	  3,   0,  1,
	 10, 255,  0,

	/* blue bits */
	  5,   8,  1,
	 11, 255,  0,

	/* Alpha + genlock/overlay bits */
	 64, 255,  0,

	/* Unused bits */
	 32, 255,  0,

	  0
};
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

/* 24-bit Intel pixelformat layout */
static char pf_24bI[] =
{
	/* red bits */
	  8,   0,  1,
	  8, 255,  0,
	/* green bits */
	  8,   8,  1,
	  8, 255,  0,
	/* blue bits */
	  8,  16,  1,
	  8, 255,  0,
	/* Alpha + genlock/overlay bits */
	 64, 255,  0,
	/* Unused bits */
	 32, 255,  0,
	/* end */
	  0	  
};
/* 24-bit Motorola pixelformat layout */
static char pf_24bM[] =
{
	/* red bits */
	  8,  16,  1,
	  8, 255,  0,

	/* green bits */
	  8,   8,  1,
	  8, 255,  0,

	/* blue bits */
	  8,   0,  1,
	  8, 255,  0,

	/* Alpha + genlock/overlay bits */
	 64, 255,  0,
	/* Unused bits */
	 32, 255,  0,

	  0
};

/* 32-bit Intel pixelformat layout */
static char pf_32bI[] = 
{
	/* red bits */
	  8,  24,  1,
	  8, 255,  0,
	/* green bits */
	  8,  16,  1,
	  8, 255,  0,
	/* blue bits */
	  8,   8,  1,
	  8, 255,  0,
	/* Alpha bits */
	 32, 255,  0,
	/* genlock/overlay bits */
	 32, 255,  0,
	/* Unused bits */
	  8,   0,  1,
	 24, 255,  0,
	
	  0
};

/* 32-bit Motorola pixelformat layout */
static char pf_32bM[] = 
{
	/* red bits */
	  8,  16,  1,
	  8, 255,  0,
	/* green bits */
	  8,   8,  1,
	  8, 255,  0,
	/* blue bits */
	  8,   0,  1,
	  8, 255,  0,
	/* Alpha bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,
	/* Unused bits */
	  8,  24,  1,
	 24, 255,  0,

	  0
};

/* 32-bit byteswapped Intel pixelformat layout */
static char pf_32bIbs[] =
{
	/* red bits */
	  8,   0, 1,
	  8, 255, 0,
	/* green bits */
	  8,   8, 1,
	  8, 255, 0,
	/* blue bits */
	  8,  16, 1,
	  8, 255, 0,
	/* Alpha bits */
	 32, 255, 0,
	/* genlock/overlay bits */
	 32, 255, 0,
	/* Unused bits */
	  8,  24, 1,
	 24, 255, 0,

	  0
};

static short
Load_Resolution(short res_index, RESOLUTION *res )
{
	
	char fname[] = "c:\\auto\\sta_vdi.bib\0";
	short fd;
	long r;

	//fname[0] = boot_drive;
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

OVDI_DEVICE *
device_init(OVDI_LIB *l)
{
	OVDI_DRIVER *drv = &driver;

	if (!(*l->getcookie)((long)0x4e4f5641 /*"NOVA"*/, (long *)&xcb))
	{
		Cconws(" oVDIDEV_INIT: Could not locate NOVA cookie\n");
		return 0;
	}

	boot_drive = Dgetdrv() + 'a';

	lib	= l;

/* Since things not implemented or provided are NULL, we clear all our structures */
	bzero(&driver, sizeof(OVDI_DRIVER));

	bzero(&drw_1b, sizeof(OVDI_DRAWERS));
	drv->drawers_1b = &drw_1b;

	bzero(&drw_2b, sizeof(OVDI_DRAWERS));
	drv->drawers_2b = &drw_2b;

	bzero(&drw_4b, sizeof(OVDI_DRAWERS));
	drv->drawers_4b = &drw_4b;

	bzero(&drw_8b, sizeof(OVDI_DRAWERS));
	drv->drawers_8b = &drw_8b;

	bzero(&drw_15b, sizeof(OVDI_DRAWERS));
	drv->drawers_15b = &drw_15b;

	bzero(&drw_16b, sizeof(OVDI_DRAWERS));
	drv->drawers_16b = &drw_16b;

	bzero(&drw_24b, sizeof(OVDI_DRAWERS));
	drv->drawers_24b = &drw_24b;

	bzero(&drw_32b, sizeof(OVDI_DRAWERS));
	drv->drawers_32b = &drw_32b;

	return &ovdidev;
};

static OVDI_DRIVER *
ovdidev_open(OVDI_DEVICE *dev, short dev_id)
{
	XCB *x = xcb;
	OVDI_DRIVER *drv = &driver;
	RESOLUTION res;
	long tmp;
	short fmt, res_index;

	res_index = (unsigned char)x->resolution;

	if ( !(Load_Resolution(res_index, &res)) )
	{
		log("No resolution??\n");
		return 0;
	}

	tmp =	(long)x->scr_base;
	tmp -=	(long)x->base;

	do_p_chres((long)x->p_chres, &res, tmp);

	drv->r.planes		= x->planes;
	drv->r.bypl		= x->bypl;
	drv->palette		= x->colors;
	drv->r.w		= x->max_x + 1;
	drv->r.h		= x->max_y + 1;
	drv->v_top		= x->v_top;
	drv->v_bottom		= x->v_bottom;
	drv->v_left		= x->v_left;
	drv->v_right		= x->v_right;
	drv->vram_start		= x->base;
	drv->vram_size		= x->mem_size;
	drv->scr_size		= x->scrn_siz;
	drv->r.base		= x->scr_base;
	drv->dev		= dev;
	drv->r.flags		= R_IS_SCREEN | R_IN_VRAM;

	if (drv->r.planes == 1)
	{
		drv->r.format		= PF_ATARI;
		drv->r.clut		= 1;
		drv->r.pixlen		= -1;
	}
	else if (drv->r.planes == 4)
	{
		drv->r.format		= PF_ATARI;
		drv->r.clut		= 1;
		drv->r.pixlen		= -4;
	}
	else if (drv->r.planes == 8)
	{
		drv->r.format		= PF_PACKED;
		drv->r.pixelformat	= pf_nova;
		drv->r.clut		= 1;
		drv->r.pixlen		= 1;
	}
	else if (drv->r.planes == 15)
	{
		drv->r.format		= PF_PACKED;
		drv->r.pixelformat	= pf_15bI;
		drv->r.clut		= 0;
		drv->r.pixlen		= 2;
	}
	else if (drv->r.planes == 16)
	{
		drv->r.format		= PF_PACKED;
		drv->r.pixelformat	= pf_16bI;
		drv->r.clut		= 0;
		drv->r.pixlen		= 2;
	}
	else if (drv->r.planes == 24)
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
	}
	else if (drv->r.planes == 32)
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
	}
	else
	{
		log(" planes = %d\n", drv->r.planes);
		return 0;
	}

	return drv;
}

static long
ovdidev_close(OVDI_DRIVER *drv)
{
	return 0;
}

static unsigned char *
ovdidev_setpscreen(OVDI_DRIVER *drv, unsigned char *scradr)
{
	if (scradr < (unsigned char *)drv->vram_start)
		return xcb->scr_base;
	else if (scradr > (unsigned char *)((long)drv->vram_start + drv->vram_size))
		return xcb->scr_base;

	do_p_setscr((long)xcb->p_setscr, scradr);
	return	xcb->scr_base;
}

static unsigned char *
ovdidev_setlscreen(OVDI_DRIVER *drv, unsigned char *logscr)
{
	*(long *)v_bas_ad = (long)logscr;
	return logscr;
}

static void
ovdidev_setcolor(OVDI_DRIVER *drv, short pen, RGB_LIST *colors)
{
	unsigned char bcols[4];

	if (drv->r.planes != 8)
		return;

	bcols[0] = (unsigned char)colors->red;
	bcols[1] = (unsigned char)colors->green;
	bcols[2] = (unsigned char)colors->blue;
	bcols[3] = 0;

	do_p_setcol((long)xcb->p_setcol, pen, (unsigned char *)&bcols[0]);
	return;
}

static void
ovdidev_vsync(OVDI_DRIVER *drv)
{
	do_p_vsync((long)xcb->p_vsync);
	return;
}

/* This will be called by the mouse-drivers (layer 1) mouse-interrupt whenever
* the mouse moves.
*/
static void
ovdidev_vreschk(short x, short y)
{
	do_p_chng_vrt((long)xcb->p_chng_vrt, x, y);
	return;
}
