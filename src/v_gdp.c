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
	int sub;
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
	PatAttr *ptrn = v->currfill;
	VDIRECT *clip;
	O_Pos coords[10];

	clip = v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;

	sortcpy_corners((O_16 *)&pb->ptsin[0], &coords[0]);
	DRAW_FILLEDRECT( r, c, (VDIRECT *)&coords[0], clip, ptrn);

	if (ptrn->t.f.perimeter)
	{
		O_Pos ptsbuff[10*5*2];

		coords[4] = coords[2];
		coords[5] = coords[7] = coords[3];
		coords[6] = coords[8] = coords[0];
		coords[3] = coords[9] = coords[1];

		DRAW_PLINE( r, c, (O_Pos *)&coords, 5, clip, (O_Pos *)&ptsbuff, sizeof(ptsbuff), ptrn->t.f.perimeter);
	}
}

/* Sub opcode 2 */
void
v_arc( VDIPB *pb, VIRTUAL *v)
{
	DRAW_ARC( v,  pb->ptsin[0], pb->ptsin[1], pb->ptsin[6], 
			pb->intin[0], pb->intin[1],
			(O_Pos *)&v->ptsbuff, v->currfill );
}

/* Sub opcode 3 */
void
v_pieslice( VDIPB *pb, VIRTUAL *v)
{
	DRAW_PIESLICE( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[6],
			  pb->intin[0], pb->intin[1],
			  (O_Pos *)&v->ptsbuff, v->currfill );
}
			  
/* Sub opcode 4 */
void
v_circle( VDIPB *pb, VIRTUAL *v)
{
	DRAW_CIRCLE( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[4],
			(O_Pos *)&v->ptsbuff, v->currfill );
}

/* Sup opcode 5 */
void
v_ellipse( VDIPB *pb, VIRTUAL *v)
{
	DRAW_ELLIPSE( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
			(O_Pos *)&v->ptsbuff, v->currfill );
}

/* Sub opcode 6 */
void
v_ellarc( VDIPB *pb, VIRTUAL *v)
{
	DRAW_ELLIPSEARC( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
				pb->intin[0], pb->intin[1],
				(O_Pos *)&v->ptsbuff, v->currfill );
}

/* Sub opcode 7 */
void
v_ellpie( VDIPB *pb, VIRTUAL *v)
{
	DRAW_ELLIPSEPIE( v, pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
				pb->intin[0], pb->intin[1],
				(O_Pos *)&v->ptsbuff, v->currfill );
}

/* Sub opcode 8 */
void
v_rbox( VDIPB *pb, VIRTUAL *v)
{
	O_Pos corners[4];

	sortcpy_corners((O_16 *)&pb->ptsin[0], (O_Pos *)&corners);
	DRAW_RBOX( v, 8, (VDIRECT *)&corners, v->currfill );
}

/* Sub opcode 9 */
void
v_rfbox( VDIPB *pb, VIRTUAL *v)
{
	O_Pos corners[4];

	sortcpy_corners((O_16 *)&pb->ptsin[0], (O_Pos *)&corners);
	DRAW_RBOX( v, 9, (VDIRECT *)&corners, v->currfill );
}

void
v_justified( VDIPB *pb, VIRTUAL *v)
{
	int slen, plen;
	POINT p;

	p.x = pb->ptsin[0];
	p.y = pb->ptsin[1];

	if ((slen = pb->contrl[N_INTIN] - 2) <= 0)
		return;

	if (!(plen = pb->ptsin[2]))
	{
		output_gdftext( v, (POINT *)&p, (O_16 *)&pb->intin[2], slen,
				0, 0, 0);
	}
	else
	{
		output_gdftext( v, (POINT *)&p, (O_16 *)&pb->intin[2], slen,
				   plen, pb->intin[0], pb->intin[1]);
	}
}

void
v_bez_onoff( VDIPB *pb, VIRTUAL *v)
{
	return;
}
