#include <mint/mintbind.h>
#include <fcntl.h>

#include "display.h"
#include "libkern.h"
#include "vdi_defs.h"
#include "ovdi_dev.h"
#include "xcb.h"
#include "std_driver.h"

int	Load_Resolution(short index, RESOLUTION *res);
short	boot_drive;
int get_cookie(long tag, long *ret);

OVDI_LIB	*lib;

XCB	*xcb;

char ovdidev_name[20] = {"oVDI Device Driver"};

OVDI_DEVICE ovdidev =
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

OVDI_DRIVER driver;

/* pf_xxx tables are used by vq_extend() to describe different pixel formats */

/* ST palette register layout */
char pf_st[] =
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
char pf_ste[] =
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
char pf_tt[] =
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
char pf_falcon[] =
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


char pf_et6k[] =
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

char pf_nova[] =
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
char pf_15b_falc[] =
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
char pf_15bI[] =
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
char pf_16bI[] =
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
char pf_16b_et6k[] =
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
char pf_24bI[] =
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
char pf_24bM[] =
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
char pf_32bI[] = 
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
char pf_32bM[] = 
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
char pf_32bIbs[] =
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

int
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
		return 0;

	r = Fread( fd, sizeof(RESOLUTION), res);

	if (r == sizeof(RESOLUTION))
		return 1;
	else
		return 0;
}

OVDI_DEVICE *
device_init(OVDI_LIB *l)
{

	if (!(*l->getcookie)((long)0x4e4f5641 /*"NOVA"*/, (long *)&xcb))
	{
		Cconws(" oVDIDEV_INIT: Could not locate NOVA cookie\n");
		return 0;
	}

	boot_drive = Dgetdrv() + 'a';

	lib	= l;

	return &ovdidev;
};

OVDI_DRIVER *
ovdidev_open(OVDI_DEVICE *dev, short dev_id)
{
	XCB *x = xcb;
	OVDIDRV_FUNCTAB *f;
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

	bzero(&drv->f, sizeof(OVDIDRV_FUNCTAB));

	f = (OVDIDRV_FUNCTAB *)&drv->f;

	if (drv->r.planes == 1)
	{
		drv->r.format		= PF_ATARI;
		drv->r.clut		= 1;

		f->pixelfuncts		= 0;

	//	*f = *(OVDIDRV_FUNCTAB *)&functions_mono;
	}
	else if (drv->r.planes == 4)
	{
		drv->r.format		= PF_ATARI;
		drv->r.clut		= 1;

		f->pixelfuncts		= (draw_pixel *)&dpf_4b;

	}
	else if (drv->r.planes == 8)
	{
		drv->r.format		= PF_PACKED;

		drv->r.pixelformat	= pf_nova;

		drv->r.clut		= 1;

		f->pixelfuncts		= (draw_pixel *)&dpf_8b;
		f->rt_functs		= (draw_pixel *)&rt_ops_8b;

	//	*f = *(OVDIDRV_FUNCTAB *)&functions_8b;
	}
	else if (drv->r.planes == 15)
	{
		drv->r.format		= PF_PACKED;

		drv->r.pixelformat	= pf_15bI;

		drv->r.clut		= 0;

		f->pixelfuncts		= (draw_pixel *)&dpf_16b;

	//	*f = *(OVDIDRV_FUNCTAB *)&functions_15bI;
	}
	else if (drv->r.planes == 16)
	{
		drv->r.format		= PF_PACKED;

		drv->r.pixelformat	= pf_16bI;

		drv->r.clut		= 0;

		f->pixelfuncts		= (draw_pixel *)&dpf_16b;

	//	*f = *(OVDIDRV_FUNCTAB *)&functions_16bI;
	}
	else if (drv->r.planes == 24)
	{
		drv->r.clut	= 0;

		if (!fmt & 1)
		{
			drv->r.format		= PF_PACKED | PF_BE;
			drv->r.pixelformat	= pf_24bM;

			f->pixelfuncts		= 0;

		//	*f = *(OVDIDRV_FUNCTAB *)&functions_24bM;
		}
		else
		{
			drv->r.format		= PF_PACKED;
			drv->r.pixelformat	= pf_24bI;

			f->pixelfuncts		= 0;

		//	*f = *(OVDIDRV_FUNCTAB *)&functions_24bI;
		}
	}
	else if (drv->r.planes == 32)
	{
		short format = ((fmt >> 1) & 3);

		drv->r.clut		= 0;

		if (!format)
		{
			drv->r.format		= PF_PACKED;
			drv->r.pixelformat	= pf_32bI;

			f->pixelfuncts		= 0;

		//	*f = *(OVDIDRV_FUNCTAB *)&functions_32bI;
		}
		else if (fmt == 1)
		{
			drv->r.format		= PF_PACKED | PF_BE;
			drv->r.pixelformat	= pf_32bM;

			f->pixelfuncts		= 0;

		//	*f = *(OVDIDRV_FUNCTAB *)&functions_32bM;
		}
		else
		{
			drv->r.format		= PF_PACKED | PF_BS;
			drv->r.pixelformat	= pf_32bIbs;

			f->pixelfuncts		= 0;

		//	*f = *(OVDIDRV_FUNCTAB *)&functions_32bIbs;
		}
	}
	else
	{
		log(" planes = %d\n", drv->r.planes);
		return 0;
	}

	return drv;
}

long
ovdidev_close(OVDI_DEVICE *drv)
{
	return 0;
}

unsigned char *
ovdidev_setpscreen(OVDI_DRIVER *drv, unsigned char *scradr)
{
	if (scradr < (unsigned char *)drv->vram_start)
		return xcb->scr_base;
	else if (scradr > (unsigned char *)((long)drv->vram_start + drv->vram_size))
		return xcb->scr_base;

	do_p_setscr((long)xcb->p_setscr, scradr);
	return	xcb->scr_base;
}

unsigned char *
ovdidev_setlscreen(OVDI_DRIVER *drv, unsigned char *logscr)
{
	*(long *)v_bas_ad = (long)logscr;
	return logscr;
}

void
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

void
ovdidev_vsync(OVDI_DRIVER *drv)
{
	do_p_vsync((long)xcb->p_vsync);
	return;
}

/* This will be called by the mouse-drivers (layer 1) mouse-interrupt whenever
* the mouse moves.
*/
void
ovdidev_vreschk(short x, short y)
{
	do_p_chng_vrt((long)xcb->p_chng_vrt, x, y);
	return;
}
