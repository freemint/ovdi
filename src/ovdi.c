#include <osbind.h>
#include <mint/mintbind.h>
#include <mint/basepage.h>
#include <mint/dcntl.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdio.h>

#include "console.h"
#include "display.h"

//#include "draw.h"
#include "fonts.h"
//#include "line.h"
//#include "rasters.h"
//#include "polygon.h"

#include "ovdi.h"
#include "ovdi_defs.h"
#include "ovdi_rasters.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "workstation.h"
#include "funcdef.h"
#include "libkern.h"

#include "kbddrv.h"
#include "mousedrv.h"
#include "vbi.h"
#include "time.h"

#include "std_driver.h"

/* #include "styles.h" */
#include "tables.h"	/* Contains the function jumptable, dev/siz/inq tabs amogst other things */

//#include "line.h"

#include "xbios.h"

#ifdef PRG_TEST
#endif

void v_nosys( VDIPB *, VIRTUAL *);

long justtest;
short MiNT = 0;
char bootdev;

OVDI_DEVICE * (*devinit)(OVDI_LIB *l);
OVDI_DEVICE *device = 0;
OVDI_HWAPI hw_api;

/* Keep a 'root' color info structure */
static COLINF colinf;
static short vdi2hw[256];
static short hw2vdi[256];
static RGB_LIST request_rgb[256];
static RGB_LIST actual_rgb[256];
static unsigned long pixelvalues[256];

//char fontdata[1024 * 10];
extern BASEPAGE *_base;

void
ovdi_init(void)
{
	long lavt, lafr, laft;

	//scrnlog("OVDI start adr %lx, ends at adr %lx\n", _base->p_tbase, _base->p_tbase + _base->p_tlen + _base->p_blen + _base->p_dlen);
	//log("OVDI start adr %lx\n", _base->p_tbase);

	bzero(&wks1, sizeof(VIRTUAL));
	bzero(&la_vt, sizeof(LINEA_VARTAB));

	get_linea_addresses(&lavt, &lafr, &laft);
	lavt -= 910;
	linea_vars = (LINEA_VARTAB *)lavt;

	//scrnlog("Linea vartab %lx, font ring %lx, func tab %lx", linea_vars, lafr, laft);

	devinit = &device_init;
	bootdev = Dgetdrv() + 'a';

	init_systemfonts(&SIZ_TAB_rom, &DEV_TAB_rom);

/* ---------------------------- */
#if 1
	{
		OVDI_DRIVER *drv;
		OVDI_HWAPI *hw = &hw_api;
		RASTER *r;
		COLINF *c;
		long  usp;

		device	= (*devinit)(&ovdilib);
		drv	= (*device->open)(device);

		r = (RASTER *)&drv->r;
		c = &colinf;

		hw->driver	= drv;
		hw->colinf	= c;
		hw->vbi		= init_vbi();
		hw->mouse	= init_mouse(drv, linea_vars);
		hw->keyboard	= init_keyboard();
		hw->time	= init_time(linea_vars);

		c->color_vdi2hw = (short *)&vdi2hw;
		c->color_hw2vdi = (short *)&hw2vdi;
		c->pixelvalues = (unsigned long *)&pixelvalues;
		c->request_rgb = (RGB_LIST *)&request_rgb;
		c->actual_rgb = (RGB_LIST *)&actual_rgb;

		init_raster(drv, r);
		raster_reschange(r, c);

		hw->console	= init_console(hw, r, linea_vars);

		reschange_devtab(&DEV_TAB_rom, r);
		reschange_inqtab(&INQ_TAB_rom, r);
		init_linea_vartab(0, linea_vars);
		linea_reschange(linea_vars, r, c);

		set_linea_vector();
		install_console_handlers(hw->console);
		install_xbios();

		usp = Super(1);
		if (!usp)
			usp = Super(0);
		else
			usp = 0;

		(*hw->vbi->add_func)((unsigned long)hw->console->textcursor_blink, 25);
		(*hw->vbi->enable)();
		enter_console(hw->console);

		if (usp)
			Super(usp);

	}

#endif
/* ---------------------------- */

	old_trap2_vec = (long) Setexc(0x22, New_Trap2);

	scrnlog("OVDI start adr %lx, ends at adr %lx\n", _base->p_tbase, _base->p_tbase + _base->p_tlen + _base->p_blen + _base->p_dlen);
	scrnlog("Linea vartab %lx, font ring %lx, func tab %lx", linea_vars, lafr, laft);

	return;
}

short logit = 0;

