#include "colors.h"
#include "console.h"
#include "display.h"
#include "draw.h"
#include "fonts.h"
#include "gdf_defs.h"
//#include "kbddrv.h"
#include "libkern.h"
#include "line.h"
#include "linea_vars.h"
#include "memory.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "ovdi_rasters.h"
#include "polygon.h"
#include "rasters.h"
#include "std_driver.h"
//#include "vbi.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "v_attribs.h"
#include "v_fill.h"
#include "v_input.h"
#include "v_line.h"
#include "v_pmarker.h"
#include "v_text.h"
#include "workstation.h"
#include "xbios.h"

extern const short systempalette[];

static short setup_drawers_jumptable(OVDI_DRAWERS *src, OVDI_DRAWERS *dst, short planes);

static OVDI_DRAWERS defdrawers = 
{
	{ /* struct vdiprimitives */
		rectfill,
		draw_arc,
		draw_pieslice,
		draw_circle,
		draw_ellipse,
		draw_ellipsearc,
		draw_ellipsepie,
		draw_rbox,

		abline,
		habline,
		vabline,
		pline,
		wide_line,

		draw_spans,
		draw_mspans,

		filled_poly,
		pmarker,

		rt_cpyfm,
		ro_cpyfm,
	},

	0,	/* draw_pixel */
	0,	/* read_pixel */
	0,	/* put_pixel */
	0,	/* get_pixel */
	0,	/* fill_16x */
	0,	/* spans_16x */
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },	/* drp - draw raster points */
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },	/* dlp - draw line points */
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },	/* pixel_blits */
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },	/* raster_blits */

	0,	/* draw_mcurs */
	0,	/* undraw_mcurs */
};

static OVDI_UTILS defutils =
{
	clc_nsteps,
	clc_arc,
	clipbox,
	sortcpy_corners,
	code,
	clip_line,
	fix_raster_coords,
	trnfm,

	conv_vdi2dev_1b,
	conv_vdi2dev_2b,
	conv_vdi2dev_4b,
	conv_vdi2dev_8b,
	conv_vdi2dev_16b,
	conv_vdi2dev_16b,
	conv_vdi2dev_24b,
	conv_vdi2dev_32b,
	{0,0,0,0},

	conv_dev2vdi_1b,
	conv_dev2vdi_2b,
	conv_dev2vdi_4b,
	conv_dev2vdi_8b,
	conv_dev2vdi_16b,
	conv_dev2vdi_16b,
	conv_dev2vdi_24b,
	conv_dev2vdi_32b,
	{0,0,0,0}
};

