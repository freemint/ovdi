#ifndef _OVDI_DRAW_H
#define _OVDI_DRAW_H

#include "ovdi_types.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

void sortcpy_corners	( short *src, short *dst);
short clipbox		( VDIRECT *corners, VDIRECT *clip);
void rectfill		( RASTER *r, COLINF *c, VDIRECT *corners, VDIRECT *clip, PatAttr *ptrn);

void draw_arc		( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
void draw_pieslice	( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
void draw_circle	( VIRTUAL *v, short xc, short yc, short xrad, short *points, PatAttr *ptrn);
void draw_ellipse	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short *points, PatAttr *ptrn);
void draw_ellipsearc	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
void draw_ellipsepie	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);

void draw_rbox		( VIRTUAL *v, short gdp_code, VDIRECT *corners, PatAttr *ptrn);

void brescircle		( RASTER *, COLINF *, short, short, short, VDIRECT *, short *, long, PatAttr *);

short clc_nsteps	( short xrad, short yrad);
void clc_arc		(VIRTUAL *v, short gdp_code, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, \
			 short del_ang, short n_steps, short *points, PatAttr *ptrn);

#endif	/* _OVDI_DRAW_H */
