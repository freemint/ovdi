#include "display.h"
#include "line.h"
#include "patattr.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "v_pmarker.h"
#include "v_fill.h"

void
lvsm_initial(VIRTUAL *v)
{
	PatAttr *pm = &v->pmarker;
	int i;

	pm->exp_data	= (unsigned short *)&v->pmarkerdata.edata;
	pm->mask	= (unsigned short *)&v->pmarkerdata.mask;

	set_fill_params(FIS_SOLID, 0, pm, &pm->interior, 0);
	pm->t.p.type = -1;
	pm->wrmode = -1;

	for (i = 0; i < 4; i++)
		pm->color[i] = pm->bgcol[i] = -1;
}
/*
*  New call - Select linetype by which to draw polymarkers
*  This also initializes the PatAttr structure used to draw
*  polymarkers
*/
void
lvsm_linetype( VIRTUAL *v, short linetype )
{
	set_pa_pmrk_lineindex(&v->pmarker, linetype);
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
	set_pa_fgcolor(&v->pmarker, v->colinf, color);
}
void
lvsm_bgcolor( VIRTUAL *v, short color)
{
	set_pa_bgcolor(&v->pmarker, v->colinf, color);
}


void
lvsm_height( VIRTUAL *v, short height)
{
	set_pa_pmrk_size(&v->pmarker, 0, height);
}

void
lvsm_type( VIRTUAL *v, short type)
{
	set_pa_pmrk_type(&v->pmarker, type);
}
void
lvsm_wrmode( VIRTUAL *v, short wrmode)
{
	set_pa_writemode(&v->pmarker, wrmode);
}
void
v_pmarker( VDIPB *pb, VIRTUAL *v)
{
	RASTER *r;
	VDIRECT *clip;
	short count, type, height, width;
	short *inpts;
	POINT pts;
	Fpmarker f;

	count = pb->contrl[N_PTSIN] - 1;

	if (count < 0)
		return;
	
	r	= v->raster;
	f	= DRAW_PMARKER_PTR(r);
	clip	= v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;

	inpts	= (short *)&pb->ptsin[0];
	width	= v->pmarker.t.p.width;
	height	= v->pmarker.t.p.height;
	type	= v->pmarker.t.p.type;

	while (count >= 0)
	{
		pts.x = *inpts++;
		pts.y = *inpts++;
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
