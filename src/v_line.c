#include "display.h"
#include "line.h"
#include "vdi_globals.h"
#include "v_attribs.h"
#include "v_line.h"
#include "v_fill.h"

//short ptsbuff[200*5*2];

/*
 * Setup initial values for lines
*/
void
lvsl_initial(VIRTUAL *v)
{
	PatAttr *l = &v->line;
	set_fill_params(FIS_SOLID, 0, l, &l->interior, 0);
}

void
vsl_color(VDIPB *pb, VIRTUAL *v)
{
	lvsl_color(v, pb->intin[0]);
	pb->intout[0] = v->colinf->color_hw2vdi[v->line.color[0]];

	pb->contrl[N_INTOUT] = 1;
}
void
lvsl_color( VIRTUAL *v, short color)
{
	register short maxcolor, planes;

	planes = v->raster->planes;
	maxcolor = v->colinf->pens;

	if	(color < 0)
		color = 0;
	else if	(color >= maxcolor)
		color = maxcolor - 1;

	color = v->colinf->color_vdi2hw[color];

	v->line.color[0] = v->line.color[1] = color;
	v->line.color[2] = v->line.color[3] = planes > 8 ? 0x0 : 0xff;
}

void
lvsl_bgcolor( VIRTUAL *v, short color)
{
	register short maxcolor, planes;

	planes = v->raster->planes;
	maxcolor = v->colinf->pens;

	if	(color < 0)
		color = 0;
	else if	(color >= maxcolor)
		color = maxcolor -1;

	color = v->colinf->color_vdi2hw[color];

	v->line.bgcol[0] = v->line.bgcol[1] = color;
	v->line.bgcol[2] = v->line.bgcol[3] = planes > 8 ? 0xff : 0x0;
}
void
lvsl_wrmode( VIRTUAL *v, short wrmode)
{
	set_writingmode( wrmode, &v->line.wrmode);
}

void
vsl_ends( VDIPB *pb, VIRTUAL *v)
{
	lvsl_ends(v, pb->intin[0], pb->intin[1]);
}
void
lvsl_ends( VIRTUAL *v, short begin, short end)
{
	v->line.t.l.beg = begin <= MAX_LN_ENDS ? begin : MAX_LN_ENDS;
	v->line.t.l.end = end <= MAX_LN_ENDS ? end : MAX_LN_ENDS;
}

void
vsl_type( VDIPB *pb, VIRTUAL *v)
{
	lvsl_type( v, pb->intin[0]);
	pb->intout[0] = v->line.t.l.index + 1;

	pb->contrl[N_INTOUT] = 1;
}

void
lvsl_type( VIRTUAL *v, short index)
{

	if (index < 1)
		index = 1;
	else if (index > MAX_LN_STYLE)
		index = 1;

	if (index == LI_USER)
		v->line.data = &v->line.ud;
	else
	{
		v->line.data = (short *)&LINE_STYLE[index - 1];
	}

	v->line.t.l.index = index - 1;
	v->line.expanded = 0;
	v->line.width = 16;
	v->line.height = 1;
	v->line.wwidth = 1;
	v->line.planes = 1;
}

void
vsl_udsty( VDIPB *pb, VIRTUAL *v)
{
	lvsl_udsty( v, pb->intin[0]);
}
void
lvsl_udsty( VIRTUAL *v, short pattern)
{
	v->line.ud = (unsigned short)pattern;
}

void
vsl_width( VDIPB *pb, VIRTUAL *v)
{
	lvsl_width( v, pb->ptsin[0]);
	pb->ptsout[0] = v->line.t.l.width;

	pb->contrl[N_PTSOUT] = 1;
}

void
lvsl_width( VIRTUAL *v, short width)
{
	if (width < 1)
		v->line.t.l.width = 1;
	else if (width > MAX_L_WIDTH)
		v->line.t.l.width = MAX_L_WIDTH | 1;
	else
		v->line.t.l.width = width | 1;
}

void
v_pline( VDIPB *pb, VIRTUAL *v)
{
	RASTER *r = v->raster;
	COLINF *c = v->colinf;
	VDIRECT *clip;
	short count;
	POINT *in_pts;
	VDIRECT points;
	Fpline dl;

	count = pb->contrl[N_PTSIN];

	if ((count -= 2) < 0)
		return;

	in_pts = (POINT *)&pb->ptsin[0];
	dl = v->line.t.l.width == 1 ? DRAW_PLINE_PTR(r) : DRAW_WIDELINE_PTR(r);

	clip = v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;

	while (count >= 0)
	{
		points.x1 = in_pts->x;
		points.y1 = in_pts->y;
		in_pts++;
		points.x2 = in_pts->x;
		points.y2 = in_pts->y;

		(*dl)(r, c, (short *)&points, 2, clip, (short *)&v->spanbuff, v->spanbuffsiz, &v->line);
		count--;
	}
}

void
vql_attributes( VDIPB *pb, VIRTUAL *v)
{

	pb->intout[0] = v->line.t.l.index + 1;
	pb->intout[1] = v->colinf->color_hw2vdi[v->line.color[0]];
	pb->intout[2] = v->line.wrmode + 1;
	pb->intout[3] = v->line.t.l.beg;
	pb->intout[4] = v->line.t.l.end;
	pb->ptsout[0] = v->line.t.l.width;
	pb->ptsout[1] = 0;

	pb->contrl[N_INTOUT] = 5;
	pb->contrl[N_PTSOUT] = 1;
	return;

}
