#include "display.h"
#include "line.h"
#include "memory.h"
#include "patattr.h"
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
	int i;

	l->exp_data	= (unsigned short *)&v->linedata.edata;
	l->mask		= (unsigned short *)&v->linedata.mask;

	set_fill_params(FIS_SOLID, 0, l, &l->interior, 0);
	l->t.l.index = -1;
	l->wrmode = -1;
	
	for (i = 0; i < 4; i++)
		l->color[i] = l->bgcol[i] = -1;
}

void
v_set_app_buff(VDIPB *pb, VIRTUAL *v)
{
	void *buff = *(void **)&(pb->intin[0]);
	long size = pb->intin[2] << 4;

	lv_set_app_buff(v, buff, size);
}

void
lv_set_app_buff(VIRTUAL *v, void *buff, long size)
{
	if (v->app_buff)
	{
		if (v->flags & V_OWN_APPBUFF)
		{
			free_mem(v->app_buff);
			v->flags &= ~V_OWN_APPBUFF;
		}
		v->app_buff = 0;
	}	
	if (!buff)
	{
		buff = (void *)omalloc(size, MX_PREFTTRAM | MX_PRIVATE);
		v->flags |= V_OWN_APPBUFF;
	}
	else
		v->flags &= ~V_OWN_APPBUFF;

	v->app_buff = buff;
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
	set_pa_fgcolor(&v->line, v->colinf, color);
}

void
lvsl_bgcolor( VIRTUAL *v, short color)
{
	set_pa_bgcolor(&v->line, v->colinf, color);
}
void
lvsl_wrmode( VIRTUAL *v, short wrmode)
{
	set_pa_writemode(&v->line, wrmode);
}

void
vsl_ends( VDIPB *pb, VIRTUAL *v)
{
	lvsl_ends(v, pb->intin[0], pb->intin[1]);
}
void
lvsl_ends( VIRTUAL *v, short begin, short end)
{
	set_pa_lineends(&v->line, begin, end);
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
	set_pa_lineindex(&v->line, index);
}

void
vsl_udsty( VDIPB *pb, VIRTUAL *v)
{
	lvsl_udsty( v, pb->intin[0]);
}
void
lvsl_udsty( VIRTUAL *v, unsigned short pattern)
{
	set_pa_udline( &v->line, pattern);
#if 0
	v->line.ud = (unsigned short)pattern;
	if (v->line.t.l.index == LI_USER-1)
		v->line.expanded = 0;
#endif
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
	set_pa_linewidth(&v->line, width);
#if 0
	if (width < 1)
		v->line.t.l.width = 1;
	else if (width > MAX_L_WIDTH)
		v->line.t.l.width = MAX_L_WIDTH | 1;
	else
		v->line.t.l.width = width | 1;
#endif
}

void
v_pline( VDIPB *pb, VIRTUAL *v)
{
	RASTER *r = v->raster;
	COLINF *c = v->colinf;
	VDIRECT *clip;
	int count;
	POINT16 *in_pts;
	VDIRECT points;
	Fpline dl;

	count = pb->contrl[N_PTSIN];

	if ((count -= 2) < 0)
		return;

	in_pts = (POINT16 *)&pb->ptsin[0];

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
