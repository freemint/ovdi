#include <mint/mintbind.h>
#include <fcntl.h>

#include "modinf.h"
#include "ovdi_lib.h"
#include "ovdi_defs.h"

#include "atarivideo.h"
#include "includes/pf_tt.h"

void init (OVDI_LIB *, struct module_desc *ret, char *p, char *f);

static OVDI_DRIVER *	dev_open		(OVDI_DEVICE *dev);
static long		dev_close		(OVDI_DRIVER *drv);
static short		dev_set_vdi_res		(OVDI_DRIVER *drv, short scrndev_id);
static short		dev_get_res_info	(OVDI_DRIVER *drv);
static unsigned char *	dev_setpscreen		(OVDI_DRIVER *drv, unsigned char *scrnadr);
static unsigned char *	dev_setlscreen		(OVDI_DRIVER *drv, unsigned char *scrnadr);
static void		dev_setcolor		(OVDI_DRIVER *drv, short pen, RGB_LIST *colors);
static void		dev_vsync		(OVDI_DRIVER *drv);
static void		dev_vreschk		(short x, short y);

static void	do_set_res(OVDI_DRIVER *drv, short res_id);

static short	boot_drive;
static OVDI_LIB	*l;

static char sname[] =	"Atari TT";
static char lname[] =	"Atari TT graphics driver for oVDI";
static char pn[128] = { "0" };
static char fn[64] = { "0" };

static OVDI_DEVICE device =
{
	0,
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
	0		/* sync */
};

