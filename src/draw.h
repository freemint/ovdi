#ifndef _OVDI_DRAW_H
#define _OVDI_DRAW_H

#include "ovdi_types.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

void sortcpy_corners	( O_16 *src, O_Pos *dst);
O_Int clipbox		( VDIRECT *corners, VDIRECT *clip);
void rectfill		( RASTER *r, COLINF *c, VDIRECT *corners, VDIRECT *clip, PatAttr *ptrn);

void draw_arc		( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn);
void draw_pieslice	( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn);
void draw_circle	( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos *points, PatAttr *ptrn);
void draw_ellipse	( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Pos *points, PatAttr *ptrn);
void draw_ellipsearc	( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn);
void draw_ellipsepie	( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn);

void draw_rbox		( VIRTUAL *v, O_Int gdp_code, VDIRECT *corners, PatAttr *ptrn);

void brescircle		( RASTER *, COLINF *, O_Pos, O_Pos, O_Pos, VDIRECT *, O_Pos *, long, PatAttr *);

O_Int clc_nsteps	( O_Pos xrad, O_Pos yrad);
void clc_arc		(VIRTUAL *v, O_Int gdp_code, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Int beg_ang, O_Int end_ang, \
			 O_Int del_ang, O_Int n_steps, O_Pos *points, PatAttr *ptrn);

#endif	/* _OVDI_DRAW_H */
