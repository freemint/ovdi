#include <mint/mintbind.h>
#include <mint/dcntl.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdio.h>

#include "display.h"
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
#include "colors.h"
#include "draw.h"
#include "gdf_text.h"
#include "file.h"
#include "rasters.h"
#include "console.h"
#include "linea.h"
#include "mouse.h"
#include "mousedrv.h"
#include "mouse_data.c"
#include "v_attribs.h"
#include "v_escape.h"
#include "v_line.h"
#include "v_gdp.h"
#include "v_fill.h"
#include "v_raster.h"
#include "polygon.h"
#include "time.h"
#include "v_input.h"

#endif

void v_nosys( VDIPB *, VIRTUAL *);

long justtest;

short MiNT = 0;

char bootdev;

OVDI_DEVICE * (*devinit)(OVDI_LIB *l);
OVDI_DEVICE *device;

char fontdata[1024 * 10];


#ifdef PRG_TEST
VIRTUAL testv;
short test_contrl[128];
short test_intin[128];
short test_ptsin[1024];
short test_intout[128];
short test_ptsout[1024];

VDIPB test_pb =
{
	test_contrl,
	test_intin,
	test_ptsin,
	test_intout,
	test_ptsout
};

short testbuff[1024*30UL];

#endif

void
ovdi_init(void)
{
	long shit;
	short shit1;
#ifdef PRG_TEST
	int i, j;
	short col;
	short handle;
	VIRTUAL *v = &testv;
	VDIPB *pb = &test_pb;
	struct opnwk_input *wkin;
	POINT *pts;
	VDIRECT *rect;
	unsigned char tsttxt[] = "  abcdefghijkl   mn opqr   stuvwxyz³‘† end\0";
	MFDB src, dst;

#endif
	shit = 0x544e694d; //0xface;
	shit1 = 0xface;

	devinit = &device_init;
	linea_vars = &la_vt;

#ifdef PRG_TEST
	//shit = get_pixel_8b((unsigned char *)0x80000000UL, 100, 0, 0);
	justtest = shit1;

	wkin = (struct opnwk_input *)&pb->intin[0];

	wkin->id = 1;
	wkin->linetype = 0;
	wkin->linecolor = 2;
	wkin->markertype = 1;
	wkin->markercolor = 1;
	wkin->fontid = 1;
	wkin->textcolor = 1;
	wkin->fillinterior = 1;
	wkin->fillstyle = 1;
	wkin->fillcolor = 1;
	wkin->coordflag = 2;


	pb->contrl[FUNCTION] = 1;
	pb->contrl[N_PTSIN] = 0;
	pb->contrl[N_INTIN] = 11;
	pb->contrl[SUBFUNCTION] = 0;
	pb->contrl[HANDLE] = 0;
	
	oVDI(pb);

	//v_opnwk(pb, v, 0, device);

	if (pb->contrl[HANDLE] == 0)
	{
		display("What the hell?!\n");
		return;
	}

/* ***************** start test  ************************ */
	wkin->id = 1;
	wkin->linetype = 0;
	wkin->linecolor = 2;
	wkin->markertype = 1;
	wkin->markercolor = 1;
	wkin->fontid = 1;
	wkin->textcolor = 1;
	wkin->fillinterior = 1;
	wkin->fillstyle = 1;
	wkin->fillcolor = 1;
	wkin->coordflag = 2;

	pb->contrl[FUNCTION] = 100, pb->contrl[N_PTSIN] = 0, pb->contrl[N_INTIN] = 11, pb->contrl[SUBFUNCTION] = 0, pb->contrl[HANDLE] = 1;
	//v_opnvwk(pb, v);
	oVDI(pb);

	handle = pb->contrl[HANDLE];
	display("handle %d\n", handle);

/* =============== Mouse api test ====================== */
#if 0
	{
		MOUSEAPI *ma;

		ma = v->mouseapi;
		for (i = 0; i < 150; i++)
		{
			(*ma->hidemcurs)();
			(*ma->relmovmcurs)(1, 0);
			(*ma->showmcurs)();
		}
	}

//	return;
#endif
#if 0
	pb->contrl[FUNCTION] = 5, pb->contrl[N_PTSIN] = 0, pb->contrl[N_INTIN] = 0;
	pb->contrl[SUBFUNCTION] = 3, pb->contrl[HANDLE] = handle;
	oVDI(pb);

	tsttxt[0] = 10, tsttxt[1] = 13;	
	{
		short chr;
		short len;
		unsigned char *string;
		long loaded;

		string = (unsigned char *)&testbuff;
		loaded = load_file("m:\\w_dialog.txt", 30000, string);
		string[loaded] = 0;

		len = 0;
		while (*string)
		{
			chr = (short)(0xff & *string++);
			if (chr == 13 || chr == 10)
			{
				pb->contrl[FUNCTION] = 5, pb->contrl[N_PTSIN] = 0, pb->contrl[N_INTIN] = len;
				pb->contrl[SUBFUNCTION] = 12, pb->contrl[HANDLE] = handle;
				pb->intin[len+1] = 0;
				oVDI(pb);
				len = 0;
				if (*string == 10)
					string++;

				
			}
			else
			{
				pb->intin[len] = chr;
				len++;
			}
		}
		if (len)
		{
			pb->contrl[FUNCTION] = 5, pb->contrl[N_PTSIN] = 0, pb->contrl[N_INTIN] = len;
			pb->contrl[SUBFUNCTION] = 12, pb->contrl[HANDLE] = handle;
			pb->intin[len+1] = 0;
			oVDI(pb);
		}
	}
#if 0
	for (i = 0; i < 21; i++)
		v_curup(pb, v);

	for (i = 0; i < 42; i++)
		v_curright(pb, v);


	v_eeol(pb, v);
#endif 0

	//exit_console(v->con);
	//return;
#endif
/* --------------- some vro_cpyfm tests --------------- */
#if 1
	src.fd_addr = &mouse_8b;
	dst.fd_addr = &mouse_8b; //&testbuff;
	src.fd_w = dst.fd_w = mouse_w;
	src.fd_h = dst.fd_h = mouse_h;
	src.fd_wdwidth = dst.fd_wdwidth = (mouse_w + 15) >> 4;
	src.fd_stand = 1;
	src.fd_nplanes = dst.fd_nplanes = 8;

	pb->contrl[FUNCTION] = 110;
	pb->contrl[N_PTSIN] = 0;
	pb->contrl[N_INTIN] = 0;
	pb->contrl[SUBFUNCTION] = 0;
	pb->contrl[HANDLE] = handle;

	pb->contrl[7] = ((unsigned long)&src >> 16);
	pb->contrl[8] = ((unsigned long)&src & 0xffff);
	pb->contrl[9] = ((unsigned long)&dst >> 16);
	pb->contrl[10] = ((unsigned long)&dst & 0xffff);

	oVDI(pb);

	pb->intin[0] = 3;

	src = dst;
	dst.fd_addr = 0;

	pb->ptsin[0] = 0, pb->ptsin[1] = 0, pb->ptsin[2] = 32, pb->ptsin[3] = 32;
	pb->ptsin[4] = 0, pb->ptsin[5] = 0, pb->ptsin[6] = 32, pb->ptsin[7] = 32;

	for (i = 0; i < 10; i++)
	{
		
		pb->contrl[FUNCTION] = 109;
		pb->contrl[N_PTSIN] = 4;
		pb->contrl[N_INTIN] = 1;
		pb->contrl[SUBFUNCTION] = 0;
		pb->contrl[HANDLE] = handle;

		dst.fd_addr = 0;
		oVDI(pb); //vro_cpyfm( pb, v);
		pb->ptsin[4] += 10;
		pb->ptsin[5] += 60;
		pb->ptsin[6] += 10;
		pb->ptsin[7] += 60;
	}
	display("end of raster tests\n");
	//return;	
#endif
/* --------------- Polymarker tests --------------- */
#if 0
	pb->intin[0] = 2;
	vsm_color( pb, v);

	pb->ptsin[1] = 200;
	vsm_height( pb, v);

	pb->intin[0] = 3;
	vsm_type( pb, v);

	pb->contrl[FUNCTION]	= 7;
	pb->contrl[N_PTSIN]	= 1;
	pb->contrl[N_INTIN]	= 0;
	pb->contrl[SUBFUNCTION]	= 0;
	pb->contrl[HANDLE]	= 1;

	pb->ptsin[0] = 100, pb->ptsin[1] = 100;
	v_pmarker( pb, v);

	
	return;
#endif
/* ------- Test some text things ---------- */
#if 0
	{
		unsigned char *tbuff;
		short *inbuf;
		
		tbuff = (unsigned char *)&tsttxt;
		inbuf = (short *)&pb->intin[0];
		for (i = 0; i < sizeof(tsttxt); i++)
		{
			*inbuf++ = 0xff & *tbuff++;
		}
		pb->contrl[N_INTIN] = sizeof(tsttxt) - 1;
	}

	for (j = 0; j < 5; j++)
	{
		pb->intin[0] = j;
		vqt_name( pb, v);

		{
			short *tbuff;
			short *outbuff;

			tbuff = (short *)&testbuff;
			outbuff = (short *)&pb->intout[1];
			for (i = 0; i < 34; i += 2)
				*tbuff++ = outbuff[i] << 8 | outbuff[i + 1];

			display(" '%s' \n", &testbuff);
		}

		display(" %d ", pb->intout[0]);
/*
	display(" %d ", pb->intout[1]);
	display(" %d ", pb->intout[2]);
	display(" %d ", pb->intout[3]);
	display(" %d ", pb->intout[4]);
	display(" %d ", pb->intout[5]);
*/

/*
	display(" %d ", pb->ptsout[0]);
	display(" %d ", pb->ptsout[1]);
	display(" %d ", pb->ptsout[2]);
	display(" %d ", pb->ptsout[3]);
	display(" %d ", pb->ptsout[4]);
	display(" %d ", pb->ptsout[5]);
	display(" %d ", pb->ptsout[6]);
	display(" %d ", pb->ptsout[7]);
	display(" %d ", pb->ptsout[8]);
	display(" %d ", pb->ptsout[9]);
	display(" %d ", pb->ptsout[10]);
*/
		display("\n");
	}

#endif

#if 0
	lvswr_mode( v, 1);

	{
		unsigned char *tbuff;
		short *inbuf;
		short coords[4];

		
		//testbuff[0] = 40 /*51*/, testbuff[1] = 80 /*60*/, testbuff[2] = 61, testbuff[3] = 91;
		pb->ptsin[0] = 299 /*51*/, pb->ptsin[1] = 0 /*60*/, pb->ptsin[2] = 999, pb->ptsin[3] = 100+30;
		
		lvs_clip( v, 1, (VDIRECT *)&pb->ptsin[0]); //testbuff);

		tbuff = (unsigned char *)&tsttxt;
		inbuf = (short *)&testbuff;
		for (i = 0; i < sizeof(tsttxt); i++)
		{
			*inbuf++ = 0xff & *tbuff++;
		}

		lvst_color(v, 2);

		lvst_font(v, 103/*103*/);

		lvsf_color(v, 255);

		coords[0] = 600, coords[1] = 90; //pts->x = 40, pts->y = 90;

		for (i = 1; i < 800; i += 8)
		{

			lvst_color(v, i & 255);

			pb->intin[0] = 8809;
			vst_font( pb, v);

			pb->intin[0] = 40;
			vst_point( pb, v);

		/* Draw a black rectangle - fine test of v_bar */
			pb->ptsin[0] = 299 /*51*/, pb->ptsin[1] = 70 /*60*/, pb->ptsin[2] = 999, pb->ptsin[3] = 120;
			pb->contrl[N_PTSIN] = 4;
			pb->contrl[SUBFUNCTION] = 1;
			v_gdp( pb, v);

		/* Test v_justified */
			inbuf = (short *)&pb->intin[2], tbuff = (unsigned char *)&tsttxt;
			while((*inbuf++ = *tbuff++)){}
			pb->intin[0] = 1, pb->intin[1] = 1;
			pb->ptsin[0] = coords[0], pb->ptsin[1] = coords[1];
			pb->ptsin[2] = i, pb->ptsin[3] = 0;
			pb->contrl[N_PTSIN] = 2;
			pb->contrl[N_INTIN] = sizeof(tsttxt) + 2;
			pb->contrl[SUBFUNCTION] = 10;
			v_gdp( pb, v);
			
			pb->intin[0] = 31;
			vst_font( pb, v);

			pb->intin[0] = 20;
			vst_point( pb, v);

			lvst_color(v, (i+32) & 255);
		/* Test v_gtext */
			inbuf = (short *)&pb->intin[0], tbuff = (unsigned char *)&tsttxt;
			while((*inbuf++ = *tbuff++)){}
			pb->ptsin[0] = coords[0], pb->ptsin[1] = coords[1] + 20;
			pb->contrl[N_PTSIN] = 1;
			pb->contrl[N_INTIN] = sizeof(tsttxt);
			v_gtext( pb, v);
			

			pb->intin[0] = 103;
			vst_font( pb, v);

			pb->intin[0] = 20;
			vst_point( pb, v);

			lvst_color(v, (i+64) & 255);
		/* Test v_justified */
			inbuf = (short *)&pb->intin[2], tbuff = (unsigned char *)&tsttxt;
			while((*inbuf++ = *tbuff++)){}
			pb->intin[0] = 1, pb->intin[1] = 1;
			pb->ptsin[0] = coords[0], pb->ptsin[1] = coords[1] + 46;
			pb->ptsin[2] = i << 1, pb->ptsin[3] = 0;
			pb->contrl[N_PTSIN] = 2;
			pb->contrl[N_INTIN] = sizeof(tsttxt) + 2;
			pb->contrl[SUBFUNCTION] = 10;
			v_gdp( pb, v);

			coords[0] -= 4;
		}
	}

	return;
#endif 
/* ----------------------- end test text things ----------------- */
/* ---------------- test gdp functions ------------------------- */
#if 0
	testbuff[0] = 10, testbuff[1] = 40, testbuff[2] = 1200, testbuff[3] = 300;
	lvs_clip( v, 0, (VDIRECT *)testbuff);

	pb->contrl[SUBFUNCTION] = 1;
	lvsf_color( v, 0);
	pb->ptsin[0] = 0, pb->ptsin[1] = 0, pb->ptsin[2] = v->clip.x2, pb->ptsin[3] = 600;
	//v_gdp( pb, v);

	lvsl_color( v, 2);
	lvsf_color( v, 2);
	set_fill_params( v, FIS_PATTERN, 19);

	pb->contrl[SUBFUNCTION] = 1;
	pb->ptsin[0] = 100, pb->ptsin[1] = 200, pb->ptsin[2] = 200, pb->ptsin[3] = 400;
	v_gdp( pb, v);

	pb->contrl[SUBFUNCTION] = 2;
	pb->ptsin[0] = 300, pb->ptsin[1] = 100, pb->ptsin[6] = 50;
	pb->intin[0] = 1000, pb->intin[1] = 0;
	v_gdp( pb, v); //v_arc( pb, v);

	pb->contrl[SUBFUNCTION] = 3;
	pb->ptsin[0] = 400;
	v_gdp( pb, v); //v_pieslice( pb, v);

	pb->contrl[SUBFUNCTION] = 4;
	pb->ptsin[0] = 500, pb->ptsin[4] = 50;
	v_gdp( pb, v); //v_circle( pb, v);

	pb->contrl[SUBFUNCTION] = 5;
	pb->ptsin[0] = 600, pb->ptsin[2] = 50, pb->ptsin[3] = 100;
	v_gdp( pb, v); //v_ellipse( pb, v);

	pb->contrl[SUBFUNCTION] = 6;
	pb->ptsin[0] = 700;
	v_gdp( pb, v); //v_ellarc( pb, v);

	pb->contrl[SUBFUNCTION] = 7;
	pb->ptsin[0] = 800;
	v_gdp( pb, v); //v_ellpie( pb, v);

	pb->contrl[SUBFUNCTION] = 8;
	pb->ptsin[0] = 900, pb->ptsin[1] = 200, pb->ptsin[2] = 1100, pb->ptsin[3] = 400;
	v_gdp( pb, v); //v_rbox( pb, v);

	pb->contrl[SUBFUNCTION] = 9;
	pb->ptsin[1] += 250, pb->ptsin[3] += 250;
	v_gdp( pb, v); //v_rfbox( pb, v);

	return;
#endif
/* ----------------------- end test GDP functin --------------------- */
/* ****************************************************************************** */
#if 0
	pts = (POINT *)&pb->ptsin[0];
	rect = (VDIRECT *)pts;

	rect->x1 = 100;
	rect->y1 = 200;
	rect->x2 = 1000;
	rect->y2 = 100;

	lvsl_color( v, 6);
	lvsl_width( v, 10);
	lvsl_ends( v, LE_ARROW, LE_ARROW);
	lvsf_perimeter( v, 1);
	v_bar ( pb, v ); //rectfill( v, rect, &v->pattern);
	//return;

	rect++;
	rect->x1 = 500;
	rect->y1 = 500;
	rect->x2 = 100;
	rect->y2 = 200;

	set_fill_params(v, FIS_PATTERN, 19);
	
	v_bar ( pb, v ); //rectfill( v, rect, &v->pattern);

	//return;

	pb->contrl[N_PTSIN] = 4;

	lvswr_mode( v, MD_TRANS);

	pb->contrl[N_PTSIN] = 4;

	v_fillarea( pb, v);
	set_fill_params(v, FIS_PATTERN, 16);
	v_fillarea( pb, v);
	set_fill_params(v, FIS_PATTERN, 21);
	v_fillarea( pb, v);
	set_fill_params(v, FIS_HATCH, 9);
	v_fillarea( pb, v);
	set_fill_params(v, FIS_PATTERN, 19);

	lvsl_width(v, 7);
	lvsl_ends(v, LE_ARROW, LE_ARROW);

	v_pline( pb, v);
//	return;

//	lvsl_width(v, 70);
	lvsl_color( v, 3);
//	v_pline( pb, v);

	col = 0;
	for (i = 0; i < 100; i++)
	{
		lvsl_color( v, col);
		col++;
		if (col > 255)
			col = 0;

		rect--;
		rect->x1 += 20;
		rect->y1 += 2;
		rect->x2 -= 40;
		rect->y2 += 30;
		rect++;
		rect->x1 += 2;
		rect->y1 -= 1;
		rect->x2 += 2;
		rect->y2 += 2;

		v_fillarea( pb, v);
		v_pline( pb, v);
	}

	return;
#endif
/* ****************************************************************************** */
/* *********************      end test  ************************** */
/* ****************************************************************************** */

#endif

	devinit = &device_init;
	bootdev = Dgetdrv() + 'a';

#ifndef PRG_TEST
	old_trap2_vec = (long) Setexc(0x22, New_Trap2);
#endif

	justtest = shit;
	return;
}