long
oVDI( VDIPB *pb )
{
	short func;
	VDI_function *f;
	VIRTUAL *v;

	func = pb->contrl[FUNCTION];

	if (func == 241)	/* remove me! */
		func = 8;

	if (func > MAX_VDI_FUNCTIONS)
	{

#if 0
		v = v_vtab[pb->contrl[HANDLE]].v;
		if (v)
			log("Calling old trap 2, func %d: pid %d %s\n", func, v->pid, v->procname);
		else
			log("Calling old trap 2, func %d - ILLEGAL HANDLE\n", func);
		
#endif
		return -1L;
	}

	f = (VDI_function *)v_jmptab[func];

	/* Opening a physical is a special case */
	if (func == OPEN_WORKSTATION)
	{
		if (!device)
			device = (*devinit)(&ovdilib);

		if (device)
		{
			//linea_vars = &la_vt;

			//(*f)(pb, &wks1, &la_wks, device);
			v_opnwk(pb, &wks1, &la_wks, &hw_api); //device);
			return 0L;
		}
		else
		{
			pb->contrl[HANDLE] = 0;
			return 0L;
		}
	}

	/* We pass the owning physical workstation's VIRTUAL to
	 * v_opnvwk
	*/
	v = v_vtab[pb->contrl[HANDLE]].v;

	/* No special cases, get correct VIRTUAL and
	 *just call the function
	*/

	pb->contrl[N_PTSOUT] = 0;
	pb->contrl[N_INTOUT] = 0;

	if (!v)
	{
		return 0L;
	}

	v->func = func;

#if 0
//	if ( !(strcmp("PROFILE2", v->procname)) )
	if ( (Kbshift(-1) & 0x1) )// && func == 121)
		logit = 1;
	else
		logit = 0;

	if (logit) // && func == 0)
	{

		//log("%s, func %d..", func);

		//log("func %d subfuc %d, %d, %d, %d, %d ..", func, pb->contrl[SUBFUNCTION],
		//	pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3]);

		log("%s (%d) - func %d, subfuc %d, ni %d, np %d\n",
			v->procname, v->pid, func, pb->contrl[SUBFUNCTION], pb->contrl[N_INTIN], pb->contrl[N_PTSIN]);
		log("  in %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d, %d, %d, %d, %d\n",
			pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3],
			pb->intin[4], pb->intin[5], pb->intin[6], pb->intin[7],
			pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
			pb->ptsin[4], pb->ptsin[5], pb->ptsin[6], pb->ptsin[7]);

		log("     %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d, %d, %d, %d, %d\n",
			pb->intout[0], pb->intout[1], pb->intout[2], pb->intout[3],
			pb->intout[4], pb->intout[5], pb->intout[6], pb->intout[7],
			pb->ptsout[0], pb->ptsout[1], pb->ptsout[2], pb->ptsout[3],
			pb->ptsout[4], pb->ptsout[5], pb->ptsout[6], pb->ptsout[7]);
	}

	(*f)(pb, v);

	if (logit)// && func == 0)
	{
		log(" out %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d, %d, %d, %d, %d\n",
			pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3],
			pb->intin[4], pb->intin[5], pb->intin[6], pb->intin[7],
			pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
			pb->ptsin[4], pb->ptsin[5], pb->ptsin[6], pb->ptsin[7]);

		log("     %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d, %d, %d, %d, %d\n",
			pb->intout[0], pb->intout[1], pb->intout[2], pb->intout[3],
			pb->intout[4], pb->intout[5], pb->intout[6], pb->intout[7],
			pb->ptsout[0], pb->ptsout[1], pb->ptsout[2], pb->ptsout[3],
			pb->ptsout[4], pb->ptsout[5], pb->ptsout[6], pb->ptsout[7]);

		log(" nio %d, npo %d - leave\n\n", pb->contrl[N_INTOUT], pb->contrl[N_PTSOUT]);
	//	log("leave\n\n");
	}

#else
	(*f)(pb, v);
#endif
	return 0L;
}

int
get_cookie(long tag, long *ret)
{
	COOKIE *jar;
	int r = 0;
	long usp;

	usp = Super(1);
	if (!usp)
		usp = Super(0L);
	else
		usp = 0L;

	jar = *CJAR;

	if (!jar)
		return r;

	while(jar->tag)
	{
		if (jar->tag == tag)
		{
			if (ret)
				*ret = jar->value;
			r = 1;
			break;
		}
		jar++;
	}

	if (usp)
		(void)Super(usp);

	return r;
}

void
v_nosys( VDIPB *pb, VIRTUAL *v)
{
	return;
}