static RESFMT resfmt_1b =
{
	1,
	1,
	PF_ATARI,
	-1,
	pf_tt,
};
static RESFMT resfmt_2b =
{
	2,
	1,
	PF_ATARI,
	-2,
	pf_tt,
};
static RESFMT resfmt_4b =
{
	4,
	1,
	PF_ATARI,
	-4,
	pf_tt,
};
static RESFMT resfmt_8b =
{
	8,
	1,
	PF_ATARI,
	-8,
	pf_tt,
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

static long
get_video_address(void)
{
	long ret, real;

	real = v_bas_ad;

	//(*l->scrnlog)("get_video_address: ");
	ret = (long)((long)VB_HI << 16) | ((long)VB_MI << 8) | VB_LO;
	//(*l->scrnlog)("returning %lx, %lx\n", ret, real);
	//real = (long)(VB_LO | ((long)VB_MI << 8) | ((long)VB_HI << 16));
	//(*l->scrnlog)("diffread %lx\n", real);
	return ret;
}
static void
do_set_res(OVDI_DRIVER *drv, short res_id)
{
	short mode, res, modemask, ww;

	res = -1;
	modemask = VMODE_TT_MASK;

	switch (res_id)
	{
		case 1:		/* Default */
		{
			break;
		}
		case 2:		/* 320x200 16 col (4bpp) ( ST Low )	*/
		{
			res	= VMODE_LOW & modemask;
			ww	= ((320 >> 4) << 2) - 1;
			break;
		}
		case 3:		/* 640x200 4 col (2bpp) ( ST medium )	*/
		{
			res	= VMODE_MED & modemask;
			ww	= ((640 >> 4) << 1) - 1;
			break;
		}
		case 4:		/* 640x400 mono ( ST High )		*/
		{
			res	= VMODE_HI & modemask;
			ww	= (640 >> 4) - 1;
			break;
		}
		case 5:		/* Not defined */
		{
			break;
		}
		case 6:		/* 640x480 16 col (4bpp) ( TT medium ) */
		{
			res	= VMODE_TT_MED & modemask;
			ww	= ((640 >> 4) << 2) - 1;
			break;
		}
		case 7:		/* Not defined */
		{
			break;
		}
		case 8:		/* 1280x960 mono (TT High) */
		{
			res	= VMODE_TT_HI & modemask;
			ww	= (1280 >> 4) - 1;
			break;
		}
		case 9:		/* 320x480, 256 col (8bpp) ( TT low ) */
		{
			res	= VMODE_TT_LOW & modemask;
			ww	= ((320 << 3) >> 4) - 1;
			break;
		}
	}
	if (res != -1)
	{
		LWIDTH = (unsigned char)ww;
		mode = (TT_VMODE & (~(VMODE_TT_MASK << 8))) | (res << 8);
		TT_VMODE = mode;
	}
}

void
init(OVDI_LIB *lib, struct module_desc *ret, char *path, char *file)
{
	OVDI_DRIVER *drv = &driver;
	OVDI_DEVICE *dev = &device;

	boot_drive = Dgetdrv() + 'a';

	l	= lib;

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
	(*l->bzero)(&driver, sizeof(OVDI_DRIVER));

	(*l->bzero)(&drw_1b, sizeof(OVDI_DRAWERS));
	drv->drawers_1b = &drw_1b;
	drv->drawers_1b->res = &resfmt_1b;

	(*l->bzero)(&drw_2b, sizeof(OVDI_DRAWERS));
	drv->drawers_2b = &drw_2b;
	drv->drawers_2b->res = &resfmt_2b;

	(*l->bzero)(&drw_4b, sizeof(OVDI_DRAWERS));
	drv->drawers_4b = &drw_4b;
	drv->drawers_4b->res = &resfmt_4b;

	(*l->bzero)(&drw_8b, sizeof(OVDI_DRAWERS));
	drv->drawers_8b = &drw_8b;
	drv->drawers_8b->res = &resfmt_8b;

	(*l->bzero)(&drw_15b, sizeof(OVDI_DRAWERS));
	drv->drawers_15b = &drw_15b;

	(*l->bzero)(&drw_16b, sizeof(OVDI_DRAWERS));
	drv->drawers_16b = &drw_16b;

	(*l->bzero)(&drw_24b, sizeof(OVDI_DRAWERS));
	drv->drawers_24b = &drw_24b;

	(*l->bzero)(&drw_32b, sizeof(OVDI_DRAWERS));
	drv->drawers_32b = &drw_32b;

	ret->types	= D_VHW;
	ret->vhw	= dev; //(void *)&ovdidev;
};

static OVDI_DRIVER *
dev_open(OVDI_DEVICE *dev)
{
	OVDI_DRIVER *drv = &driver;
	long usp;

	usp = Super(1);
	if (!usp)
		usp = Super(0);
	else
		usp = 0;

	drv->dev	= dev;
	do_set_res(drv, 4);
	(void)dev_get_res_info(drv);

	if (usp)
		Super(usp);

	return drv;
}

static long
dev_close(OVDI_DRIVER *drv)
{
	long usp;

	usp = Super(1);
	if (!usp)
		usp = Super(0);
	else
		usp = 0;

	do_set_res(drv, 4);
	(void)dev_get_res_info(drv);

	if (usp)
		Super(usp);

	return (long)drv;
}

static short
dev_set_vdi_res(OVDI_DRIVER *drv, short res_id)
{
	if (res_id <= 0)
	{
		dev_get_res_info(drv);
		return 0;
	}

	do_set_res(drv, res_id);

	(void)dev_get_res_info(drv);

	return res_id;
}

static short
get_video_mode(void)
{
	short mode;

	(unsigned short)mode = (TT_VMODE  >> 8) & VMODE_TT_MASK;
	return mode;
}

static short
dev_get_res_info(OVDI_DRIVER *drv)
{
	short mode;

	drv->r.sync		= 0; 	/* sync */
	drv->r.x1 = drv->r.y1	= 0;

	mode = get_video_mode();
	//(*l->scrnlog)("got mode %d\n", mode);
	switch (mode)
	{
		case VMODE_LOW:		/* 320x200 4bpp */
		{
			//drv->r.planes	= 4;
			drv->r.bypl	= (320 >> 4) << 3;
			drv->palette	= 4096;
			drv->r.w	= 320;
			drv->r.h	= 200;
			drv->r.x2	= drv->r.w - 1;
			drv->r.y2	= drv->r.h - 1;
			drv->v_top	= 0;
			drv->v_bottom	= 0;
			drv->v_left	= 0;
			drv->v_right	= 0;
			drv->vram_start	= (void *)get_video_address();
			drv->vram_size	= (long)drv->r.h * drv->r.bypl;
			drv->scr_size	= drv->vram_size;

			drv->r.res	= *drv->drawers_1b->res;
			//drv->r.format		= PF_ATARI;
			//drv->r.pixelformat	= pf_tt;
			//drv->r.clut		= 1;
			//drv->r.pixlen		= -4;
			break;
		}
		case VMODE_MED:		/* 640x200 2bpp */
		{
			//drv->r.planes	= 2;
			drv->r.bypl	= 80;
			drv->palette	= 4096;
			drv->r.w	= 640;
			drv->r.h	= 200;
			drv->r.x2	= drv->r.w - 1;
			drv->r.y2	= drv->r.h - 1;
			drv->v_top	= 0;
			drv->v_bottom	= 0;
			drv->v_left	= 0;
			drv->v_right	= 0;
			drv->vram_start	= (void *)get_video_address();
			drv->vram_size	= (long)drv->r.h * drv->r.bypl;
			drv->scr_size	= drv->vram_size;

			drv->r.res	= *drv->drawers_2b->res;
			//drv->r.format		= PF_ATARI;
			//drv->r.pixelformat	= pf_tt;
			//drv->r.clut		= 1;
			//drv->r.pixlen		= -2;
			break;
		}
		case VMODE_HI: 		/* 640x400 mono */
		{
			drv->r.bypl	= 80;
			drv->palette	= 2;
			drv->r.w	= 640;
			drv->r.h	= 400;
			drv->r.x2	= drv->r.w - 1;
			drv->r.y2	= drv->r.h - 1;
			drv->v_top	= 0;
			drv->v_bottom	= 0;
			drv->v_left	= 0;
			drv->v_right	= 0;
			drv->vram_start	= (void *)get_video_address();
			drv->vram_size	= (long)drv->r.h * drv->r.bypl;
			drv->scr_size	= drv->vram_size;

			drv->r.res	= *drv->drawers_1b->res;
			break;
		}
		case VMODE_TT_LOW:	/* 320x480 8bpp */
		{
			drv->r.bypl	= 320;
			drv->palette	= 4096;
			drv->r.w	= 320;
			drv->r.h	= 480;
			drv->r.x2	= drv->r.w - 1;
			drv->r.y2	= drv->r.h - 1;
			drv->v_top	= 0;
			drv->v_bottom	= 0;
			drv->v_left	= 0;
			drv->v_right	= 0;
			drv->vram_start	= (void *)get_video_address();
			drv->vram_size	= (long)drv->r.h * drv->r.bypl;
			drv->scr_size	= drv->vram_size;

			drv->r.res	= *drv->drawers_8b->res;
			break;
		}
		case VMODE_TT_MED:	/* 640x480 4bpp */
		{
			drv->r.bypl	= 320;
			drv->palette	= 4096;
			drv->r.w	= 640;
			drv->r.h	= 480;
			drv->r.x2	= drv->r.w - 1;
			drv->r.y2	= drv->r.h - 1;
			drv->v_top	= 0;
			drv->v_bottom	= 0;
			drv->v_left	= 0;
			drv->v_right	= 0;
			drv->vram_start	= (void *)get_video_address();
			drv->vram_size	= (long)drv->r.h * drv->r.bypl;
			drv->scr_size	= drv->vram_size;

			drv->r.res	= *drv->drawers_4b->res;
			break;
		}
		case VMODE_TT_HI:	/* 1280x960 mono */
		{
			drv->r.bypl	= 160;
			drv->palette	= 2;
			drv->r.w	= 1280;
			drv->r.h	= 960;
			drv->r.x2	= drv->r.w - 1;
			drv->r.y2	= drv->r.h - 1;
			drv->v_top	= 0;
			drv->v_bottom	= 0;
			drv->v_left	= 0;
			drv->v_right	= 0;
			drv->vram_start	= (void *)get_video_address();
			drv->vram_size	= drv->r.bypl * drv->r.h;
			drv->scr_size	= drv->vram_size;

			drv->r.res	= *drv->drawers_1b->res;
			break;
		}
	}

	drv->r.lenght	= drv->scr_size;
	drv->r.base	= drv->vram_start;
	drv->r.flags = drv->r.realflags = R_IS_SCREEN | R_IN_VRAM;

	//(*l->scrnlog)("got scrnadr %lx, r.len %ld, planes %d\n", drv->vram_start, drv->r.lenght, drv->r.planes);

#if 0
	drv->r.sync		= 0; 	/* sync */
	drv->r.planes		= x->planes;
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

	if (drv->r.planes == 1)
	{
		drv->r.format		= PF_ATARI;
		drv->r.pixelformat	= pf_tt;
		drv->r.clut		= 1;
		drv->r.pixlen		= -1;
	}
	else if (drv->r.planes == 4)
	{
		drv->r.format		= PF_ATARI;
		drv->r.pixelformat	= pf_nova;
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
		return -1;
	}
#endif
	//(void)Cnecin();
	return 0;
}

static unsigned char *
dev_setpscreen(OVDI_DRIVER *drv, unsigned char *scradr)
{
	if ((long)scradr > 0 && (long)scradr < (0x00e00000L - 156000) )
	{
		VB_HI = (char)((long)scradr >> 16);
		VB_MI = (char)((long)scradr >> 8);
		VB_LO = (char)((long)scradr);
#if 0
		if (hw & GOT_VB_LO)
			*(volatile unsigned char *)VB_LO = (char)scradr;
		else
			(long)scradr &= 0xffff00L;
#endif
	}
	return	scradr;
}

static unsigned char *
dev_setlscreen(OVDI_DRIVER *drv, unsigned char *logscr)
{
	v_bas_ad = (long)logscr;
	return logscr;
}

static void
dev_setcolor(OVDI_DRIVER *drv, short pen, RGB_LIST *colors)
{
	volatile unsigned short *palreg = (short *)(TT_PALETTE0_REG + (pen << 1));
	unsigned short col;
	long usp;

	//(*l->scrnlog)("Set col..");


	usp = Super(1);
	if (!usp)
		usp = Super(0);
	else
		usp = 0;

	pen <<= 1;
	col = colors->red << 8;
	col |= colors->green << 4;
	col |= colors->blue;
	*palreg = col;

	if (usp)
		Super(usp);

	//(*l->scrnlog)("Oki\n");
}

static void
dev_vsync(OVDI_DRIVER *drv)
{
	return;
}

/* This will be called by the mouse-drivers (layer 1) mouse-interrupt whenever
* the mouse moves.
*/
static void
dev_vreschk(short x, short y)
{
	return;
}