long
oVDI( VDIPB *pb )
{
	short func;
	VDI_function *f;
	VIRTUAL *v;

	func = pb->contrl[FUNCTION];

	if (func > MAX_VDI_FUNCTIONS)
		return -1;

	f = (VDI_function *)v_jmptab[func];

	/* Opening a physical is a special case */
	if (func == OPEN_WORKSTATION)
	{
		device = (*devinit)(&ovdilib);

		if (device)
		{
			linea_vars = &la_vt;
			(*f)(pb, &wks1, &la_wks, device);
			return 0;
		}
		else
		{
			pb->contrl[HANDLE] = 0;
			return 0;
		}
	}

	/* We pass the owning physical workstation's VIRTUAL to
	 * v_opnvwk
	*/
	if (func == OPEN_VWORKSTATION)
	{
		(*f)(pb, &wks1);
		return 0;
	}

	/* No special cases, get correct VIRTUAL and
	 *just call the function
	*/

	pb->contrl[N_PTSOUT] = 0;
	pb->contrl[N_INTOUT] = 0;

	v = v_vtab[pb->contrl[HANDLE]].v;
	if (!v)
		return 0;

	(*f)(pb, v);

	return 0;
}

int
get_cookie(long tag, long *ret)
{
	COOKIE *jar;
	long sstack;
	int r = 0;

	(long)sstack = Super(0L);
	jar = *CJAR;
	Super((long)sstack);

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

