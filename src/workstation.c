//#include <mint/sysvars.h>
#include <mintbind.h>
//#include <osbind.h>
#include <fcntl.h>

#include "ovdi.h"
#include "colors.h"
#include "console.h"
#include "display.h"
#include "draw.h"
#include "fonts.h"
#include "file.h"
#include "gdf_defs.h"
#include "libkern.h"
#include "line.h"
#include "linea.h"
#include "memory.h"
#include "rasters.h"
#include "ovdi_defs.h"
#include "ovdi_rasters.h"
#include "vdi_defs.h"
#include "vdi_font.h"
#include "vdi_globals.h"
#include "workstation.h"
#include "xbios.h"

#include "v_attribs.h"
#include "v_fill.h"
#include "v_input.h"
#include "v_line.h"
#include "v_perimeter.h"
#include "v_pmarker.h"
#include "v_text.h"

#include "vbiapi.h"
#include "mouseapi.h"
#include "timerapi.h"

static void change_resolution(VIRTUAL *v);
static void update_devtab(DEV_TAB *, VIRTUAL *);
static void update_inqtab(INQ_TAB *, VIRTUAL *);
static void update_siztab(SIZ_TAB *, VIRTUAL *);
static void prepare_stdreturn( VDIPB *, VIRTUAL *);
static void prepare_extreturn( VDIPB *, VIRTUAL *);
static void prepare_scrninfreturn( VDIPB *, VIRTUAL *);
static void setup_virtual(VDIPB *, VIRTUAL *, VIRTUAL *);
static void load_vdi_fonts(VIRTUAL *, SIZ_TAB *, DEV_TAB *);
static void unload_vdi_fonts(VIRTUAL *v);

extern O_16 using_trap;

