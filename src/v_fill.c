#include "display.h"
#include "draw.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "polygon.h"
#include "rasters.h"
#include "vdi_globals.h"
#include "v_attribs.h"
#include "v_fill.h"
#include "v_perimeter.h"
#include "patattr.h"
#include "styles.h"

//short UDPAT_BUFF[ (2*16) * 16];
void
lvsf_initial( VIRTUAL *v )
{
	PatAttr *f = &v->fill, *u = &v->udfill;
	int i;

	f->exp_data	= (O_u16 *)&v->filldata.edata;
	f->mask		= (O_u16 *)&v->filldata.mask;

	u->data		= (O_u16 *)&v->udfilldata.data;
	u->mask		= (O_u16 *)&v->udfilldata.mask;
	u->exp_data	= (O_u16 *)&v->udfilldata.edata;

	for (i = 0; i < 4; i++)
		f->color[i] = f->bgcol[i] = u->color[i] = u->bgcol[i] = -1;

	f->interior = f->wrmode = u->interior = u->wrmode = -1;
	f->t.f.style = -1;
}

void
vsf_color ( VDIPB *pb, VIRTUAL *v )
{
	lvsf_color (v, pb->intin[0]);
	lvsprm_color(v, pb->intin[0]);
	pb->intout[0] = v->colinf->color_hw2vdi[v->fill.color[0]];
	
	pb->contrl[N_INTOUT] = 1;
}

void
lvsf_color( VIRTUAL *v, O_Int color )
{
	set_pa_fgcolor(&v->fill, v->colinf, color);
	set_pa_fgcolor(&v->udfill, v->colinf, color);
}

void
lvsf_bgcolor( VIRTUAL *v, O_Int color )
{
	set_pa_bgcolor(&v->fill, v->colinf, color);
	set_pa_bgcolor(&v->udfill, v->colinf, color);
}

void
lvsf_wrmode( VIRTUAL *v, O_Int wrmode )
{
	set_pa_writemode(&v->fill, wrmode);
}

void
lvsudf_wrmode( VIRTUAL *v, O_Int wrmode )
{
	set_pa_writemode(&v->udfill, wrmode);
}

void
vsf_interior ( VDIPB *pb, VIRTUAL *v )
{

	lvsf_interior(v, pb->intin[0]);
	
	pb->intout[0] = v->currfill->interior;
	pb->contrl[N_INTOUT] = 1;
}

void
lvsf_interior(VIRTUAL *v, O_Int interior)
{

	if (interior == FIS_USER)
	{
		v->currfill = &v->udfill;
	}
	else
	{
		v->currfill = &v->fill;
		if (v->fill.interior != interior)
			set_fill_params (interior, v->fill.t.f.style, &v->fill, &v->fill.interior, &v->fill.t.f.style);
	}
}

void
vsf_perimeter( VDIPB *pb, VIRTUAL *v )
{
	lvsf_perimeter (v, pb->intin[0]);
	pb->intout[0] = v->fill.t.f.perimeter ? 1: 0;

	pb->contrl[N_INTOUT] = 1;
}

void
lvsf_perimeter( VIRTUAL *v, O_Int flag)
{
	if (flag)
		v->fill.t.f.perimeter = v->udfill.t.f.perimeter = &v->perimeter;
	else
		v->fill.t.f.perimeter = v->udfill.t.f.perimeter = 0;
}

void
vsf_style( VDIPB *pb, VIRTUAL *v)
{
	int style = pb->intin[0];

	if (v->fill.t.f.style != style)
	{
		set_fill_params( v->fill.interior, pb->intin[0], &v->fill, &v->fill.interior, &v->fill.t.f.style );
	}
	pb->intout[0] = v->fill.t.f.style;

	pb->contrl[N_INTOUT] = 1;
}

void
vsf_udpat( VDIPB *pb, VIRTUAL *v )
{
	set_udfill( v, pb->contrl[N_INTIN] / 16, (O_u16 *)&pb->intin[0], 16, 16 );
}

void
v_fillarea( VDIPB *pb, VIRTUAL *v)
{
	RASTER *r = v->raster;
	VDIRECT *clip;

	if (pb->contrl[N_PTSIN] < 2)
		return;

	clip = v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;
	DRAW_FILLEDPOLY(r, v->colinf, (O_Pos *)&pb->ptsin[0], pb->contrl[N_PTSIN], clip, (O_Pos *)&v->spanbuff, v->spanbuffsiz,
			v->currfill );
}

