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
#include "memory.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "ovdi_rasters.h"
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


static void change_resolution(VIRTUAL *v);
static void reschange_devtab(DEV_TAB *, RASTER *);
static void update_devtab(DEV_TAB *, VIRTUAL *);
static void reschange_inqtab(INQ_TAB *, RASTER *);
static void update_inqtab(INQ_TAB *, VIRTUAL *);
static void update_siztab(SIZ_TAB *, VIRTUAL *);
static void prepare_stdreturn( VDIPB *, VIRTUAL *);
static void prepare_extreturn( VDIPB *, VIRTUAL *);
static void prepare_scrninfreturn( VDIPB *, VIRTUAL *);
static void get_MiNT_info(VIRTUAL *);
static void setup_virtual(VDIPB *, VIRTUAL *, VIRTUAL *);
static void setup_fonts(VIRTUAL *, SIZ_TAB *, DEV_TAB *);

/* Keep a 'root' color info structure */
static COLINF colinf;
static short vdi2hw[256];
static short hw2vdi[256];
static RGB_LIST request_rgb[256];
static RGB_LIST actual_rgb[256];
static unsigned long pixelvalues[256];

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
		//log("Only support for screen drivers!\n");
		if (v_vtab[1].v)
			prepare_stdreturn(pb, wk);
		goto error;
	}

	log("Opening physical ID %d\n", vdidev_id);

	MiNT = get_cookie((long)0x4d694e54/*"MiNT"*/, 0);

	if (v_vtab[1].v)
	{
		scrnlog("This should absolutely NOT happen, I think\n");
		log("This should absolutely NOT happen, I think\n");
		prepare_stdreturn(pb, wk);
		goto error;
	}

	if (wk->handle)
	{
		exit_console(wk->con);
	}

	drv = (*dev->open)(dev, vdidev_id);

	if (drv)
	{
		short vdipen;
		RASTER *r;
		COLINF *c;

		r = (RASTER *)&drv->r;
		wk->driver = drv;
		wk->physical = drv;
		wk->raster = r;

		init_raster(drv, r);

		c = &colinf;
		if (!wk->handle)
		{
			c->color_vdi2hw = (short *)&vdi2hw;
			c->color_hw2vdi = (short *)&hw2vdi;
			c->pixelvalues = (unsigned long *)&pixelvalues;
			c->request_rgb = (RGB_LIST *)&request_rgb;
			c->actual_rgb = (RGB_LIST *)&actual_rgb;
		}

		wk->raster = r;
		wk->colinf = c;

		raster_reschange(r, c);
		reschange_devtab(&DEV_TAB_rom, r);
		reschange_inqtab(&INQ_TAB_rom, r);
		linea_reschange(linea_vars, r, c);

		change_resolution(wk);

		if (wk->handle)
		{
			change_console_resolution(wk->con, r);
		}

		if (r->clut)
		{
			for (i = 0; i <= c->pens; i++)
			{
				vdipen = c->color_vdi2hw[i];
				lvs_color(wk, vdipen, &c->actual_rgb[vdipen]);
			}
		}

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

	/* Set physical/logical screen addresses */
		r->base = (*dev->setpscr)(drv, r->base);
		drv->log_base = (*dev->setlscr)(drv, r->base);
		*(long *)v_bas_ad = (long)r->base;

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

	/* Let mousedriver know about resolution */
		(*wk->mouseapi->setxmfres)(wk->raster, wk->colinf);

	/* Setup more LineA stuff */
		init_linea_vartab(wk, linea_vars);
	/* Add the mousecursor rendering function to VBI */
		(*drv->add_vbifunc)((unsigned long)wk->mouseapi->housekeep, 0);
	/* Add consoles textcursor blinker to VBI */
		if (!wk->handle)
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

static void
change_resolution(VIRTUAL *v)
{
	PatAttr *ptrn;
	RASTER *r;
	COLINF *c;

	r = v->raster;
	c = v->colinf;

/* Setup some commonly used PatAttr's */
	/* WHITE rectangle */
	ptrn = &WhiteRect;
	ptrn->expanded = 0;
	ptrn->color[0] = ptrn->color[1] = c->color_vdi2hw[0];
	ptrn->bgcol[0] = ptrn->bgcol[1] = c->color_vdi2hw[1];
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
	ptrn->color[0] = ptrn->color[1] = c->color_vdi2hw[1];
	ptrn->bgcol[0] = ptrn->bgcol[1] = c->color_vdi2hw[0];
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
}
static void
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
static void
update_devtab(DEV_TAB *dt, VIRTUAL *v)
{
	dt->cheights	= 0;
	return;
}
static void
update_siztab(SIZ_TAB *st, VIRTUAL *v)
{
	return;
}
static void
reschange_inqtab(INQ_TAB *it, RASTER *r)
{
	it->planes	= r->planes;
	it->lut		= r->clut;
}
static void
update_inqtab(INQ_TAB *it, VIRTUAL *v)
{
	it->textfx		= F_SUPPORTED;
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

	new = (VIRTUAL *)omalloc(sizeof(VIRTUAL), MX_PREFTTRAM | MX_SUPER); //MX_PRIVATE);

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

			if (root->raster->clut)
				new->colinf	= root->colinf;
			else
			{
				new->colinf = new_colinf(root->raster);
				if (new->colinf)
					clone_colinf(new->colinf, root->colinf);
				else
					new->colinf = root->colinf;
			}

			new->kbdapi	= root->kbdapi;
			new->mouseapi	= root->mouseapi;
			new->timeapi	= root->timeapi;

			setup_virtual(pb, new, root);
			lvst_load_fonts(new);
			prepare_stdreturn(pb, new);

			//log("vwk: pid %d, name '%s'\n", new->pid, (char *)&new->procname);
		}
		else
		{
			free_mem(new);
		}
	}

	pb->contrl[HANDLE] = handle;

	return;
}



