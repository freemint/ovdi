#include "display.h"
#include "line.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "v_pmarker.h"

/*
*  New call - Select linetype by which to draw polymarkers
*  This also initializes the PatAttr structure used to draw
*  polymarkers
*/
void
lvsm_linetype( VIRTUAL *v, register short linetype )
{
	if (linetype < 1)
		linetype = 1;
	else if (linetype > MAX_LN_STYLE)
		linetype = MAX_LN_STYLE;

	if (linetype == LI_USER)
		v->pmarker.data = &v->line.ud;
	else
		v->pmarker.data = (unsigned short *)&LINE_STYLE[linetype - 1];

	v->pmarker.t.p.index = linetype - 1;

	v->pmarker.expanded = 0;
	v->pmarker.width = 16;
	v->pmarker.height = 1;
	v->pmarker.wwidth = 1;
	v->pmarker.planes = 1;
	v->pmarker.data = &v->pmarker.data;
}

void
vsm_color( VDIPB *pb, VIRTUAL *v)
{
	lvsm_color( v, pb->intin[0]);
	pb->intout[0] = v->colinf->color_hw2vdi[v->pmarker.color[0]];

	pb->contrl[N_INTOUT] = 1;
}

void
vsm_height( VDIPB *pb, VIRTUAL *v)
{
	lvsm_height( v, pb->ptsin[1]);
	pb->ptsout[0] = v->pmarker.t.p.width;
	pb->ptsout[1] = v->pmarker.t.p.height;

	pb->contrl[N_PTSOUT] = 1;
}

void
vsm_type( VDIPB *pb, VIRTUAL *v)
{
	lvsm_type (v, pb->intin[0]);
	pb->intout[0] = v->pmarker.t.p.type;

	pb->contrl[N_INTOUT] = 1;
}

void
lvsm_color( VIRTUAL *v, short color)
{
	register short maxcolor, planes;

	planes = v->raster->planes;
	maxcolor = v->colinf->pens;

	if (color < 0)
		color = 0;
	else if (color >= maxcolor)
		color = maxcolor - 1;

	color = v->colinf->color_vdi2hw[color];

	v->pmarker.color[0] = v->pmarker.color[1] = color;
	v->pmarker.color[2] = v->pmarker.color[3] = planes > 8 ? 0x0 : 0xff;
}
void
lvsm_bgcolor( VIRTUAL *v, short color)
{
	register short maxcolor, planes;

	planes = v->raster->planes;
	maxcolor = v->colinf->pens;

	if (color < 0)
		color = 0;
	else if (color >= maxcolor)
		color = maxcolor - 1;

	color = v->colinf->color_vdi2hw[color];

	v->pmarker.bgcol[0] = v->pmarker.bgcol[1] = color;
	v->pmarker.bgcol[2] = v->pmarker.bgcol[3] = planes > 8 ? 0x0 : 0xff;
}


void
lvsm_height( VIRTUAL *v, short height)
{
	v->pmarker.t.p.height = height;
	v->pmarker.t.p.width = height;
}

void
lvsm_type( VIRTUAL *v, short type)
{

	if (type < MIN_PMARKERTYPE)
		type = MIN_PMARKERTYPE;
	else if (type > MAX_PMARKERTYPE)
		type = MAX_PMARKERTYPE;

	v->pmarker.t.p.type = type - 1;
}

void
v_pmarker( VDIPB *pb, VIRTUAL *v)
{
	RASTER *r;
	VDIRECT *clip;
	register short count, type, height, width;
	POINT *inpts;
	POINT pts;
	Fpmarker f;

	count = pb->contrl[N_PTSIN] - 1;

	if (count < 0)
		return;
	
	r	= v->raster;
	f	= DRAW_PMARKER_PTR(r);
	clip	= v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;

	inpts	= (POINT *)&pb->ptsin[0];
	width	= v->pmarker.t.p.width;
	height	= v->pmarker.t.p.height;
	type	= v->pmarker.t.p.type;

	while (count >= 0)
	{
		pts = *inpts++;
		(*f)( r, v->colinf, &pts, clip, type, 0, width, height, &v->pmarker);
		count--;
	}
}

void
vqm_attributes( VDIPB *pb, VIRTUAL *v)
{
	pb->intout[0] = v->pmarker.t.p.type + 1;
	pb->intout[1] = v->colinf->color_hw2vdi[v->pmarker.color[0]];
	pb->intout[2] = v->pmarker.wrmode +1;

	pb->ptsout[0] = v->pmarker.t.p.width;
	pb->ptsout[1] = v->pmarker.t.p.height;

	pb->contrl[N_INTOUT] = 3;
	pb->contrl[N_PTSOUT] = 1;
}