void
v_opnwk(VDIPB *pb, VIRTUAL *wk, VIRTUAL *lawk, OVDI_HWAPI *hwapi) //struct ovdi_device *dev)
{
	int i, vdidev_id;
	OVDI_DRIVER *drv;
	OVDI_DEVICE *dev;
	RASTER *r;
	register struct opnwk_input *wkin;

	if (using_trap)
	{
		(void)Ssystem( 3000, oVDI, 0x1L);
		using_trap = 0;
	}

#if 0
	if (contrl[SUBFUNCTION] == 1)
	{
		
		v_opnprn(
		return;
	}
#endif
	
	wkin = (struct opnwk_input *)&pb->intin[0];

	vdidev_id = wkin->id;

	/*
	 * Cannot open anything but screen for now
	*/
	if (vdidev_id < 1 || vdidev_id > 10)
	{
		scrnlog("Cannot open dev %d, (SUBFUNC %d). Only support for screen drivers!\n", vdidev_id, pb->contrl[SUBFUNCTION]);
		if (v_vtab[1].v)
			prepare_stdreturn(pb, wk);
		goto error;
	}

	MiNT = get_cookie((long)0x4d694e54/*"MiNT"*/, 0);

	/*
	 * Hmm.. how to react when an v_opnwk() call happens on an already opened
	 * physical workstation? 
	 * For now actually prepare valid return-values and return NULL for handle.
	*/
	if (v_vtab[1].v)
	{
		scrnlog("This should absolutely NOT happen, I think\n");
		//log("This should absolutely NOT happen, I think\n");
		prepare_stdreturn(pb, wk);
		goto error;
	}

	drv	= hwapi->driver;

	/*
	 * If this device is not already opened, the console uses another
	 * device. If already opened, it was opened during startup, and is
	 * already in use by the console driver.
	*/
	if (!drv)
	{
		/*
		 * Graphics hardware driver not opened, do it now, then.
		 *
		*/
		dev = hwapi->device;
		drv = open_device(hwapi);
		if (!drv)
		{
			scrnlog("v_opnwk: Could not open graphics hardware device!");
			goto error;
		}
		hwapi->driver = drv;
		r = &drv->r;
		init_raster(hwapi, r);
	}
	else
	{
		/*
		 * Exit console while we change raster physics
		*/
		exit_console(hwapi->console);
	}
	{
		int vdipen;
		COLINF *c;

		dev = drv->dev;		

		/*
		 * Ask driver to change to the specified video-mode.
		*/
		if (!MiNT)
			vdidev_id = 1;

		(void)(*dev->set_vdires)(drv, vdidev_id);

		r = &drv->r;
		wk->driver = drv;
		wk->physical = drv;
		wk->raster = r;
		wk->hw = hwapi;

		c = hwapi->colinf;

		wk->raster = r;
		wk->colinf = c;

		/*
		 * Change variables, etc. regarding resolution....
		*/
		raster_reschange(r, c);
		reschange_devtab(&DEV_TAB_rom, r);
		reschange_inqtab(&INQ_TAB_rom, r);
		linea_reschange(linea_vars, r, c);

		change_resolution(wk);

		/*
		 * If console is on another hardware driver (may be same device),
		 * we dont touch it here!
		*/
		if (hwapi->console->drv == drv)
			change_console_resolution(hwapi->console, r);

		/*
		 * Set initial hardware palette registers, if we're in clut mode.
		*/
		if (r->res.clut)
		{
			for (i = 0; i <= c->pens; i++)
			{
				vdipen = c->color_vdi2hw[i];
				lvs_color(wk, vdipen, &c->actual_rgb[vdipen]);
			}
		}

		/*
		 * Set physical/logical screen addresses.
		*/
		r->base = (*dev->setpscr)(drv, r->base);
		drv->log_base = (*dev->setlscr)(drv, r->base);
		v_bas_ad = (long)r->base;

		/*
		 * Keep a private table of the hardware API's this physical (and later,
		 * virtual) workstation uses.
		*/
		wk->mouseapi	= hwapi->mouse;
		wk->kbdapi	= hwapi->keyboard;
		wk->timeapi	= hwapi->time;
		wk->vbiapi	= hwapi->vbi;
		wk->con		= hwapi->console;
		wk->fontapi	= hwapi->font;

		/*
		 * Load the VDI fonts. This will load fonts specified with the
		 * 'vdi_fonts' configuration variable in ovdi.cnf
		*/
		load_vdi_fonts(wk, &SIZ_TAB_rom, &DEV_TAB_rom);

		/*
		 * Update some common tables to reflect new stuff. We have most information
		 * needed by now, like the number of buttons on mice, etc.
		*/
		update_devtab(&DEV_TAB_rom, wk);
		update_inqtab(&INQ_TAB_rom, wk);
		update_siztab(&SIZ_TAB_rom, wk);

		/*
		 * Setup the rest of the virtual structure
		*/
		setup_virtual(pb, wk, 0);

		/*
		 * Hmm.... do we need xbios compatibility before a physical workstation is
		 * opened (before AES starts) ? 
		*/
		if (!wk->handle)
		{
			enable_xbios(hwapi);
		}

		/*
		 * Let mousedriver know about resolution
		*/
		(*wk->mouseapi->setxmfres)(wk->raster, wk->colinf);
		/*
		 * Add the mousecursor rendering function to VBI
		*/
		(*wk->vbiapi->add_func)((O_u32)wk->mouseapi->housekeep, 0);

		/*
		 * Enable things ...
		*/
		(*wk->timeapi->enable)();
		(*wk->vbiapi->enable)();
		(*wk->mouseapi->enable)();

		get_MiNT_info(wk);

		wk->handle = 1;
		v_vtab[1].pid = wk->pid;
		v_vtab[1].v = wk;
		pb->contrl[HANDLE] = 1;

		lv_clrwk(wk);

		/*
		 * Prepare return information
		*/
		prepare_stdreturn(pb, wk);

		/*
		 * And lets turn on mousecursor...
		*/
		(*wk->mouseapi->enablemcurs)();
	}

	return;

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
	ptrn->interior = FIS_HOLLOW;
	ptrn->color[0] = ptrn->color[1] = c->color_vdi2hw[0];
	ptrn->bgcol[0] = ptrn->bgcol[1] = c->color_vdi2hw[1];
	if (r->res.planes > 8)
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
	ptrn->data = &SOLID;
	ptrn->exp_data = ( O_u16 *)&WRdata.edata;
	ptrn->mask = ( O_u16 *)&WRdata.mask;

	/* BLACK rectangle */
	ptrn = &BlackRect;
	ptrn->expanded = 0;
	ptrn->interior = FIS_SOLID;
	ptrn->color[0] = ptrn->color[1] = c->color_vdi2hw[1];
	ptrn->bgcol[0] = ptrn->bgcol[1] = c->color_vdi2hw[0];
	if (r->res.planes > 8)
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
	ptrn->data = &SOLID;
	ptrn->exp_data = ( O_u16 *)&BRdata.edata;
	ptrn->mask = ( O_u16 *)&BRdata.mask;
}

static void
update_devtab(DEV_TAB *dt, VIRTUAL *v)
{
	dt->cheights	= 0;
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
	it->mousebuttons	= v->mouseapi->buttons;
}

/*
 * Get name and pid of the process opening the workstation.
 * This only works with MiNT 1.16 and above, I think.
*/
void
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
				while (*strt) *p++ = *strt++;				
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
copy_common_virtual_vars(VIRTUAL *new, VIRTUAL *root)
{
	/*
	 * Things the virtual inherit from the root raster (the physical device)...
	*/
	new->root	= root;
	new->lawk	= root->lawk;
	new->con	= root->con;
	new->driver	= root->driver;
	new->physical	= root->physical;
	/*
	 * Hardware driver API's ...
	*/
	new->kbdapi	= root->kbdapi;
	new->mouseapi	= root->mouseapi;
	new->timeapi	= root->timeapi;
	new->vbiapi	= root->vbiapi;
	new->fontapi	= root->fontapi;
}

void
v_opnvwk(VDIPB *pb, VIRTUAL *v)
{
	VIRTUAL *root, *new;
	OVDI_VTAB *entry;
	int i, handle, pid;

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

			root = v->root ? v->root : v;

			new->handle = handle;

			copy_common_virtual_vars(new, root);

			if (pb->contrl[SUBFUNCTION] == 1)
			{
				O_Pos x2, y2;
				long colors;
				RASTER *r = root->raster;
				RASTER *nr;
				OVDI_HWAPI *hw = root->hw;
				OVDI_DRAWERS *drawers;
				MFDB *bm;
				struct opnwk_input *wkin = (struct opnwk_input *)&pb->intin[0];
				RESFMT resfmt;

				scrnlog("Open offscreen for %s\n", new->procname);

				bm = *(MFDB **)&(pb->contrl[7]);

				if ((x2 = wkin->dev.eddi.max_x) == 0)
					x2 = r->x2;
				if ((y2 = wkin->dev.eddi.max_y) == 0)
					y2 = r->y2;

				x2 |= 15;

				resfmt.format = 0;

				if (wkin->dev.eddi.planes)
				{
					resfmt.planes = wkin->dev.eddi.planes;
					if (wkin->dev.eddi.pixelformat == 0)
						resfmt.format = PF_ATARI;
					else if (wkin->dev.eddi.pixelformat == 1)
						resfmt.format = PF_PLANES;
					else if (wkin->dev.eddi.pixelformat == 2)
						resfmt.format = PF_PACKED;

					if (wkin->dev.eddi.endian & 2)
						resfmt.format |= PF_FALCON;
					if (wkin->dev.eddi.endian & 128)
						resfmt.format |= PF_BS;

					colors = wkin->dev.eddi.colors;
				}
				else if (bm->fd_nplanes)
				{
					resfmt.planes = bm->fd_nplanes;
				}
				else
				{
					resfmt.planes = r->res.planes;
				}
				drawers = hw->odrawers[resfmt.planes];

				if (!resfmt.format)
					resfmt.format = drawers->res->format;

				resfmt.clut = drawers->res->clut;
				resfmt.pixlen = drawers->res->pixlen;
				resfmt.pixelformat = drawers->res->pixelformat;

				if ( !(nr = new_raster(root->hw, bm->fd_addr, x2, y2, &resfmt)) )
					goto error;

				nr->wpixel = wkin->dev.eddi.wpixel;
				nr->hpixel = wkin->dev.eddi.hpixel;

				init_raster(root->hw, nr);
				init_raster_rgb(nr);
				nr->drawers = hw->odrawers[r->res.planes];
				{
					COLINF *c;
					c = new_colinf(nr->res.pixelformat);
					if (!c)
					{
						free_raster(nr);
						free_mem(new);
						goto error;
					}
					new->colinf = c;
				}
				init_colinf(nr, new->colinf);

				bm->fd_w	= nr->w;
				bm->fd_h	= nr->h;
				bm->fd_wdwidth	= (nr->w + 15) >> 4;
				bm->fd_nplanes	= nr->res.planes;
				bm->fd_r1 	= 0;
				bm->fd_r2	= 0;
				bm->fd_r3	= 0;

				if (bm->fd_addr)
				{
					if (bm->fd_stand == 1)
						trnfm(bm, bm);
				}
				bm->fd_addr = nr->base;

				scrnlog(" fd_adr %lx, w %d, h %d, wdw %d, planes %d \n",
						bm->fd_addr, bm->fd_w, bm->fd_h, bm->fd_wdwidth, bm->fd_nplanes);

				new->raster = nr;

				new->flags |= V_OSBM;
			}
			else
			{
				new->raster	= root->raster;

				/*
				 * If we are using TC/HC, (or a non-clut mode) we let every virtual
				 * workstation get its own colinf structure, making color related
				 * things work indipendantly. I dont know how other VDI's do this,
				 * but I WANT it to stay this way :)
				*/
				if (root->raster->res.clut)
					new->colinf	= root->colinf;
				else
				{
					new->colinf = new_colinf(root->raster->res.pixelformat);
					if (new->colinf)
						clone_colinf(new->colinf, root->colinf);
					else
						new->colinf = root->colinf;
				}
			}

			setup_virtual(pb, new, root);
			lvst_load_fonts(new);
			prepare_stdreturn(pb, new);

			entry[handle].v = new;
			entry[handle].pid = new->pid;
		}
		else
		{
error:
			handle = 0;
			free_mem(new);
		}
	}
	pb->contrl[HANDLE] = handle;
}


