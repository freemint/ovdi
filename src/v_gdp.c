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

	return;
}

/* Sup opcode 1 */
void
v_bar( VDIPB *pb, VIRTUAL *v)
{

	short coords[10];

	sortcpy_corners(&pb->ptsin[0], &coords[0]);
	rectfill( v,	(VDIRECT *)&coords[0],
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);

	if (v->fill.perimeter)
	{
		short ptsbuff[100*5*2];

		coords[4] = coords[2];
		coords[5] = coords[7] = coords[3];
		coords[6] = coords[8] = coords[0];
		coords[3] = coords[9] = coords[1];

		pline( v, (short *)&coords, 5, (short *)&ptsbuff, sizeof(ptsbuff), &v->perimdata); //&v->linedat);
	}
	return;
}

/* Sub opcode 2 */
void
v_arc( VDIPB *pb, VIRTUAL *v)
{
	short points[MAX_ARC_CT * 3];

	draw_arc( v, 	pb->ptsin[0], pb->ptsin[1], pb->ptsin[6], 
			pb->intin[0], pb->intin[1],
			(short *)&points,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);

	return;
}

/* Sub opcode 3 */
void
v_pieslice( VDIPB *pb, VIRTUAL *v)
{
	short points[MAX_ARC_CT * 3];

	draw_pieslice( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[6],
			  pb->intin[0], pb->intin[1],
			  (short *)&points,
			  v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);

	return;
}
			  
/* Sub opcode 4 */
void
v_circle( VDIPB *pb, VIRTUAL *v)
{
	short points[MAX_ARC_CT * 3];

	draw_circle( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[4],
			(short *)&points,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);


	return;
}

/* Sup opcode 5 */
void
v_ellipse( VDIPB *pb, VIRTUAL *v)
{
	short points[MAX_ARC_CT *3];

	draw_ellipse( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
			(short *)&points,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);


	return;
}

/* Sub opcode 6 */
void
v_ellarc( VDIPB *pb, VIRTUAL *v)
{
	short points[MAX_ARC_CT * 3];

	draw_ellipsearc( v,	pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
				pb->intin[0], pb->intin[1],
				(short *)&points,
				v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);

	return;
}

/* Sub opcode 7 */
void
v_ellpie( VDIPB *pb, VIRTUAL *v)
{
	short points[MAX_ARC_CT * 3];

	draw_ellipsepie( v,	pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
				pb->intin[0], pb->intin[1],
				(short *)&points,
				v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);

	return;
}

/* Sub opcode 8 */
void
v_rbox( VDIPB *pb, VIRTUAL *v)
{
	short corners[4];

	sortcpy_corners((short *)&pb->ptsin[0], (short *)&corners);
	draw_rbox( v, 8, (VDIRECT *)&corners,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);

	return;
}

/* Sub opcode 9 */
void
v_rfbox( VDIPB *pb, VIRTUAL *v)
{
	short corners[4];
	sortcpy_corners((short *)&pb->ptsin[0], (short *)&corners);
	draw_rbox( v, 9, (VDIRECT *)&corners,
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);

	return;
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

	return;
}
void
v_bez_onoff( VDIPB *pb, VIRTUAL *v)
{
	return;
}