void
v_clswk( VDIPB *pb, VIRTUAL *root)
{
	short i;
	RASTER *r;
	COLINF *c;

/* DO NOT CLEAR 'handle' !! It indicates to the v_opnwk() that basic things are already initialized */
	if (root->root)
	{
		scrnlog("Cannot close physical with virtual handle!!!!\n");
		log("Cannot close physical with virtual handle!!!!\n");
		return;
	}

	log("%s is Closing physical\n", root->procname);
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

 /* ANY MEMORY ALLOCATED FOR THE PROCESS THAT OPENED THE PHYSICAL */
 /* MUST NOW BE RELEASED!!! */
	r = root->raster;
	c = root->colinf;

	exit_console(root->con);
	(*root->driver->dev->close)(root->driver);

	raster_reschange(r, c);
	reschange_devtab(&DEV_TAB_rom, r);
	reschange_inqtab(&INQ_TAB_rom, r);
	linea_reschange(linea_vars, r, c);
	change_console_resolution(root->con, r);
	enter_console(root->con);

	if (root->scratchp)
		free_mem(root->scratchp);

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
			free_mem(v->scratchp);

		/* If virtual's colinf is not the same as the root colinf,
		*  we're in a TC/HC resolution (non-clut), and need to free
		*  it.
		*/
		if (v->colinf != v->root->colinf)
			free_mem(v->colinf);

		free_mem(v);
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
			log("doing scrninfreturn for %s!\n", v->procname);
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
		palettesize =	(unsigned long)r->rgb_levels.red *
				(unsigned long)r->rgb_levels.green *
				(unsigned long)r->rgb_levels.blue;
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
	
	pb->intout[8]	= r->rgb_bits.red;
	pb->intout[9]	= r->rgb_bits.green;
	pb->intout[10]	= r->rgb_bits.blue;
	pb->intout[11]	= r->rgb_bits.alpha;
	pb->intout[12]	= r->rgb_bits.ovl;

 /* Bit order */
	if (planes > 8)
	{
		char *pf;

		pb->intout[13] = planes - (	r->rgb_bits.red +
						r->rgb_bits.green +
						r->rgb_bits.blue +
						r->rgb_bits.alpha +
						r->rgb_bits.ovl);
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
			*misc++ = v->colinf->color_vdi2hw[i];

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

	pb->intout[19] = v->clip.flag;

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
	VIRTUAL *v;
	RASTER *r;

	v = virtual->root ? virtual->root : virtual;

	r = v->raster;

	lvs_clip(v, 0, 0);
	rectfill( r, v->colinf, (VDIRECT *)&r->x1, (VDIRECT *)&r->x1, &WhiteRect, FIS_SOLID);

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

					//log("Loaded font id %d, name: '%s'\n", xf->font_head->id, xf->font_head->name);
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
			f1 = xf->font_head;
			//log("  id %d, point %d, height %d, name %s\n", f1->id, f1->point, f1->top, f1->name);
			xf = xf->next;
		}

		dt->faces = v->font.num;

		//log("\n");
	}
	return;
}