/*
 * Close a virtual workstation.
*/
void
v_clswk( VDIPB *pb, VIRTUAL *root)
{
	int i;
	RASTER *r;
	COLINF *c;

	/*
	 * DO NOT CLEAR 'handle' !! It indicates to the v_opnwk()
	 * that basic things are already initialized
	*/

	/*
	 * for attempts to close physical using wrong handle. If this
	 * worstation structure has a link to a root workstation, it is not
	 * root (physical) itself...
	*/
	if (root->root)
	{
		scrnlog("Cannot close physical with virtual handle!!!!\n");
		//log("Cannot close physical with virtual handle!!!!\n");
		return;
	}

	lvs_clip(root, 0, 0);

	/*
	 * Make sure that all virtual workstations spawned from this physical
	 * are closed.
	*/
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
	//	(*root->vbiapi->del_func)((O_u32)root->con->textcursor_blink);
	/* remove the mousecursor rendering function from VBI */
		(*root->vbiapi->del_func)((O_u32)root->mouseapi->housekeep);


	lvst_exit(root);

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

	unload_vdi_fonts(root);
	gdf_free_cache();

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
	int handle;

	handle = v->handle;
	entry = v_vtab;

	if (handle > 1 && handle < MAX_VIRTUALS)
	{

		lvst_exit(v);

		if (v->scratchp)
			free_mem(v->scratchp);

		/* If virtual's colinf is not the same as the root colinf,
		*  we're in a TC/HC resolution (non-clut), and need to free
		*  it.
		*/
		if (v->colinf != v->root->colinf)
			free_mem(v->colinf);

		if (v->flags & V_OSBM)
		{
			free_raster(v->raster);
		}

		if (v->flags & V_OWN_APPBUFF && v->app_buff)
			free_mem(v->app_buff);

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
	int flag = pb->intin[0];

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
			scrnlog("doing scrninfreturn for %s!\n", v->procname);
			prepare_scrninfreturn(pb, v);
			break;
		}
	}
	return;
}

