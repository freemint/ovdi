#include "display.h"
#include "draw.h"
#include "gdf_text.h"
#include "libkern.h"
#include "line.h"
#include "ovdi_defs.h"
#include "ovdi_types.h"
#include "vdi_defs.h"
#include "v_gdp.h"

#define MIN_GDP		1
#define MAX_GDP		13

vdi_function gdps[] =
{
	0,
	v_bar,
	v_arc,
	v_pieslice,
	v_circle,
	v_ellipse,
	v_ellarc,
	v_ellpie,
	v_rbox,
	v_rfbox,
	v_justified,
	0,
	0,
	v_bez_onoff
};

void
v_gdp( VDIPB *pb, VIRTUAL *v)
{
	short sub;
	vdi_function gdp;

	sub = pb->contrl[SUBFUNCTION];

	if (sub < MIN_GDP || sub > MAX_GDP)
		return;
	else
		gdp = gdps[sub];

	if (gdp)
		(*gdp)(pb, v);

}

/* Sup opcode 1 */
void
v_bar( VDIPB *pb, VIRTUAL *v)
{
	RASTER *r = v->raster;
	COLINF *c = v->colinf;
	OVDI_DRAWERS *drw = r->drawers;
	VDIRECT *clip;
	LINE_ATTRIBS *latr;
	short interior;
	short coords[10];

	clip = v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;
	interior = v->fill.interior;
	latr = &v->line;

	sortcpy_corners(&pb->ptsin[0], &coords[0]);
	DRAW_FILLEDRECT( r, c, (VDIRECT *)&coords[0], clip,
			interior == FIS_USER ? &v->udpat : &v->pattern, interior);

	if (v->fill.perimeter)
	{
		short ptsbuff[10*5*2];

		coords[4] = coords[2];
		coords[5] = coords[7] = coords[3];
		coords[6] = coords[8] = coords[0];
		coords[3] = coords[9] = coords[1];

		DRAW_PLINE( r, c, (short *)&coords, 5, clip, (short *)&ptsbuff, sizeof(ptsbuff), latr, &v->perimdata);
	}
}

/* Sub opcode 2 */
void
v_arc( VDIPB *pb, VIRTUAL *v)
{
	DRAW_ARC( v,  pb->ptsin[0], pb->ptsin[1], pb->ptsin[6], 
			pb->intin[0], pb->intin[1],
			(short *)&v->ptsbuff,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);
}

/* Sub opcode 3 */
void
v_pieslice( VDIPB *pb, VIRTUAL *v)
{
	DRAW_PIESLICE( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[6],
			  pb->intin[0], pb->intin[1],
			  (short *)&v->ptsbuff,
			  v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);
}
			  
/* Sub opcode 4 */
void
v_circle( VDIPB *pb, VIRTUAL *v)
{
	DRAW_CIRCLE( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[4],
			(short *)&v->ptsbuff,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);
}

/* Sup opcode 5 */
void
v_ellipse( VDIPB *pb, VIRTUAL *v)
{
	DRAW_ELLIPSE( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
			(short *)&v->ptsbuff,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);
}

/* Sub opcode 6 */
void
v_ellarc( VDIPB *pb, VIRTUAL *v)
{
	DRAW_ELLIPSEARC( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
				pb->intin[0], pb->intin[1],
				(short *)&v->ptsbuff,
				v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);
}

/* Sub opcode 7 */
void
v_ellpie( VDIPB *pb, VIRTUAL *v)
{
	DRAW_ELLIPSEPIE( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
				pb->intin[0], pb->intin[1],
				(short *)&v->ptsbuff,
				v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);
}

/* Sub opcode 8 */
void
v_rbox( VDIPB *pb, VIRTUAL *v)
{
	short corners[4];

	sortcpy_corners((short *)&pb->ptsin[0], (short *)&corners);
	DRAW_RBOX( v, 8, (VDIRECT *)&corners,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);
}

/* Sub opcode 9 */
void
v_rfbox( VDIPB *pb, VIRTUAL *v)
{
	short corners[4];
	sortcpy_corners((short *)&pb->ptsin[0], (short *)&corners);
	DRAW_RBOX( v, 9, (VDIRECT *)&corners,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);
}

void
v_justified( VDIPB *pb, VIRTUAL *v)
{
	short slen, plen;

	if ((slen = pb->contrl[N_INTIN] - 2) <= 0)
		return;

	if (!(plen = pb->ptsin[2]))
	{
		output_gdftext( v, (POINT *)&pb->ptsin[0], (short *)&pb->intin[2], slen,
				0, 0, 0);
	}
	else
	{
		output_gdftext( v, (POINT *)&pb->ptsin[0], (short *)&pb->intin[2], slen,
				   plen, pb->intin[0], pb->intin[1]);
	}
}

void
v_bez_onoff( VDIPB *pb, VIRTUAL *v)
{
	return;
}
