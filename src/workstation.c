//#include <mint/sysvars.h>
#include <mintbind.h>
//#include <osbind.h>
#include <fcntl.h>

#include "colors.h"
#include "console.h"
#include "display.h"
#include "draw.h"
#include "fonts.h"
#include "kbddrv.h"
#include "libkern.h"
#include "linea.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
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
	int i;
	OVDI_DRIVER *drv;


	MiNT = get_cookie((long)0x4d694e54/*"MiNT"*/, 0);
	log("MiNT %d\n", MiNT);

	bzero(wk, sizeof(VIRTUAL));

	if (lawk)
	{
		bzero(lawk, sizeof(VIRTUAL));
	}


	for ( i = 0 ; i < MAX_VIRTUALS ; i++ )
	{
		v_vtab[i].v	= 0;
		v_vtab[i].pid	= -1;
	}

	drv = (*dev->open)(dev, pb->intin[0]);

	log(" drv %lx\n", drv);

	if (drv)
	{
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

		if (scrsizmm_x)
			r->wpixel = (scrsizmm_x * 1000) / r->w;
		else
			r->wpixel = 372;

		if (scrsizmm_y)
			r->hpixel = (scrsizmm_y * 1000) / r->h;
		else
			r->hpixel = 372;

	/* Check for necessary functions which the current graphics card driver */
	/* didnt have 								*/
		switch (r->planes)
		{
			case 8:
			{
				if (!drv->f.raster_operations)
				{
					log("Using std_driver 8b raster ops!\n");
					drv->f.raster_operations = rops_8b;
				}
				if (!drv->f.draw_mc)
					drv->f.draw_mc = draw_mousecurs_8b;
				if (!drv->f.undraw_mc)
					drv->f.undraw_mc = restore_msave_8b;

				if (!drv->f.put_pixel)
					drv->f.put_pixel = put_pixel_8b;
				if (!drv->f.get_pixel)
					drv->f.get_pixel = get_pixel_8b;

				break;
			}
			case 15:
			{
				if (!drv->f.raster_operations)
				{
					display("Using std_driver 16b raster ops!\n");
					drv->f.raster_operations = rops_16b;
				}
				break;
			}
			case 16:
			{
				if (!drv->f.raster_operations)
				{
					display("Using std_driver 16b raster ops!\n");
					drv->f.raster_operations = rops_16b;
				}
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
				display("Unknown color resolution!!\n");
				goto error;
			}
		}

	/* Check for available color indipendant driver services */
		if (!drv->f.add_vbifunc)
		{
			init_vbi();
			drv->f.get_vbitics = get_vbitics;
			drv->f.add_vbifunc = add_vbi_function;
			drv->f.remove_vbifunc = remove_vbi_function;
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
			VDI2HW_colorindex[1] = 15;
			HW2VDI_colorindex[15] = 1;
			pens = 16;
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
		ptrn = &WhiteRect;
		ptrn->expanded = 0;
		ptrn->color[0] = ptrn->color[1] = VDI2HW_colorindex[0];
		ptrn->color[2] = ptrn->color[3] = 0xff;
		ptrn->bgcol = VDI2HW_colorindex[1];
		ptrn->width = 16;
		ptrn->height = 1;
		ptrn->wwidth = 1;
		ptrn->planes = 1;
		ptrn->wrmode = 0;
		ptrn->mask = 0xffff;
		ptrn->data = &SOLID;

		ptrn = &BlackRect;
		ptrn->expanded = 0;
		ptrn->color[0] = ptrn->color[1] = VDI2HW_colorindex[1];
		ptrn->color[2] = ptrn->color[3] = 0xff;
		ptrn->bgcol = VDI2HW_colorindex[0];
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

		setup_fonts(wk, &SIZ_TAB_rom, &DEV_TAB_rom);
		update_devtab(&DEV_TAB_rom, wk);
		update_inqtab(&INQ_TAB_rom, wk);
		update_siztab(&SIZ_TAB_rom, wk);

	/* Setup the rest of the virtual structure */
		setup_virtual(pb, wk, 0);
	/* Setup more LineA stuff */
		init_linea_vartab(wk, linea_vars);
	/* Init the console driver (VT-52) */
		init_console(wk, linea_vars);		/* !! This depends on systemfonts being fixedup, done in setup_virtual() */

#if 0
	/* Add the mousecursor rendering function to VBI */
		(*drv->f.add_vbifunc)((unsigned long)wk->mouseapi->housekeep, 0);
#endif

	/* Install some vectors */
		set_linea_vector();
		install_console_handlers(wk->con);
		install_xbios(wk);

	/* Add the mousecursor rendering function to VBI */
		(*drv->f.add_vbifunc)((unsigned long)wk->mouseapi->housekeep, 0);
	/* Add consoles textcursor blinker to VBI */
		(*drv->f.add_vbifunc)((unsigned long)wk->con->textcursor_blink, 0);

		get_MiNT_info(wk);
		log("pid %d, name '%s' opened physical\n", wk->pid, (char *)&wk->procname);
		wk->handle = 1;
		v_vtab[1].pid = wk->pid;
		v_vtab[1].v = wk;
		pb->contrl[HANDLE] = 1;

	/* Clear the screen */
		lv_clrwk(wk);
	/* Prepare return information */
		prepare_stdreturn(pb, wk);
	}
	else
error:	{
		pb->contrl[HANDLE] = 0;
	}

	return;
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
			if (fl < 0)
				fl = 0;
			pnbuff[fl] = 0;
			Fclose(fh);
			if (fl)
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
	short handle, pid;

	handle = 0;
	pid = -1;

	new = (VIRTUAL *)Malloc(sizeof(VIRTUAL));

	if (new)
	{
		short i;

		entry = v_vtab;

		for (i = 1; i < MAX_VIRTUALS; i++)
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

			setup_virtual(pb, new, root);
			lvst_load_fonts(new);
			prepare_stdreturn(pb, new);

			log("vwk: pid %d, name '%s'\n", new->pid, (char *)&new->procname);
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
v_clswk( VDIPB *pb, VIRTUAL *virtual)
{
	VIRTUAL *v;
	OVDI_VTAB *entry;
	short i;

	entry = v_vtab;

	for (i = 2; i < MAX_VIRTUALS; i++)
	{
		if (entry[i].v)
			v_clsvwk(pb, entry[i].v);
	}

	v = virtual->root ? virtual->root : virtual;
	(*v->driver->dev->close)(v->driver);

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
#if 0
		case 2:
		{
			prepare_scrninfreturn(pb, v);
			break;
		}
#endif
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

	pb->ptsout[0] = v->clip.x1;
	pb->ptsout[1] = v->clip.y1;
	pb->ptsout[2] = v->clip.x2;
	pb->ptsout[3] = v->clip.y2;

	pb->ptsout[4] = 0;
	pb->ptsout[5] = 0;
	pb->ptsout[6] = 0;
	pb->ptsout[7] = 0;
	pb->ptsout[8] = 0;
	pb->ptsout[9] = 0;
	pb->ptsout[10] = 0;
	pb->ptsout[11] = 0;


	pb->intout[20] = 0;

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
	VIRTUAL *v;
	VDIRECT corners;

	v = virtual->root ? virtual->root : virtual;

	lvs_clip(v, 0, 0);

	corners.x1 = corners.y1 = 0;
	corners.x2 = v->raster->w - 1;
	corners.y2 = v->raster->h - 1;

	rectfill( v, &corners, &WhiteRect);

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

/* **** wrmode */
	lvswr_mode( v, 1);

/* **** Clipping */
	lvs_clip( v, 0, 0);

/* **** Line settings */ 
	lvsl_type( v, wkin->linetype);
	lvsl_color( v, wkin->linecolor);
	lvsl_width( v, 1);
	lvsl_ends( v, 0, 0);
	lvsl_udsty( v, 0);

/* **** Marker settings */
	lvsm_linetype( v, 0);
	lvsm_type( v, wkin->markertype);
	lvsm_color( v, wkin->markercolor);
	lvsm_height( v, 1);
	v->pmarker.scale = 0;

/* **** Fill stuff ... */
	lvsprm_color( v, wkin->fillcolor);
	set_fill_params( FIS_SOLID, 0, &v->perimdata, 0, 0);

	lvsf_color ( v, wkin->fillcolor);
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
		long size;
		char *fnptrs, *fnptrd, *fdnptr;
		char fname[40];

		FONT_HEAD *f1;

		st->minwchar = st->minhchar = 0x7fff;
		st->maxwchar = st->maxhchar = 0;

		v->font.pts	= 0;
		v->font.chup	= 0;
		v->font.scaled	= 0;
		v->font.sclsts	= 0;
		v->font.scratch_head = 0;

		v->font.num	= 1;

		f1 = (FONT_HEAD *)&systemfont08;
		fixup_font(f1);
		sysfnt08p = f1;
		f1->id = 1;

		f1 = (FONT_HEAD *)&systemfont09;
		fixup_font(f1);
		sysfnt09p = f1;
		f1->id = 1;

		if ((add_font(sysfnt08p, f1)) == 1)
			v->font.num++;

		f1 = (FONT_HEAD *)&systemfont10;
		fixup_font(f1);
		sysfnt10p = f1;
		f1->id = 1;

		if ((add_font(sysfnt08p, f1)) == 1)
			v->font.num++;

		v->fring = sysfnt08p;
		v->font.defid = sysfnt10p->id;
		v->font.defht = sysfnt10p->top;		
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

				if (!load_font(&fname[0], &size, (long *)&f1))
				{
					fixup_font(f1);

					if (v->font.loaded)
					{
						if ((add_font(v->font.loaded, f1)) == 1)
							v->font.lcount++;
					}
					else
					{
						v->font.loaded = f1;
						v->font.lcount++;
					}

					log("Loaded font id %d, name: '%s'\n", f1->id, f1->name);
				}
				else
					log("Cound not load font '%s' !!\n", &fname[0]);

				if (!(*fnptrs))
					break;
			}
		}
		log("Loaded %d fontfaces\n", v->font.lcount);

		log("*** Font-ring (sysfonts): ***\n");
		f1 = sysfnt08p;
		while(f1)
		{
			log("  id %d, point %d, height %d, name %s\n", f1->id, f1->point, f1->top, f1->name);

			if (f1->max_char_width < st->minwchar)
				st->minwchar = f1->max_char_width;
			if (f1->max_char_width > st->maxwchar)
				st->maxwchar = f1->max_char_width;

			if (f1->top < st->minhchar)
				st->minhchar = f1->top;
			if (f1->top > st->maxhchar)
				st->maxhchar = f1->top;

			f1 = f1->next;
		}
		log("*** Loaded fonts: ***\n");
		f1 = v->font.loaded;
		while(f1)
		{
			log("  id %d, point %d, height %d, name %s\n", f1->id, f1->point, f1->top, f1->name);

#if 0
			if (f1->max_char_width < st->minwchar)
				st->minwchar = f1->max_char_width;
			if (f1->max_char_width > st->maxwchar)
				st->maxwchar = f1->max_char_width;

			if (f1->top < st->minhchar)
				st->minhchar = f1->top;
			if (f1->top > st->maxhchar)
				st->maxhchar = f1->top;
#endif

			f1 = f1->next;
		}

		dt->faces = v->font.num;

		log("\n");
	}
	return;
}