static void
prepare_scrninfreturn( VDIPB *pb, VIRTUAL *v)
{
	int i, pfmt, planes;
	O_u32 palettesize;
	O_16 *misc;
	RASTER *r;

	r = v->raster;

	pfmt = r->res.format; //v->driver->format;
	planes = r->res.planes; //v->driver->planes;

	if (pfmt & PF_ATARI)
		pb->intout[0] = 0;
	else if (pfmt & PF_PLANES)
		pb->intout[0] = 1;
	else if ((pfmt & PF_PACKED) || (pfmt & PF_FALCON))
		pb->intout[0] = 2;

	if (planes == 1)
		pb->intout[1] = 0;
	else if (r->res.clut)
		pb->intout[1] = 1;
	else
		pb->intout[1] = 2;

	pb->intout[2] = planes;

	if (planes > 8)
	{
		palettesize =	(O_u32)r->rgb_levels.red *
				(O_u32)r->rgb_levels.green *
				(O_u32)r->rgb_levels.blue;
		pb->intout[3] = (O_u16)((O_u32)palettesize >> 16);
		pb->intout[4] = (O_u16)((O_u32)palettesize & 0xffff);
	}
	else
	{
		pb->intout[3] = 0;
		pb->intout[4] = Planes2Pens[planes];
	}

	pb->intout[5]	= r->bypl;

	pb->intout[6]	= (O_u16)((O_u32)r->base >> 16);
	pb->intout[7]	= (O_u16)((O_u32)r->base & 0xffff);
	
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
					pb->intout[14] = 2;
				else if (pfmt & PF_BS)
					pb->intout[14] = 128;
				else
					pb->intout[14] = 1;
				break;
			}
			case 24:
			{
				if (pfmt & PF_BS)
					pb->intout[14] = 128;
				else
					pb->intout[14] = 1;
				break;
			}
			case 32:
			{
				if (pfmt & PF_BS)
					pb->intout[14] = 128;
				else
					pb->intout[14] = 1;
				break;
			}
		}

		pf = r->res.pixelformat;
		misc = (O_16 *)&pb->intout[16];
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

		misc = (O_16 *)&pb->intout[128];
		for (i = 128; i < 271; i++)
			*misc++ = 0;
		
	}
	else
	{
		//long *pxls = v->pixelvalues;
		int ncols;

		ncols = Planes2Pens[planes];

		pb->intout[13] = 0;
		pb->intout[14] = 0;
		misc = (O_16 *)&pb->intout[16];
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
	INQ_TAB *it = (INQ_TAB *)&pb->intout[0];
	RASTER *r = v->raster;

	memcpy( &pb->intout[0], &INQ_TAB_rom, sizeof(INQ_TAB));

	reschange_inqtab(it, r);

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
	DEV_TAB *dt = (DEV_TAB *)&pb->intout[0];
	RASTER *r = v->raster;

	memcpy((void *)&pb->intout[0], (void *)&DEV_TAB_rom, sizeof(DEV_TAB));
	reschange_devtab(dt, r);
	memcpy((void *)&pb->ptsout[0], (void *)&SIZ_TAB_rom, sizeof(SIZ_TAB));
	pb->contrl[N_INTOUT] = 45;
	pb->contrl[N_PTSOUT] = 6;
	return;
}
	
