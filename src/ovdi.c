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
	scrnlog("OVDI start adr %lx\n", _base->p_tbase);
	log("OVDI start adr %lx\n", _base->p_tbase);
	devinit = &device_init;
	bootdev = Dgetdrv() + 'a';

	old_trap2_vec = (long) Setexc(0x22, New_Trap2);
	return;
}

long
oVDI( VDIPB *pb )
{
	short func, i, logit;
	short *s, *d;
	VDI_function *f;
	VIRTUAL *v;

	func = pb->contrl[FUNCTION];

	if (func == 241)	/* remove me! */
		func = 8;

	if (func > MAX_VDI_FUNCTIONS)
	{
		v = v_vtab[pb->contrl[HANDLE]].v;
		if (v)
			log("Calling old trap 2, func %d: pid %d %s\n", func, v->pid, v->procname);
		else
			log("Calling old trap 2, func %d - ILLEGAL HANDLE\n", func);
		
		return -1L;
	}

	f = (VDI_function *)v_jmptab[func];

	/* Opening a physical is a special case */
	if (func == OPEN_WORKSTATION)
	{
		device = (*devinit)(&ovdilib);

		if (device)
		{
			linea_vars = &la_vt;
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
		short pid = Pgetpid();
		log("oVDI Invalid handle %d, func %d, pid %d\n", pb->contrl[HANDLE], func, pid);
		return 0L;
	}

	v->func = func;

#if 0
	if (!(strcmp("PROFILE2", v->procname)))
		logit = 1;
	else
		logit = 0;
#else
	logit = 0;
#endif

	if (logit)
	{
		log("func %d subfuc %d, %d, %d, %d, %d ..", func, pb->contrl[SUBFUNCTION],
			pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3]);
	}
	if (logit && func == 114)
		return 0L;

	(*f)(pb, v);

	if (logit)
		log("leave\n");


	return 0L;
}

int
get_cookie(long tag, long *ret)
{
	COOKIE *jar;
	int r = 0;

	log("get_cookie: ");

	jar = *CJAR;

	if (!jar)
	{
		log("no jar found\n");
		return r;
	}

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
	log("return %d\n", r);
	return r;
}

void
v_nosys( VDIPB *pb, VIRTUAL *v)
{
	return;
}

