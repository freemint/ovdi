#ifndef _OVDI_DRAW_H
#define _OVDI_DRAW_H

#include "ovdi_defs.h"
#include "vdi_defs.h"

void arb_corner( VDIRECT *corners, short type);
void sortcpy_corners( short *src, short *dst);
short clipbox( VDIRECT *corners, VDIRECT *clip);
void rectfill( VIRTUAL *v, VDIRECT *corners, PatAttr *ptrn);

void draw_arc( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
void draw_pieslice( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
void draw_circle( VIRTUAL *v, short xc, short yc, short xrad, short *points, PatAttr *ptrn);
void draw_ellipse( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short *points, PatAttr *ptrn);
void draw_ellipsearc( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
void draw_ellipsepie( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);

void draw_rbox( VIRTUAL *v, short gdp_code, VDIRECT *corners, PatAttr *ptrn);


short clc_nsteps( short xrad, short yrad);
void clc_arc(VIRTUAL *v, short gdp_code, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, \
			 short del_ang, short n_steps, short *points, PatAttr *ptrn);

#endif	/* _OVDI_DRAW_H */
