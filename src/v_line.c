#include "display.h"
#include "line.h"
#include "vdi_globals.h"
#include "v_attribs.h"
#include "v_line.h"

//short ptsbuff[200*5*2];

void
vsl_color(VDIPB *pb, VIRTUAL *v)
{
	lvsl_color(v, pb->intin[0]);
	pb->intout[0] = v->color_hw2vdi[v->line.color];

	pb->contrl[N_INTOUT] = 1;
	return;
}
void
lvsl_color( VIRTUAL *v, short color)
{
	register short maxcolor;

	maxcolor = Planes2Pens[v->raster->planes];

	color = color < maxcolor ? color : maxcolor - 1;
	color = v->color_vdi2hw[color];
	v->line.color = color;
	v->line.bgcol = color;
	v->linedat.color[0] = v->linedat.color[1] = color; //v->linedat.color[2] = v->linedat.color[3] = color;
	v->linedat.color[2] = v->linedat.color[3] = 0xff;
	v->linedat.bgcol = color;
	return;
}

void
lvsl_wrmode( VIRTUAL *v, short wrmode)
{
	set_writingmode( wrmode, &v->linedat.wrmode);
	return;
}

void
vsl_ends( VDIPB *pb, VIRTUAL *v)
{
	lvsl_ends(v, pb->intin[0], pb->intin[1]);
	return;
}
void
lvsl_ends( VIRTUAL *v, short begin, short end)
{
	v->line.beg = begin <= MAX_LN_ENDS ? begin : MAX_LN_ENDS;
	v->line.end = end <= MAX_LN_ENDS ? end : MAX_LN_ENDS;
	return;
}

void
vsl_type( VDIPB *pb, VIRTUAL *v)
{
	lvsl_type( v, pb->intin[0]);
	pb->intout[0] = v->line.index + 1;

	pb->contrl[N_INTOUT] = 1;
	return;
}

void
lvsl_type( VIRTUAL *v, short index)
{

	if (index < 1)
		index = 1;
	else if (index > MAX_LN_STYLE)
		index = 1;

	if (index == LI_USER)
		v->line.data = v->line.ud;
	else
		v->line.data = LINE_STYLE[index - 1];

	v->line.index = index - 1;
	v->linedat.expanded = 0;
	v->linedat.width = 16;
	v->linedat.height = 1;
	v->linedat.wwidth = 1;
	v->linedat.planes = 1;
	v->linedat.data = &v->line.data; //&LINE_STYLE[ind]; //&v->line.data;

	return;
}

void
vsl_udsty( VDIPB *pb, VIRTUAL *v)
{
	lvsl_udsty( v, pb->intin[0]);
	return;
}
void
lvsl_udsty( VIRTUAL *v, short pattern)
{
	v->line.ud = (unsigned short)pattern;
	return;
}

void
vsl_width( VDIPB *pb, VIRTUAL *v)
{
	lvsl_width( v, pb->ptsin[0]);
	pb->ptsout[0] = v->line.width;

	pb->contrl[N_PTSOUT] = 1;
	return;
}
void
lvsl_width( VIRTUAL *v, short width)
{
	if (width < 1)
		v->line.width = 1;
	else if (width > MAX_L_WIDTH)
		v->line.width = MAX_L_WIDTH | 1;
	else
		v->line.width = width | 1;
	return;
}

//short ptsbuff[100*10*2];

void
v_pline( VDIPB *pb, VIRTUAL *v)
{
	short wide, count;
	POINT *in_pts;
	VDIRECT points;
	//short ptsbuff[10*5*2];

	count = pb->contrl[N_PTSIN];

	if ((count -= 2) < 0)
		return;

	in_pts = (POINT *)&pb->ptsin[0];
	wide = v->line.width == 1 ? 0 : 1;

	while (count >= 0)
	{
		points.x1 = in_pts->x;
		points.y1 = in_pts->y;
		in_pts++;
		points.x2 = in_pts->x;
		points.y2 = in_pts->y;

		if (wide)
			wide_line(v, (short *)&points, 2, (short *)&v->spanbuff, v->spanbuffsiz, &v->linedat); //(short *)&ptsbuff, sizeof(ptsbuff), &v->linedat );
		else
			pline(v, (short *)&points, 2, (short *)&v->spanbuff, v->spanbuffsiz, &v->linedat); //(short *)&ptsbuff, sizeof(ptsbuff), &v->linedat);

		count--;
	}
	return;
}

void
vql_attributes( VDIPB *pb, VIRTUAL *v)
{

	pb->intout[0] = v->line.index + 1;
	pb->intout[1] = v->color_hw2vdi[v->line.color];
	pb->intout[2] = v->linedat.wrmode + 1;
	pb->intout[3] = v->line.beg;
	pb->intout[4] = v->line.end;
	pb->ptsout[0] = v->line.width;
	pb->ptsout[1] = 0;

	pb->contrl[N_INTOUT] = 5;
	pb->contrl[N_PTSOUT] = 1;
	return;

}