/* Just clear the raster */
void
lv_clrwk(VIRTUAL *virtual)
{
	VIRTUAL *v;
	RASTER *r;

	v = virtual->root ? virtual->root : virtual;

	r = v->raster;

	lvs_clip(v, 0, 0);
	rectfill( r, v->colinf, (VDIRECT *)&r->x1, (VDIRECT *)&r->x1, &WhiteRect);

}
/*
 * Set up things common to all workstation structures.
*/
static void
setup_virtual(VDIPB *pb, VIRTUAL *v, VIRTUAL *root)
{
	//register DEV_TAB *d = &DEV_TAB_rom;
	register struct opnwk_input *wkin;

	wkin	= (struct opnwk_input *)&pb->intin[0];

	v->scratchp	= 0;
	v->scratchs	= SCRATCH_BUFFER_SIZE;

	v->ptsbuffsiz	= PTSBUFF_SIZ;
	v->spanbuffsiz	= SPANBUFF_SIZ;

/* **** Clipping */
	lvs_clip( v, 0, 0);

/* **** Line settings */ 
	lvsl_initial( v );
	lvsl_wrmode( v, MD_REPLACE );
	lvsl_type( v, wkin->linetype);
	lvsl_color( v, wkin->linecolor);
	lvsl_bgcolor( v, 0);
	lvsl_width( v, 1);
	lvsl_ends( v, 0, 0);
	lvsl_udsty( v, 0xffff);

/* **** Marker settings */
	lvsm_initial( v );
	lvsm_wrmode( v, MD_REPLACE );
	lvsm_linetype( v, 0);
	lvsm_type( v, wkin->markertype);
	lvsm_color( v, wkin->markercolor);
	lvsm_bgcolor( v, 0);
	lvsm_height( v, 1);
	v->pmarker.t.p.scale = 0;

/* **** Fill stuff ... */
#if 0
	v->fill.exp_data	= (O_u16 *)&v->filldata.edata;
	v->fill.mask		= (O_u16 *)&v->filldata.mask;

	v->udfill.data		= (O_u16 *)&v->udfilldata.data;
	v->udfill.mask		= (O_u16 *)&v->udfilldata.mask;
	v->udfill.exp_data	= (O_u16 *)&v->udfilldata.edata;
#endif
/* **** Perimeter-line attribs ... */
	lvsprm_initial( v );
	lvsprm_wrmode( v, MD_REPLACE );
	lvsprm_type( v, 0);
	lvsprm_color( v, wkin->fillcolor);
	lvsprm_bgcolor( v, 0);
	lvsprm_width( v, 1);
	lvsprm_ends( v, 0, 0);
	lvsprm_udsty( v, 0xffff);

	lvsf_initial( v );
	lvsf_wrmode( v, MD_REPLACE );
	lvsudf_wrmode( v, MD_REPLACE );
	lvsf_color ( v, wkin->fillcolor);
	lvsf_bgcolor ( v, 0);
	/* Potential bug - what if fillinterior is set to FIS_USER here?? */
	/* Not a problem anylonger */
	if (wkin->fillinterior == FIS_USER)
		v->currfill = &v->udfill;
	else
		v->currfill = &v->fill;

	set_fill_params( wkin->fillinterior, wkin->fillstyle, &v->fill, &v->fill.interior, &v->fill.t.f.style);
	lvsf_perimeter( v, 0);
	set_udfill( v, 0, 0, 0, 0);

/* **** Text stuff */
	/* If this is the root workstation, (root pointer == NULL),
	 * fonts were setup by load_vdi_fonts(). Else we copy initial
	 * font config from root.
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

/* **** wrmode */
	lvswr_mode( v, 1);
}

