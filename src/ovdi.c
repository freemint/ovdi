#include <osbind.h>
#include <mint/mintbind.h>
#include <mint/basepage.h>
#include <mint/dcntl.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdio.h>

#include "display.h"

#include "draw.h"
#include "line.h"
#include "rasters.h"
#include "polygon.h"

#include "ovdi.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "workstation.h"
#include "funcdef.h"
#include "libkern.h"

#include "std_driver.h"

/* #include "styles.h" */
#include "tables.h"	/* Contains the function jumptable, dev/siz/inq tabs amogst other things */

#include "ovdi_dev.h"
#include "line.h"

#ifdef PRG_TEST
#endif

void v_nosys( VDIPB *, VIRTUAL *);

long justtest;
short MiNT = 0;
char bootdev;

OVDI_DEVICE * (*devinit)(OVDI_LIB *l);
OVDI_DEVICE *device;
//char fontdata[1024 * 10];
extern BASEPAGE *_base;

OVDI_DRAWERS drawers = 
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
};

OVDI_UTILS utils =
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

void
ovdi_init(void)
{
	long lavt, lafr, laft;

	scrnlog("OVDI start adr %lx\n", _base->p_tbase);
	//log("OVDI start adr %lx\n", _base->p_tbase);

	bzero(&wks1, sizeof(VIRTUAL));
	bzero(&la_vt, sizeof(LINEA_VARTAB));

	get_linea_addresses(&lavt, &lafr, &laft);
	lavt -= 910;
	linea_vars = (LINEA_VARTAB *)lavt;

	scrnlog("Linea vartab %lx, font ring %lx, func tab %lx", linea_vars, lafr, laft);

	devinit = &device_init;
	bootdev = Dgetdrv() + 'a';

	old_trap2_vec = (long) Setexc(0x22, New_Trap2);
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
		device = (*devinit)(&ovdilib);

		if (device)
		{
			//linea_vars = &la_vt;

			//(*f)(pb, &wks1, &la_wks, device);
			v_opnwk(pb, &wks1, &la_wks, device);
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
	if ( (Kbshift(-1) & 0x1) )
		logit = 1;
	else
		logit = 0;

	if (logit)
	{
		//log("func %d..", func);

		//log("func %d subfuc %d, %d, %d, %d, %d ..", func, pb->contrl[SUBFUNCTION],
		//	pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3]);

		log("%s (%d) - func %d, subfuc %d, ni %d, np %d\n",
			v->procname, v->pid, func, pb->contrl[SUBFUNCTION], pb->contrl[N_INTIN], pb->contrl[N_PTSIN]);
		log("  in %d, %d, %d, %d %d, %d, %d, %d - %d, %d, %d, %d\n",
			pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3],
			pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
			pb->ptsin[4], pb->ptsin[5], pb->ptsin[6], pb->ptsin[7]);

		log("     %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d\n",
			pb->intout[0], pb->intout[1], pb->intout[2], pb->intout[3],
			pb->intout[4], pb->intout[5], pb->intout[6], pb->intout[7],
			pb->ptsout[0], pb->ptsout[1], pb->ptsout[2], pb->ptsout[3]);
	}

	(*f)(pb, v);

	if (logit)
	{
		log(" out %d, %d, %d, %d %d, %d, %d, %d - %d, %d, %d, %d\n",
			pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3],
			pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
			pb->ptsin[4], pb->ptsin[5], pb->ptsin[6], pb->ptsin[7]);

		log("     %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d\n",
			pb->intout[0], pb->intout[1], pb->intout[2], pb->intout[3],
			pb->intout[4], pb->intout[5], pb->intout[6], pb->intout[7],
			pb->ptsout[0], pb->ptsout[1], pb->ptsout[2], pb->ptsout[3]);

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
	return r;
}

void
v_nosys( VDIPB *pb, VIRTUAL *v)
{
	return;
}

