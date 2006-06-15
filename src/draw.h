#ifndef _OVDI_DRAW_H
#define _OVDI_DRAW_H

#include "ovdi_types.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

void _cdecl sortcpy_corners	( short *src, short *dst);
short clipbox		( VDIRECT *corners, VDIRECT *clip);
void _cdecl rectfill		( RASTER *r, COLINF *c, VDIRECT *corners, VDIRECT *clip, PatAttr *ptrn);

void _cdecl draw_arc		( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
void _cdecl draw_pieslice	( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
void _cdecl draw_circle	( VIRTUAL *v, short xc, short yc, short xrad, short *points, PatAttr *ptrn);
void _cdecl draw_ellipse	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short *points, PatAttr *ptrn);
void _cdecl draw_ellipsearc	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
void _cdecl draw_ellipsepie	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);

void _cdecl draw_rbox		( VIRTUAL *v, short gdp_code, VDIRECT *corners, PatAttr *ptrn);

void _cdecl brescircle		( RASTER *, COLINF *, short, short, short, VDIRECT *, short *, long, PatAttr *);

short _cdecl clc_nsteps	( short xrad, short yrad);
void _cdecl  clc_arc	(VIRTUAL *v, short gdp_code, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, \
			 short del_ang, short n_steps, short *points, PatAttr *ptrn);

#endif	/* _OVDI_DRAW_H */