/* Only to be executed for the physical workstation. Also updates the
 * INQ tab associated with that physical workstation.
*/
static int fonts_are_loaded = 0;

static void
load_vdi_fonts(VIRTUAL *v, SIZ_TAB *st, DEV_TAB *dt)
{
	if (!fonts_are_loaded)
	{
		char *fnptrs, *fnptrd, *fdnptr;
		char fname[40];
		XGDF_HEAD *xf;
		struct gdf_membuff *m = &loaded_vdi_gdfs;

	/* I know this is not needed here right now .. but later? */
		st->minwchar = sysfnt_minwchar;
		st->maxwchar = sysfnt_maxwchar;
		st->minhchar = sysfnt_minhchar;
		st->maxhchar = sysfnt_maxhchar;

		v->font.pts	= 0;
		v->font.chup	= 0;
		v->font.scaled	= 0;
		v->font.sclsts	= 0;
		v->font.scratch_head = 0;

		v->font.num	= sysfnt_faces;

		v->fring = sysfnt08p;
		v->font.defid = sysfnt10p->font_head->id;
		v->font.defht = sysfnt10p->font_head->top;
		v->font.lcount = 0;
		v->font.loaded = 0;

		if ((fnptrs = vdi_fontlist))
		{
			long fs, size;
			char *tmp;

			bzero(m, sizeof(struct gdf_membuff));
			size = 0;

			/* First, figure out how large a buffer we need to load all fonts */
			tmp = fnptrs;
			while (*tmp)
			{
				fdnptr = gdf_path;
				fnptrd = (char *)&fname[0];

				while (*fdnptr) *fnptrd++ = *fdnptr++;
				while (*tmp) *fnptrd++ = *tmp++;

				*fnptrd++ = *tmp++;

				fs = get_file_size ( &fname[0] );
				if (fs > 0)
				{
					size += (fs + sizeof(XGDF_HEAD ) + 3) & ~3;
				}
			}
			/* .. then we attempt to allocate ram for this buffer .. */
			if (size)
			{
				long mem;

				mem = (long)omalloc(size, MX_PREFTTRAM | MX_SUPER); //READABLE);
				if (!mem)
					return;
				m->base	= (char *)mem;
				m->free	= (char *)mem;
				m->size	= size;
			}
			/* Then we start loading the fonts off the disk .... */
			for (;;)
			{
				fdnptr = gdf_path;
				fnptrd = (char *)&fname[0];

				while (*fdnptr)	*fnptrd++ = *fdnptr++;
				while (*fnptrs)	*fnptrd++ = *fnptrs++;

				*fnptrd++ = *fnptrs++;

				if ( !load_font(&fname[0], &size, (long *)&xf, m) )
				{
					fixup_font(xf->font_head);

					if (v->font.loaded)
					{
						if ((add_font(v->font.loaded, xf)) == 1)
							v->font.lcount++;
					}
					else
					{
						v->font.loaded = xf;
						v->font.lcount++;
					}

					//log("Loaded font id %d, name: '%s'\n", xf->font_head->id, xf->font_head->name);
				}
				else
					//log("Cound not load font '%s' !!\n", &fname[0]);

				if (!(*fnptrs))
					break;
			}

			/* If no fonts were loaded, no use keeping the buffer */
			if (m->base == m->free)
			{
				fonts_are_loaded = 0;
				if (m->base)
				{
					free_mem(m->base);
					(long)m->base = m->size = (long)m->free = 0;
				}
			}
			else
				fonts_are_loaded = 1;
			
		}
	}
}

static void
unload_vdi_fonts(VIRTUAL *v)
{
	struct gdf_membuff *m = &loaded_vdi_gdfs;

	if (m->base)
		free_mem(m->base);
	bzero(m, sizeof(struct gdf_membuff));

	v->font.pts	= 0;
	v->font.chup	= 0;
	v->font.scaled	= 0;
	v->font.sclsts	= 0;
	v->font.scratch_head = 0;

	v->font.lcount	= 0;
	v->font.loaded	= 0;

	fonts_are_loaded = 0;
}
