//#include <mint/sysvars.h>
#include <mintbind.h>
//#include <osbind.h>
#include <fcntl.h>

#include "colors.h"
#include "console.h"
#include "display.h"
#include "draw.h"
#include "fonts.h"
#include "gdf_defs.h"
#include "kbddrv.h"
#include "libkern.h"
#include "line.h"
#include "linea.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "polygon.h"
#include "rasters.h"
#include "std_driver.h"
#include "vbi.h"
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

static OVDI_DRAWERS defdrawers = 
{
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
	wide_line,
	draw_spans,
	filled_poly,
	rt_cpyfm,
	ro_cpyfm,

	0,	/* draw_pixel */
	0,	/* read_pixel */
	0,	/* put_pixel */
	0,	/* get_pixel */
	0,	/* draw_solid_rect */
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



static short setup_drawers_jumptable(OVDI_DRAWERS *src, OVDI_DRAWERS *dst, short planes);
static void update_devtab(DEV_TAB *, VIRTUAL *);
static void update_inqtab(INQ_TAB *, VIRTUAL *);
static void update_siztab(SIZ_TAB *, VIRTUAL *);
static void prepare_stdreturn( VDIPB *, VIRTUAL *);
static void prepare_extreturn( VDIPB *, VIRTUAL *);
static void prepare_scrninfreturn( VDIPB *, VIRTUAL *);
static void get_MiNT_info(VIRTUAL *);
static void setup_virtual(VDIPB *, VIRTUAL *, VIRTUAL *);
static void setup_fonts(VIRTUAL *, SIZ_TAB *, DEV_TAB *);

void
v_opnwk(VDIPB *pb, VIRTUAL *wk, VIRTUAL *lawk, struct ovdi_device *dev)
{
	short i, vdidev_id;
	OVDI_DRIVER *drv;
	register struct opnwk_input *wkin;

	wkin = (struct opnwk_input *)&pb->intin[0];

	vdidev_id = wkin->id;

	/* Cannot open anything but screen for now */
	if (vdidev_id < 1 || vdidev_id > 10)
	{
		log("Only support for screen drivers!\n");
		if (v_vtab[1].v)
			prepare_stdreturn(pb, wk);
		goto error;
	}
	
	MiNT = get_cookie((long)0x4d694e54/*"MiNT"*/, 0);

	if (v_vtab[1].v)
	{
		scrnlog("This should absolutely NOT happen, I think\n");
		log("This should absolutely NOT happen, I think\n");
		prepare_stdreturn(pb, wk);
		goto error;
	}
	
	drv = (*dev->open)(dev, vdidev_id);

	if (drv)
	{
		OVDI_DRAWERS *drawers;
		OVDI_UTILS *utils;
		long *rlpxl;
		short pens;
		struct rgb_list *dst_reqrgb, *actrgb;
		struct rgb_list tmp_rgb;
		short *src_reqrgb;
		RASTER *r;
		PatAttr *ptrn;


		r = (RASTER *)&drv->r;
		wk->driver = drv;
		wk->physical = drv;
		wk->raster = r;

	/* Check for necessary functions which the current graphics card driver */
	/* didnt have 								*/

		setup_drawers_jumptable(drv->drawers_1b, &drawers_1b, 1);
		wk->odrawers[1] = &drawers_1b;
		setup_drawers_jumptable(drv->drawers_8b, &drawers_8b, 8);
		wk->odrawers[8] = &drawers_8b;
		setup_drawers_jumptable(drv->drawers_15b, &drawers_15b, 15);
		wk->odrawers[15] = &drawers_15b;
		setup_drawers_jumptable(drv->drawers_16b, &drawers_16b, 16);
		wk->odrawers[16] = &drawers_16b;

		drawers		= wk->odrawers[r->planes];
		wk->drawers	= drawers;
		r->drawers	= drawers;

		utils		= &root_utils;
		*utils		= defutils;
		wk->utils	= utils;


		if (scrsizmm_x)
			r->wpixel = (scrsizmm_x * 1000) / r->w;
		else
			r->wpixel = 278;

		if (scrsizmm_y)
			r->hpixel = (scrsizmm_y * 1000) / r->h;
		else
			r->hpixel = 278;

	/* Check for available color indipendant driver services */
		if (!drv->add_vbifunc) /* Check if driver has VBI api, if not, use generic built in stuff */
		{
			init_vbi();
			drv->get_vbitics = get_vbitics;
			drv->add_vbifunc = add_vbi_function;
			drv->remove_vbifunc = remove_vbi_function;
			drv->reset_vbi = reset_vbi;
			drv->enable_vbi = enable_vbi;
			drv->disable_vbi = disable_vbi;
		}

	/* setup color tables */
		wk->color_vdi2hw = (short *)&VDI2HW_colorindex;
		wk->color_hw2vdi = (short *)&HW2VDI_colorindex;

		src_reqrgb = (short *)&systempalette;
		dst_reqrgb = (struct rgb_list *)&req_pens;
		actrgb = (struct rgb_list *)&act_pens;
		rlpxl = realpixels;
		r->pixelvalues = rlpxl;
		wk->request_rgb = dst_reqrgb;
		wk->actual_rgb = actrgb;

		wk->rgb_levels = &rgb_levels;
		get_rgb_levels( r->pixelformat, wk->rgb_levels);
		wk->rgb_bits = &rgb_bits;
		get_rgb_bits( r->pixelformat, wk->rgb_bits);

		if (r->planes < 8)
		{
			pens = Planes2Pens[r->planes];
			VDI2HW_colorindex[1] = pens - 1;
			HW2VDI_colorindex[pens - 1] = 1;
		}
		else
		{
			VDI2HW_colorindex[1] = 255;
			HW2VDI_colorindex[15] = 255;
			pens = 256;
		}

		tmp_rgb.alpha	= 0;
		tmp_rgb.ovl	= 0;
		for (i = 0; i < pens; i++)
		{
			tmp_rgb.red	= *src_reqrgb++;
			tmp_rgb.green	= *src_reqrgb++;
			tmp_rgb.blue	= *src_reqrgb++;
			lvs_color( wk, i, &tmp_rgb);
		}

	/* Set physical/logical screen addresses */
		r->base = (*dev->setpscr)(drv, r->base);
		drv->log_base = (*dev->setlscr)(drv, r->base);
		*(long *)v_bas_ad = (long)r->base; //scr_base;

	/* Setup some commonly used PatAttr's */
		/* WHITE rectangle */
		ptrn = &WhiteRect;
		ptrn->expanded = 0;
		ptrn->color[0] = ptrn->color[1] = VDI2HW_colorindex[0];
		ptrn->bgcol[0] = ptrn->bgcol[1] = VDI2HW_colorindex[1];
		if (r->planes > 8)
		{
			ptrn->color[2] = ptrn->color[3] = 0x0;
			ptrn->bgcol[2] = ptrn->bgcol[3] = 0xff;
		}
		else
		{
			ptrn->color[2] = ptrn->color[3] = 0xff;
			ptrn->bgcol[2] = ptrn->bgcol[3] = 0x0;
		}
		ptrn->width = 16;
		ptrn->height = 1;
		ptrn->wwidth = 1;
		ptrn->planes = 1;
		ptrn->wrmode = 0;
		ptrn->mask = 0xffff;
		ptrn->data = &SOLID;

		/* BLACK rectangle */
		ptrn = &BlackRect;
		ptrn->expanded = 0;
		ptrn->color[0] = ptrn->color[1] = VDI2HW_colorindex[1];
		ptrn->bgcol[0] = ptrn->bgcol[1] = VDI2HW_colorindex[0];
		if (r->planes > 8)
		{
			ptrn->color[2] = ptrn->color[3] = 0x00;
			ptrn->bgcol[2] = ptrn->bgcol[3] = 0xff;
		}
		else
		{
			ptrn->color[2] = ptrn->color[3] = 0xff;
			ptrn->bgcol[2] = ptrn->bgcol[3] = 0x0;
		}
		ptrn->width = 16;
		ptrn->height = 1;
		ptrn->wwidth = 1;
		ptrn->planes = 1;
		ptrn->wrmode = 0;
		ptrn->mask = 0xffff;
		ptrn->data = &SOLID;

	/* Init Mouse/Keyboard device driver */
		wk->mouseapi = init_mouse(wk, linea_vars);
		wk->kbdapi = init_keyboard(wk);

	/* install the time/interrupt specific thigns */
		wk->timeapi = init_time(wk, linea_vars);

		if (!wk->handle)
			setup_fonts(wk, &SIZ_TAB_rom, &DEV_TAB_rom);

		update_devtab(&DEV_TAB_rom, wk);
		update_inqtab(&INQ_TAB_rom, wk);
		update_siztab(&SIZ_TAB_rom, wk);

	/* Setup the rest of the virtual structure */
		setup_virtual(pb, wk, 0);

		if (!wk->handle)
		{
		/* Make a copy of VIRTUAL for the linea and console (VT-52) to use */
			*lawk = *wk;
			wk->lawk = lawk;
			lvs_clip(lawk, 0, 0);
			init_console(wk->lawk, linea_vars);
			wk->con = wk->lawk->con;

		/* Init the console driver (VT-52) */
			//init_console(wk->lawk, linea_vars);	/* !! This depends on systemfonts being fixedup, done in setup_virtual() */

		/* Install some vectors */
			set_linea_vector();
			install_console_handlers(wk->con);
			enable_xbios(wk);
		}

	/* Setup more LineA stuff */
		init_linea_vartab(wk, linea_vars);
	/* Add the mousecursor rendering function to VBI */
		(*drv->add_vbifunc)((unsigned long)wk->mouseapi->housekeep, 0);
	/* Add consoles textcursor blinker to VBI */
		(*drv->add_vbifunc)((unsigned long)wk->con->textcursor_blink, 25);

	/* Enable things ... */
		(*wk->timeapi->enable)();
		(*drv->enable_vbi)();
		(*wk->mouseapi->enable)();

		get_MiNT_info(wk);

		wk->handle = 1;
		v_vtab[1].pid = wk->pid;
		v_vtab[1].v = wk;
		pb->contrl[HANDLE] = 1;

	/* Clear the screen */
		lv_clrwk(wk);
	/* Prepare return information */
		prepare_stdreturn(pb, wk);

		(*wk->mouseapi->enablemcurs)();
	}
	else
error:	{
		pb->contrl[HANDLE] = 0;
	}

	return;
}

static short
setup_drawers_jumptable(OVDI_DRAWERS *src, OVDI_DRAWERS *dst, short planes)
{
	short i;

	*dst = defdrawers;

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

			if (src->draw_solid_rect)
				dst->draw_solid_rect = src->draw_solid_rect;
			else
				dst->draw_solid_rect = draw_solid_rect_1b;

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

			if (src->draw_solid_rect)
				dst->draw_solid_rect = src->draw_solid_rect;
			else
				dst->draw_solid_rect = draw_solid_rect_8b;

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

static void
update_devtab(DEV_TAB *dt, VIRTUAL *v)
{
	unsigned long palettesize;
	RASTER *r = v->raster;

	dt->xres	= r->w - 1;
	dt->yres	= r->h - 1;
	dt->wpixel	= r->wpixel;
	dt->hpixel	= r->hpixel;

	dt->cheights	= 0;

	if (r->planes == 1)
		dt->cancolor = 0;
	else
		dt->cancolor = 1;

	dt->colors = Planes2Pens[r->planes];

	palettesize = (long)v->rgb_levels->red * (long)v->rgb_levels->green * (long)v->rgb_levels->blue;

	if (palettesize > 32767UL)
		dt->palette = 0;
	else
		dt->palette = (unsigned short)palettesize;

	return;
}

static void
update_siztab(SIZ_TAB *st, VIRTUAL *v)
{
	return;
}

static void
update_inqtab(INQ_TAB *it, VIRTUAL *v)
{

	it->textfx		= F_SUPPORTED;
	it->planes		= v->driver->r.planes;
	it->lut			= v->driver->r.clut;
	it->mousebuttons	= v->mouseapi->buttons;
	return;
}

static void
get_MiNT_info(VIRTUAL *v)
{
	if (MiNT)
	{
		long fh, fl;
		char *p, *strt, *end;
		char pnbuff[132];

		v->pid = Pgetpid();

		fh = Fopen("u:\\kern\\self\\fname\0", O_RDONLY);
		if (fh >= 0)
		{
			fl = Fread(fh, 128L, (char *)&pnbuff);
			pnbuff[fl] = 0;
			Fclose(fh);
			if (fl >= 0)
			{
				p = (char *)&pnbuff;
				strt = end = 0;
				while (*p)
				{
					if (*p == 0x3a || *p == 0x2f || *p == 0x5c)
						strt = p + 1;
					else if (*p == 0x2e)
						end = p;
					p++;
				}
				if (!end)
					end = p;
				*end = 0;
				p = (char *)&v->procname[0];
				while (*strt)
					*p++ = *strt++;				
			}
		}
		else
			v->procname[0] = 0;
	}
	else
	{
		v->pid = -1;
		v->procname[0] = 0;
	}
}

void
v_opnvwk(VDIPB *pb, VIRTUAL *v)
{
	VIRTUAL *root, *new;
	OVDI_VTAB *entry;
	short i, handle, pid;

	handle = 0;
	pid = -1;

	new = (VIRTUAL *)Malloc(sizeof(VIRTUAL));

	if (new)
	{
		entry = v_vtab;

		for (i = 2; i < MAX_VIRTUALS; i++)
		{
			if (!(entry[i].v))
			{
				handle = i;
				break;
			}
		}

		if (handle)
		{
			bzero(new, sizeof(VIRTUAL));
			get_MiNT_info(new);

			entry[handle].v = new;
			entry[handle].pid = new->pid;

			root = v->root ? v->root : v;

			new->handle = handle;

			new->root	= root;
			new->lawk	= root->lawk;
			new->con	= root->con;
			new->driver	= root->driver;
			new->physical	= root->physical;
			new->raster	= root->raster;
			new->kbdapi	= root->kbdapi;
			new->mouseapi	= root->mouseapi;
			new->timeapi	= root->timeapi;

			new->color_vdi2hw	= root->color_vdi2hw;
			new->color_hw2vdi	= root->color_hw2vdi;
			new->pixelvalues	= root->pixelvalues;
			new->request_rgb	= root->request_rgb;
			new->actual_rgb		= root->actual_rgb;
			new->rgb_levels		= root->rgb_levels;
			new->rgb_bits		= root->rgb_bits;
			new->drawers		= root->drawers;
			new->utils		= root->utils;

			for (i = 0; i < 32; i++)
				new->odrawers[i] = root->odrawers[i];

			setup_virtual(pb, new, root);
			lvst_load_fonts(new);
			prepare_stdreturn(pb, new);

			//log("vwk: pid %d, name '%s'\n", new->pid, (char *)&new->procname);
		}
		else
		{
			Mfree(new);
		}
	}

	pb->contrl[HANDLE] = handle;

	return;
}



void
v_clswk( VDIPB *pb, VIRTUAL *root)
{
	short i;

/* DO NOT CLEAR 'handle' !! It indicates to the v_opnwk() that basic things are already initialized */
	if (root->root)
	{
		scrnlog("Cannot close physical with virtual handle!!!!\n");
		log("Cannot close physical with virtual handle!!!!\n");
		return;
	}

	lvs_clip(root, 0, 0);

	/* Make sure that all virtual workstations are closed */
	for (i = 2; i < MAX_VIRTUALS; i++)
	{
		if (v_vtab[i].v)
		{			
			if (MiNT)
			{
				if ( (Pkill(v_vtab[i].pid, 0) >= 0) && (v_vtab[i].pid != -1) )
					v_clsvwk(pb, v_vtab[i].v);
				else
				{
					v_vtab[i].v = 0;
					v_vtab[i].pid = -1;
				}
			}
			else
				v_clsvwk(pb, v_vtab[i].v);
		}
	}

	/* Disable mouse driver */
		(*root->mouseapi->disablemcurs)();
		(*root->mouseapi->disable)();
	/* remove user/next tim user functions */
		(*root->timeapi->disable)();
		(*root->timeapi->reset_user_tim)();
		(*root->timeapi->reset_next_tim)();
	/* remove consoles textcursor blinker from VBI */
	//	(*root->driver->f.remove_vbifunc)((unsigned long)root->con->textcursor_blink);
	/* remove the mousecursor rendering function from VBI */
		(*root->driver->remove_vbifunc)((unsigned long)root->mouseapi->housekeep);

	(*root->driver->dev->close)(root->driver);

	if (root->scratchp)
		Mfree(root->scratchp);

	v_vtab[1].v = 0;
	v_vtab[1].pid = -1;
	
	return;
}

void
v_clrwk( VDIPB *pb, VIRTUAL *v)
{
	lv_clrwk(v);
	return;
}

void
v_clsvwk( VDIPB *pb, VIRTUAL *v)
{
	OVDI_VTAB *entry;
	short handle;

	handle = v->handle;
	entry = v_vtab;

	if (handle > 1 && handle < MAX_VIRTUALS)
	{
		if (v->scratchp)
			Mfree(v->scratchp);

		Mfree(v);
		entry[handle].v = 0;
		entry[handle].pid = -1;
	}
	return;
}

/* unimplemented */
void
v_updwk( VDIPB *pb, VIRTUAL *v)
{
	return;
}

void
vq_extnd( VDIPB *pb, VIRTUAL *v)
{
	short flag = pb->intin[0];

	switch (flag)
	{
		case 0:
		{
			prepare_stdreturn(pb, v);
			break;
		}
		case 1:
		{
			prepare_extreturn(pb, v);
			break;
		}
		case 2:
		{
			prepare_scrninfreturn(pb, v);
			break;
		}
	}
	return;
}

static void
prepare_scrninfreturn( VDIPB *pb, VIRTUAL *v)
{
	short i, pfmt, planes;
	unsigned long palettesize;
	short *misc;
	RASTER *r;

	r = v->raster;

	pfmt = r->format; //v->driver->format;
	planes = r->planes; //v->driver->planes;

	if (pfmt & PF_ATARI)
		pb->intout[0] = 0;
	else if (pfmt & PF_PLANES)
		pb->intout[0] = 1;
	else if ((pfmt & PF_PACKED) || (pfmt & PF_FALCON))
		pb->intout[0] = 2;

	if (planes == 1)
		pb->intout[1] = 0;
	else if (r->clut)
		pb->intout[1] = 1;
	else
		pb->intout[1] = 2;

	pb->intout[2] = planes;

	if (planes > 8)
	{
		palettesize =	(unsigned long)v->rgb_levels->red *
				(unsigned long)v->rgb_levels->green *
				(unsigned long)v->rgb_levels->blue;
		pb->intout[3] = (unsigned short)((unsigned long)palettesize >> 16);
		pb->intout[4] = (unsigned short)((unsigned long)palettesize & 0xffff);
	}
	else
	{
		pb->intout[3] = 0;
		pb->intout[4] = Planes2Pens[planes];
	}

	pb->intout[5]	= r->bypl;

	pb->intout[6]	= (unsigned short)((unsigned long)r->base >> 16);
	pb->intout[7]	= (unsigned short)((unsigned long)r->base & 0xffff);
	
	pb->intout[8]	= v->rgb_bits->red;
	pb->intout[9]	= v->rgb_bits->green;
	pb->intout[10]	= v->rgb_bits->blue;
	pb->intout[11]	= v->rgb_bits->alpha;
	pb->intout[12]	= v->rgb_bits->ovl;

 /* Bit order */
	if (planes > 8)
	{
		char *pf;

		pb->intout[13] = planes - (	v->rgb_bits->red +
						v->rgb_bits->green +
						v->rgb_bits->blue +
						v->rgb_bits->alpha +
						v->rgb_bits->ovl);
		switch (planes)
		{
			case 15:
			case 16:
			{
				if (pfmt & PF_FALCON)
					pb->intout[14] = 1;
				else if (pfmt & PF_BS)
					pb->intout[14] = 7;
				else
					pb->intout[14] = 0;
				break;
			}
			case 24:
			{
				if (pfmt & PF_BS)
					pb->intout[14] = 7;
				else
					pb->intout[14] = 0;
				break;
			}
			case 32:
			{
				if (pfmt & PF_BS)
					pb->intout[14] = 7;
				else
					pb->intout[14] = 0;
				break;
			}
		}

		pf = r->pixelformat;
		misc = (short *)&pb->intout[16];
		while (pf[0])
		{
			while ( pf[2] )
			{
				for (i = 0; i < pf[0]; i++)
					*misc++ = pf[1] + i;
				pf += 3;
			}
			for (i = 0; i < pf[0]; i++)
				*misc++ = -1;

			pf += 3;
		}

		misc = (short *)&pb->intout[128];
		for (i = 128; i < 271; i++)
			*misc++ = 0;
		
	}
	else
	{
		//long *pxls = v->pixelvalues;
		short ncols;

		ncols = Planes2Pens[planes];

		pb->intout[13] = 0;
		pb->intout[14] = 0;
		misc = (short *)&pb->intout[16];
		for (i = 0; i < ncols; i++)
			*misc++ = v->color_vdi2hw[i];

		while (256 - ncols)
		{
			*misc++ = 0;
			ncols++;
		}
	}
	pb->intout[15] = 0;

	pb->contrl[N_INTOUT] = 272;

	return;
}	

static void
prepare_extreturn( VDIPB *pb, VIRTUAL *v)
{
	//short i;
	//short *src, *dst;

	memcpy( &pb->intout[0], &INQ_TAB_rom, sizeof(INQ_TAB));

	pb->intout[19] = v->clip_flag;

	//bzero(&pb->intout[20], (45-20) << 1);
	//pb->intout[20] = 0;

	pb->ptsout[0] = v->clip.x1;
	pb->ptsout[1] = v->clip.y1;
	pb->ptsout[2] = v->clip.x2;
	pb->ptsout[3] = v->clip.y2;

	bzero(&pb->ptsout[4], sizeof(SIZ_TAB) - 8);
#if 0
	pb->ptsout[4] = 0;
	pb->ptsout[5] = 0;
	pb->ptsout[6] = 0;
	pb->ptsout[7] = 0;
	pb->ptsout[8] = 0;
	pb->ptsout[9] = 0;
	pb->ptsout[10] = 0;
	pb->ptsout[11] = 0;
#endif


	pb->contrl[N_INTOUT] = 45;
	pb->contrl[N_PTSOUT] = 6;
	return;
}

static void
prepare_stdreturn( VDIPB *pb, VIRTUAL *v)
{
	//short *src, *dst;
	//short i;

	memcpy((void *)&pb->intout[0], (void *)&DEV_TAB_rom, sizeof(DEV_TAB));
	memcpy((void *)&pb->ptsout[0], (void *)&SIZ_TAB_rom, sizeof(SIZ_TAB));
	pb->contrl[N_INTOUT] = 45;
	pb->contrl[N_PTSOUT] = 6;
	return;
}
	

void
lv_clrwk(VIRTUAL *virtual)
{
	short tmp;
	VIRTUAL *v;
	VDIRECT corners;

	v = virtual->root ? virtual->root : virtual;

	lvs_clip(v, 0, 0);

	corners.x1 = corners.y1 = 0;
	corners.x2 = v->raster->w - 1;
	corners.y2 = v->raster->h - 1;

	tmp = v->fill.interior;
	v->fill.interior = FIS_SOLID;
	rectfill( v, &corners, &WhiteRect);
	v->fill.interior = tmp;

	return;
}

char fontdir[] = { "c:\\gemsys\\\0" };
char sysfnames[] =
{
//	"\0\0"
	"edms01.fnt\0"
	"edms03.fnt\0"
	"edms06.fnt\0"
	"edms07.fnt\0"
	"edms08.fnt\0"
	"edms09.fnt\0"
	"edms10.fnt\0"
	"edms12.fnt\0"
	"edms15.fnt\0"
	"gene09.fnt\0"
	"gene10.fnt\0"
	"gene12.fnt\0"
	"gene14.fnt\0"
	"gene18.fnt\0"
	"gene20.fnt\0"
	"gene24.fnt\0"
	"bagb08.fnt\0"
	"bage08.fnt\0"
	"bage09.fnt\0"
	"bigt13.fnt\0"
	"blks39.fnt\0"
	"blue10.fnt\0"
	"bubb14.fnt\0"
	"bubb24.fnt\0"
	"cair18.fnt\0"
	"cair24.fnt\0"
	"cali14.fnt\0"
	"cali24.fnt\0"
	"charcoal.fnt\0"
	"charcpro.fnt\0"
	"chic09.fnt\0"
	"chic12.fnt\0"
	"chic18.fnt\0"
	"chords.fnt\0"
	"cmlt14.fnt\0"
	"cmlt18.fnt\0"
	"cmlt36.fnt\0"
	"connect.fnt\0"
	"connligh.fnt\0"
	"cour09.fnt\0"
	"cour10.fnt\0"
	"cour12.fnt\0"
	"cour14.fnt\0"
	"cour18.fnt\0"
	"cour24.fnt\0"
	"cubant.fnt\0"
	"cubantlo.fnt\0"
	"grek13.fnt\0"
	"helv09.fnt\0"
	"helv10.fnt\0"
	"helv12.fnt\0"
	"helv14.fnt\0"
	"helv18.fnt\0"
	"helv24.fnt\0"

	"\0"
};

extern char systemfont08[];
extern char systemfont09[];
extern char systemfont10[];

static void
setup_virtual(VDIPB *pb, VIRTUAL *v, VIRTUAL *root)
{
	//register DEV_TAB *d = &DEV_TAB_rom;
	register struct opnwk_input *wkin;

	wkin	= (struct opnwk_input *)&pb->intin[0];

	v->scratchp	= 0;
	v->scratchs	= SCRATCH_BUFFER_SIZE;

	v->ptsbuffsiz	= PTSBUFF_SIZ;
/* **** wrmode */
	lvswr_mode( v, 1);

/* **** Clipping */
	lvs_clip( v, 0, 0);

/* **** Line settings */ 
	lvsl_type( v, wkin->linetype);
	lvsl_color( v, wkin->linecolor);
	lvsl_bgcolor( v, 0);
	lvsl_width( v, 1);
	lvsl_ends( v, 0, 0);
	lvsl_udsty( v, 0xffff);

/* **** Marker settings */
	lvsm_linetype( v, 0);
	lvsm_type( v, wkin->markertype);
	lvsm_color( v, wkin->markercolor);
	lvsm_bgcolor( v, 0);
	lvsm_height( v, 1);
	v->pmarker.scale = 0;

/* **** Fill stuff ... */
	lvsprm_color( v, wkin->fillcolor);
	lvsprm_bgcolor( v, 0);
	set_fill_params( FIS_SOLID, 0, &v->perimdata, 0, 0);

	lvsf_color ( v, wkin->fillcolor);
	lvsf_bgcolor ( v, 0);
	set_fill_params( wkin->fillinterior, wkin->fillstyle, &v->pattern, &v->fill.interior, &v->fill.style);
	lvsf_perimeter(v, 0);
	set_udfill( v, 0, 0, 0, 0);

/* **** Text stuff */


	//setup_virtual_fonts(v);

	/* If this is the root workstation, (root pointer == NULL),
	 * fonts were setup by setup_fonts().
	*/
	if (root)
	{
		v->font.loaded	= 0;
		v->font.lcount	= 0;
		v->font.pts	= 0;
		v->font.chup	= 0;
		v->font.scaled	= 0;
		v->font.sclsts	= 0;

		v->fring		= root->fring;
		v->font.num		= root->font.num;
		v->font.defid		= root->font.defid;
		v->font.defht		= root->font.defht;
		v->font.scratch_head	= 0;
	}

	lvst_color( v, wkin->textcolor);
	v->font.bgcol = VDI2HW_colorindex[0];
	lvst_alignment( v, 0, 0);
	lvst_effects( v, 0);
	lvst_rotation( v, 0);
	(void)lvst_font ( v, v->font.defid);
	(void)lvst_height ( v, v->font.defht);

	(void)lvsin_mode( v, 1, 1);
	(void)lvsin_mode( v, 2, 1);
	(void)lvsin_mode( v, 3, 1);
	(void)lvsin_mode( v, 4, 1);

	return;

}

/* Only to be executed for the physical workstation. Also updates the
 * INQ tab associated with that physical workstation.
*/
static void
setup_fonts(VIRTUAL *v, SIZ_TAB *st, DEV_TAB *dt)
{
	if (!sysfnt08p)
	{
		short i;
		long size;
		char *fnptrs, *fnptrd, *fdnptr;
		char fname[40];
		FONT_HEAD *f1;
		XGDF_HEAD *xf;

		st->minwchar = st->minhchar = 0x7fff;
		st->maxwchar = st->maxhchar = 0;

		v->font.pts	= 0;
		v->font.chup	= 0;
		v->font.scaled	= 0;
		v->font.sclsts	= 0;
		v->font.scratch_head = 0;

		v->font.num	= 1;

		f1 = (FONT_HEAD *)&systemfont08;
		xf = &xsystemfont08;
		xf->links = 1;
		xf->font_head = f1;
		for (i = 0; i < 256; i++)
			xf->cache[i] = 0;
		fixup_font(f1);
		sysfnt08p = xf; //f1;
		f1->id = 1;

		f1 = (FONT_HEAD *)&systemfont09;
		xf = &xsystemfont09;
		xf->links = 1;
		xf->font_head = f1;
		for (i = 0; i < 256; i++)
			xf->cache[i] = 0;
		fixup_font(f1);
		sysfnt09p = xf; //f1;
		f1->id = 1;
		if ((add_font(sysfnt08p, xf)) == 1) //f1)) == 1)
			v->font.num++;

		f1 = (FONT_HEAD *)&systemfont10;
		xf = &xsystemfont10;
		xf->links = 1;
		xf->font_head = f1;
		for (i = 0; i < 256; i++)
			xf->cache[i] = 0;
		fixup_font(f1);
		sysfnt10p = xf; //f1;
		f1->id = 1;
		if ((add_font(sysfnt08p, xf)) == 1) //f1)) == 1)
			v->font.num++;

		v->fring = sysfnt08p;
		v->font.defid = sysfnt10p->font_head->id;
		v->font.defht = sysfnt10p->font_head->top;
		v->font.lcount = 0;
		v->font.loaded = 0;

		if (sysfnames[0])
		{
			fnptrs = (char *)&sysfnames[0];

			for (;;)
			{
				fdnptr = &fontdir[0];
				fnptrd = (char *)&fname[0];

				while (*fdnptr)
					*fnptrd++ = *fdnptr++;

				while (*fnptrs)
					*fnptrd++ = *fnptrs++;

				*fnptrd++ = *fnptrs++;

				if (!load_font(&fname[0], &size, (long *)&xf)) //f1))
				{
					fixup_font(xf->font_head); //f1);

					for (i = 0; i < 256; i++)
						xf->cache[i] = 0;

					if (v->font.loaded)
					{
						if ((add_font(v->font.loaded, xf)) == 1) //f1)) == 1)
							v->font.lcount++;
					}
					else
					{
						v->font.loaded = xf; //f1;
						v->font.lcount++;
					}

					//log("Loaded font id %d, name: '%s'\n", f1->id, f1->name);
				}
				else
					//log("Cound not load font '%s' !!\n", &fname[0]);

				if (!(*fnptrs))
					break;
			}
		}
		//log("Loaded %d fontfaces\n", v->font.lcount);

		//log("*** Font-ring (sysfonts): ***\n");
		xf = sysfnt08p; //f1 = sysfnt08p;
		while(xf) //f1)
		{
			f1 = xf->font_head;
			//log("  id %d, point %d, height %d, name %s\n", f1->id, f1->point, f1->top, f1->name);

			if (f1->max_char_width < st->minwchar)
				st->minwchar = f1->max_char_width;
			if (f1->max_char_width > st->maxwchar)
				st->maxwchar = f1->max_char_width;

			if (f1->top < st->minhchar)
				st->minhchar = f1->top;
			if (f1->top > st->maxhchar)
				st->maxhchar = f1->top;

			xf = xf->next; //f1 = f1->next;
		}
		//log("*** Loaded fonts: ***\n");
		xf = v->font.loaded; //f1 = v->font.loaded;
		while(xf)
		{
			//log("  id %d, point %d, height %d, name %s\n", f1->id, f1->point, f1->top, f1->name);
			xf = xf->next;
		}

		dt->faces = v->font.num;

		//log("\n");
	}
	return;
}
