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
		v->pmarker.data = v->line.ud;
	else
		v->pmarker.data = LINE_STYLE[linetype - 1];

	v->pmrkdat.expanded = 0;
	v->pmrkdat.width = 16;
	v->pmrkdat.height = 1;
	v->pmrkdat.wwidth = 1;
	v->pmrkdat.planes = 1;
	v->pmrkdat.data = &v->pmarker.data;

	return;
}

void
vsm_color( VDIPB *pb, VIRTUAL *v)
{
	lvsm_color( v, pb->intin[0]);
	pb->intout[0] = v->color_hw2vdi[v->pmarker.color];

	pb->contrl[N_INTOUT] = 1;
	return;
}

void
vsm_height( VDIPB *pb, VIRTUAL *v)
{
	lvsm_height( v, pb->ptsin[1]);
	pb->ptsout[0] = v->pmarker.width;
	pb->ptsout[1] = v->pmarker.height;

	pb->contrl[N_PTSOUT] = 1;
	return;
}

void
vsm_type( VDIPB *pb, VIRTUAL *v)
{
	lvsm_type (v, pb->intin[0]);
	pb->intout[0] = v->pmarker.type;

	pb->contrl[N_INTOUT] = 1;
	return;
}

void
lvsm_color( VIRTUAL *v, register short color)
{
	register short maxcolor;

	maxcolor = Planes2Pens[v->raster->planes];
	color = color < maxcolor ? color : maxcolor - 1;
	v->pmarker.color = v->color_vdi2hw[color];
	v->pmrkdat.color[0] = v->pmrkdat.color[1] = v->pmarker.color;
	v->pmrkdat.color[2] = v->pmrkdat.color[3] = 0xff;
	return;
}


void
lvsm_height( VIRTUAL *v, register short height)
{
	v->pmarker.height = height;
	v->pmarker.width = height;
	return;
}

void
lvsm_type( VIRTUAL *v, register short type)
{

	if (type < MIN_PMARKERTYPE)
		type = MIN_PMARKERTYPE;
	else if (type > MAX_PMARKERTYPE)
		type = MAX_PMARKERTYPE;

	v->pmarker.type = type - 1;
	return;
}

void
v_pmarker( VDIPB *pb, VIRTUAL *v)
{
	register short count, type, height, width;
	POINT *inpts;
	POINT pts;

	if ((count = pb->contrl[N_PTSIN]) < 1)
		return;
	
	inpts = (POINT *)&pb->ptsin[0];
	width = v->pmarker.width;
	height = v->pmarker.height;
	type = v->pmarker.type;

	while (count)
	{
		pts = *inpts++;
		pmarker( v, &pts, type, 0, width, height, &v->pmrkdat);
		count--;
	}
	return;
}

void
vqm_attributes( VDIPB *pb, VIRTUAL *v)
{
	pb->intout[0] = v->pmarker.type;
	pb->intout[1] = v->color_hw2vdi[v->pmarker.color];
	pb->intout[2] = v->wrmode +1;

	pb->ptsout[0] = v->pmarker.width;
	pb->ptsout[1] = v->pmarker.height;

	pb->contrl[N_INTOUT] = 3;
	pb->contrl[N_PTSOUT] = 1;
	return;
}