void
vr_recfl( VDIPB *pb, VIRTUAL *v)
{
	RASTER *r = v->raster;
	O_Pos coords[4];

	sortcpy_corners((O_16 *)&pb->ptsin[0], &coords[0]);
	DRAW_FILLEDRECT( r, v->colinf, (VDIRECT *)&coords[0], v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1,
			v->currfill );
}

void
set_fill_params(O_Int interior, O_Int style, PatAttr *ptrn, O_16 *iout, O_16 *sout)
{

	if (interior > MAX_FIS)
		interior = FIS_SOLID;
	else if (interior < FIS_HOLLOW)
		interior = FIS_SOLID;

	switch (interior)
	{

		case FIS_HOLLOW:
		case FIS_SOLID:
		{
			ptrn->expanded = 0;
			ptrn->width = 16;
			ptrn->height = 1;
			ptrn->wwidth = 1;
			ptrn->planes = 1;
			if (interior == FIS_HOLLOW)
			{
				ptrn->data = &HOLLOW;
			}
			else
			{
				ptrn->data = &SOLID;
			}
			break;
		}
		case FIS_PATTERN:
		{
			ptrn->expanded = 0;
			ptrn->width = 16;
			ptrn->wwidth = 1;
			ptrn->planes = 1;

			if (style < 1 || style > MAX_FIL_PAT_INDEX)
				style = 1;

			if (style > 8)
			{
				ptrn->height = PATTERNLEN + 1;
				ptrn->data = &PATTERNS[ (style - 8 - 1) * (PATTERNLEN +1)];
			}
			else
			{
				ptrn->height = DITHERLEN + 1;
				ptrn->data = &DITHER[ (style - 1) * (DITHERLEN + 1)];
			}

			break;
		}
		case FIS_HATCH:
		{
			ptrn->expanded = 0;
			ptrn->width = 16;
			ptrn->wwidth = 1;
			ptrn->planes = 1;

			if (style < 1 || style > MAX_FIL_HAT_INDEX)
				style = 1;

			if (style > 6)
			{
				ptrn->height = HATCH1LEN + 1;
				ptrn->data = &HATCH1[ (style - 6 - 1) * (HATCH1LEN + 1)];
			}
			else
			{
				ptrn->height = HATCH0LEN + 1;
				ptrn->data = &HATCH0[ (style - 1) * (HATCH0LEN + 1)];
			}
			break;
		}
		case FIS_USER:
		{
			break;
		}
	}

	if (iout)
		*iout = interior;
	if (sout)
		*sout = style;

	return;
}

void
set_udfill( VIRTUAL *v, O_Int planes, O_u16 *ud, O_Int width, O_Int height)
{
	O_u16 *src, *dst;
	int i;

	if (ud)
	{
		//log("Setting udfill for %s. Planes %d, width %d, height %d\n", v->procname, planes, width, height);

		src = ud;
		dst = (O_u16 *)v->udfill.data;

		if (planes == 1)
			v->udfill.expanded = 0;
		else if (planes == v->raster->res.planes)
		{
			//log("set_udfill: Multiplane fill for pid %d - %s\n", v->pid, v->procname);
			v->udfill.expanded = 0;
		}
		else
		{
			//log("set_udfill: Forcing ROM pattern for '%s'\n", v->procname);
			src = (O_u16 *)&ROM_UD_PATRN;
			v->udfill.expanded = 0;
			width = 16;
			height = 16;
			planes = 1;
		}

		v->udfill.width = width;
		v->udfill.height = height;
		v->udfill.wwidth = (width + 15) >> 4; //(width / 16) * planes;
		v->udfill.data = dst;

		for (i = (v->udfill.wwidth * planes) * height; i > 0; i--)
			*dst++ = *src++;
	}
	else
	{
		src = (O_u16 *)&ROM_UD_PATRN;
		dst = (O_u16 *)v->udfill.data;

		v->udfill.expanded = 0;
		v->udfill.width = 16;
		v->udfill.height = 16;
		v->udfill.wwidth = 1;
		v->udfill.planes = 1;
		v->udfill.data = dst;

		for (i = 0; i < 16; i++)
			*dst++ = *src++;
	}
	v->udfill.interior = FIS_USER;
}

void
vqf_attributes( VDIPB *pb, VIRTUAL *v)
{

	pb->intout[0] = v->currfill->interior;
	pb->intout[1] = v->colinf->color_hw2vdi[v->fill.color[0]];
	pb->intout[2] = v->fill.t.f.style;
	pb->intout[3] = v->fill.wrmode + 1;
	pb->intout[4] = v->fill.t.f.perimeter ? 1 : 0;

	pb->contrl[N_INTOUT] = 5;
	return;	
}