static OVDI_DRAWERS *other_drawers[] =
{
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#if 0
static OVDI_UTILS	root_utils;
static OVDI_DRAWERS	root_drawers;
static OVDI_DRAWERS	drawers_1b;
static OVDI_DRAWERS	drawers_2b;
static OVDI_DRAWERS	drawers_4b;
static OVDI_DRAWERS	drawers_8b;
static OVDI_DRAWERS	drawers_15b;
static OVDI_DRAWERS	drawers_16b;
static OVDI_DRAWERS	drawers_24b;
static OVDI_DRAWERS	drawers_32b;
#endif

/*
 * initialize the function tables associated with a raster,
 * like the 'ovdi_drawers' structures and 'ovdi_utils' structure.
*/
void
init_raster(OVDI_DRIVER *drv, RASTER *r)
{
	char *mem;

	if (!r->odrawers)
	{
		mem = (char *)omalloc( sizeof(other_drawers) +
					(sizeof(OVDI_DRAWERS) * 8) +
					sizeof(OVDI_UTILS),
					MX_PREFTTRAM | MX_PRIVATE );

		if (mem)
		{
			(long)r->odrawers = (long)mem;
			bzero(mem, sizeof(other_drawers));
			mem	+= sizeof(other_drawers);

			r->odrawers[1]	= (OVDI_DRAWERS *)mem;
			mem	+= sizeof(OVDI_DRAWERS);

			r->odrawers[2] = (OVDI_DRAWERS *)mem;
			mem	+= sizeof(OVDI_DRAWERS);

			r->odrawers[4] = (OVDI_DRAWERS *)mem;
			mem	+= sizeof(OVDI_DRAWERS);

			r->odrawers[8] = (OVDI_DRAWERS *)mem;
			mem	+= sizeof(OVDI_DRAWERS);

			r->odrawers[15] = (OVDI_DRAWERS *)mem;
			mem	+= sizeof(OVDI_DRAWERS);

			r->odrawers[16] = (OVDI_DRAWERS *)mem;
			mem	+= sizeof(OVDI_DRAWERS);

			r->odrawers[24] = (OVDI_DRAWERS *)mem;
			mem	+= sizeof(OVDI_DRAWERS);

			r->odrawers[32] = (OVDI_DRAWERS *)mem;
			mem	+= sizeof(OVDI_DRAWERS);

			r->utils = (OVDI_UTILS *)mem;

			*r->utils = defutils;

			setup_drawers_jumptable(drv->drawers_1b, r->odrawers[1], 1);
			setup_drawers_jumptable(drv->drawers_1b, r->odrawers[4], 4);
			setup_drawers_jumptable(drv->drawers_8b, r->odrawers[8], 8);
			setup_drawers_jumptable(drv->drawers_15b, r->odrawers[15], 15);
			setup_drawers_jumptable(drv->drawers_16b, r->odrawers[16], 16);
		}
	}

#if 0
	r->odrawers = other_drawers;

	setup_drawers_jumptable(drv->drawers_1b, &drawers_1b, 1);
	r->odrawers[1] = &drawers_1b;
	setup_drawers_jumptable(drv->drawers_8b, &drawers_8b, 8);
	r->odrawers[8] = &drawers_8b;
	setup_drawers_jumptable(drv->drawers_15b, &drawers_15b, 15);
	r->odrawers[15] = &drawers_15b;
	setup_drawers_jumptable(drv->drawers_16b, &drawers_16b, 16);
	r->odrawers[16] = &drawers_16b;

	root_utils	= defutils;
	r->utils	= &root_utils;
#endif

}

void
raster_reschange(RASTER *r, COLINF *c)
{

	get_rgb_levels( r->pixelformat, &r->rgb_levels);
	get_rgb_bits( r->pixelformat, &r->rgb_bits);

	if (c)
		init_colinf(r, c);

	r->drawers	= r->odrawers[r->planes];

	if (scrsizmm_x)
		r->wpixel = (scrsizmm_x * 1000) / r->w;
	else
		r->wpixel = 278;

	if (scrsizmm_y)
		r->hpixel = (scrsizmm_y * 1000) / r->h;
	else
		r->hpixel = 278;

}

void
reschange_devtab(DEV_TAB *dt, RASTER *r)
{
	unsigned long palettesize;

	dt->xres	= r->w - 1;
	dt->yres	= r->h - 1;
	dt->wpixel	= r->wpixel;
	dt->hpixel	= r->hpixel;

	if (r->planes == 1)
		dt->cancolor = 0;
	else
		dt->cancolor = 1;

	if (r->planes < 8)
		dt->colors = 1 << r->planes;
	else
		dt->colors = 256;

	palettesize = (long)r->rgb_levels.red * (long)r->rgb_levels.green * (long)r->rgb_levels.blue;

	if (palettesize > 32767UL)
		dt->palette = 0;
	else
		dt->palette = (unsigned short)palettesize;
}

void
reschange_inqtab(INQ_TAB *it, RASTER *r)
{
	it->planes	= r->planes;
	it->lut		= r->clut;
}

COLINF *
new_colinf(RASTER *r)
{
	COLINF *c;
	long len, mem;

	c = 0;

	len	=  sizeof(COLINF);
	len	+= 256 << 2;				/* VDI2HW and HW2VDI arrays */
	len	+= 256 << 2;				/* Pixelvalues array (256 longs) */
	len	+= (sizeof(RGB_LIST) << 1) * 256;	/* request, actual 0-256*/

	mem = (long)omalloc(len, MX_PREFTTRAM|MX_SUPER);

	if (mem)
	{
		c = (COLINF *)mem;
		mem += sizeof(COLINF);

		c->color_vdi2hw = (short *)mem;
		mem += 256 << 1;

		c->color_hw2vdi = (short *)mem;
		mem += 256 << 1;

		c->pixelvalues = (long *)mem;
		mem += 256 << 2;

		c->request_rgb = (RGB_LIST *)mem;
		mem += (long)sizeof(RGB_LIST) * 256;

		c->actual_rgb = (RGB_LIST *)mem;
		mem += (long)sizeof(RGB_LIST) * 256;

		c->pixelformat	= r->pixelformat;
	}
	return c;	
}

void
init_colinf(RASTER *r, COLINF *c)
{
	short *syspal;
	short i, pens;
	RGB_LIST temp;

	for (i = 0; i < 256; i++)
	{
		c->color_vdi2hw[i] = VDI2HW_colorindex[i];
		c->color_hw2vdi[i] = HW2VDI_colorindex[i];
	}
	if (r->planes < 8)
	{
		pens = 1 << r->planes;
		c->color_vdi2hw[1] = pens - 1;
		c->color_hw2vdi[pens - 1] = 1;
	}
	else
	{
		c->color_vdi2hw[1] = 255;
		c->color_hw2vdi[15] = 255;
		pens = 256;
	}
	c->pens		= pens;
	c->planes	= r->planes;
	syspal	= (short *)&systempalette;
	temp.alpha = temp.ovl = 0;
	for (i = 0; i < pens; i++)
	{
		temp.red	= *syspal++;
		temp.green	= *syspal++;
		temp.blue	= *syspal++;
		(void)calc_vdicolor( r, c, i, &temp);
	}
}

void
clone_colinf(COLINF *dst, COLINF *src)
{
	short i;

	for (i = 0; i < 256; i++)
	{
		dst->color_vdi2hw[i]	= src->color_vdi2hw[i];
		dst->color_hw2vdi[i]	= src->color_hw2vdi[i];
		dst->request_rgb[i]	= src->request_rgb[i];
		dst->actual_rgb[i]	= src->actual_rgb[i];
		dst->pixelvalues[i]	= src->pixelvalues[i];
	}
	dst->pens	= src->pens;
	dst->planes	= src->planes;
}

/*
 * Setup ovdi_drawers structure.
*/	
static short
setup_drawers_jumptable(OVDI_DRAWERS *src, OVDI_DRAWERS *dst, short planes)
{
	short i;
	long *srcp, *dstp, *defp;

	*dst = defdrawers;

	/*
	 * Set up the VDI primitive function pointers.
	 * If graphics driver provide a pointer, we take that, else we
	 * take the pointer from the default table.
	*/
	srcp = (long *)&src->p;
	dstp = (long *)&dst->p;
	defp = (long *)&defdrawers.p;
	for (i = 0; i < sizeof(VDIPRIMITIVES) >> 2; i++)
	{
		if (*srcp)
			*dstp++ = *srcp++, defp++;
		else
			*dstp++ = *defp++, srcp++;
	}
				
	switch (planes)
	{
		case 1:
		{
			for (i = 0; i < 16; i++)
			{
				if (src->pixel_blits[i])
					dst->pixel_blits[i] = src->pixel_blits[i];
				else
					dst->pixel_blits[i] = rt_ops_1b[i];

				if (src->raster_blits[i])
					dst->raster_blits[i] = src->raster_blits[i];
				else
					dst->raster_blits[i] = rops_1b[i];
			}

			if (src->draw_mcurs)
				dst->draw_mcurs = src->draw_mcurs;
			else
				dst->draw_mcurs = draw_mousecurs_1b;

			if (src->undraw_mcurs)
				dst->undraw_mcurs = src->undraw_mcurs;
			else
				dst->undraw_mcurs = restore_msave_1b;

			if (src->put_pixel)
				dst->put_pixel = src->put_pixel;
			else
				dst->put_pixel = put_pixel_1b;

			if (src->get_pixel)
				dst->get_pixel = src->get_pixel;
			else
				dst->get_pixel = get_pixel_1b;

			if (src->fill_16x)
				dst->fill_16x = src->fill_16x;
			else
				dst->fill_16x = fill_16x_1b;
			break;
		}
		case 4:
		{
			for (i = 0; i < 16; i++)
			{
				if (src->pixel_blits[i])
					dst->pixel_blits[i] = src->pixel_blits[i];
				else
					dst->pixel_blits[i] = rt_ops_4b[i];

				if (src->raster_blits[i])
					dst->raster_blits[i] = src->raster_blits[i];
				else
					dst->raster_blits[i] = rops_4b[i];
			}

			if (src->draw_mcurs)
				dst->draw_mcurs = src->draw_mcurs;
			else
				dst->draw_mcurs = draw_mousecurs_4b;

			if (src->undraw_mcurs)
				dst->undraw_mcurs = src->undraw_mcurs;
			else
				dst->undraw_mcurs = restore_msave_4b;

			if (src->put_pixel)
				dst->put_pixel = src->put_pixel;
			else
				dst->put_pixel = put_pixel_4b;

			if (src->get_pixel)
				dst->get_pixel = src->get_pixel;
			else
				dst->get_pixel = get_pixel_4b;

			if (src->fill_16x)
				dst->fill_16x = src->fill_16x;
			else
				dst->fill_16x = fill_16x_4b;

			break;
		}
		case 8:
		{
			for (i = 0; i < 16; i++)
			{
				if (src->pixel_blits[i])
					dst->pixel_blits[i] = src->pixel_blits[i];
				else
					dst->pixel_blits[i] = rt_ops_8b[i];

				if (src->raster_blits[i])
					dst->raster_blits[i] = src->raster_blits[i];
				else
					dst->raster_blits[i] = rops_8b[i];
			}

			if (src->draw_mcurs)
				dst->draw_mcurs = src->draw_mcurs;
			else
				dst->draw_mcurs = draw_mousecurs_8b;

			if (src->undraw_mcurs)
				dst->undraw_mcurs = src->undraw_mcurs;
			else
				dst->undraw_mcurs = restore_msave_8b;

			if (src->put_pixel)
				dst->put_pixel = src->put_pixel;
			else
				dst->put_pixel = put_pixel_8b;

			if (src->get_pixel)
				dst->get_pixel = src->get_pixel;
			else
				dst->get_pixel = get_pixel_8b;

			if (src->fill_16x)
				dst->fill_16x = src->fill_16x;
			else
				dst->fill_16x = fill_16x_8b;

#if 1
			if (src->spans_16x)
				dst->spans_16x = src->spans_16x;
			else
				dst->spans_16x = spans_16x_8b;
#endif
			break;
		}
		case 15:
		{
			for (i = 0; i < 16; i++)
			{
				if (src->pixel_blits[i])
					dst->pixel_blits[i] = src->pixel_blits[i];
				else
					dst->pixel_blits[i] = rt_ops_16b[i];

				if (src->raster_blits[i])
					dst->raster_blits[i] = src->raster_blits[i];
				else
					dst->raster_blits[i] = rops_16b[i];
			}

			if (src->draw_mcurs)
				dst->draw_mcurs = src->draw_mcurs;
			else
				dst->draw_mcurs = draw_mousecurs_16b;

			if (src->undraw_mcurs)
				dst->undraw_mcurs = src->undraw_mcurs;
			else
				dst->undraw_mcurs = restore_msave_16b;

			if (src->put_pixel)
				dst->put_pixel = src->put_pixel;
			else
				dst->put_pixel = put_pixel_16b;

			if (src->get_pixel)
				dst->get_pixel = src->get_pixel;
			else
				dst->get_pixel = get_pixel_16b;

			break;
		}
		case 16:
		{
			for (i = 0; i < 16; i++)
			{
				if (src->pixel_blits[i])
					dst->pixel_blits[i] = src->pixel_blits[i];
				else
					dst->pixel_blits[i] = rt_ops_16b[i];

				if (src->raster_blits[i])
					dst->raster_blits[i] = src->raster_blits[i];
				else
					dst->raster_blits[i] = rops_16b[i];
			}

			if (src->draw_mcurs)
				dst->draw_mcurs = src->draw_mcurs;
			else
				dst->draw_mcurs = draw_mousecurs_16b;

			if (src->undraw_mcurs)
				dst->undraw_mcurs = src->undraw_mcurs;
			else
				dst->undraw_mcurs = restore_msave_16b;

			if (src->put_pixel)
				dst->put_pixel = src->put_pixel;
			else
				dst->put_pixel = put_pixel_16b;

			if (src->get_pixel)
				dst->get_pixel = src->get_pixel;
			else
				dst->get_pixel = get_pixel_16b;

			break;
		}
#if 0
		case 24:
		{
			if (!drv->f.raster_operations)
				drv->f.raster_operation = &rops_24b;
			break;
		}
		case 32:
		{
			if (!drv->f.raster_operations)
				drv->f.raster_operation = &rops_32b;
			break;
		}
#endif
		default:
		{
			return 1;
		}


	}

 /* DRP - Draw Raster Point, used for vrt_cpyfm(). */
	/* MD_REPLACE */
	dst->drp[0] = dst->pixel_blits[3];
	dst->drp[1] = dst->pixel_blits[3];
	/* MD_TRANS */
	dst->drp[2] = dst->pixel_blits[3];
	dst->drp[3] = 0;
	/* MD_EOR */
	dst->drp[4] = dst->pixel_blits[6];
	dst->drp[5] = dst->pixel_blits[6];
	/* MD_ERASE */
	dst->drp[6] = 0;
	dst->drp[7] = dst->pixel_blits[3];

 /* DLP - used for Lines, patterns, etc... */
	/* MD_REPLACE */
	dst->dlp[0] = dst->pixel_blits[3];
	dst->dlp[1] = dst->pixel_blits[3];
	/* MD_TRANS */
	dst->dlp[2] = dst->pixel_blits[3];
	dst->dlp[3] = 0;
	/* MD_EOR */
	dst->dlp[4] = dst->pixel_blits[6];
	dst->dlp[5] = 0;
	/* MD_ERASE */
	dst->dlp[6] = dst->pixel_blits[3];
	dst->dlp[7] = 0;

	return 0;
}
