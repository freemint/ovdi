#include "display.h"
#include "draw.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "polygon.h"
#include "rasters.h"
#include "vdi_globals.h"
#include "v_attribs.h"
#include "v_fill.h"
#include "styles.h"

//short UDPAT_BUFF[ (2*16) * 16];

void
vsf_color ( VDIPB *pb, VIRTUAL *v )
{
	lvsf_color (v, pb->intin[0]);
	lvsprm_color(v, pb->intin[0]);
	pb->intout[0] = v->colinf->color_hw2vdi[v->fill.color[0]];
	
	pb->contrl[N_INTOUT] = 1;
}

void
lvsf_color( VIRTUAL *v, short color )
{
	register short maxcolor, planes;

	planes = v->raster->planes;
	maxcolor = v->colinf->pens;

	if (color < 0)
		color = 0;
	else if (color >= maxcolor)
		color = maxcolor - 1;

	color = v->colinf->color_vdi2hw[color];

	v->fill.color[0] = v->fill.color[1] = v->udfill.color[0] = v->udfill.color[1] = color;
	v->fill.color[2] = v->fill.color[3] = v->udfill.color[2] = v->udfill.color[3] = planes > 8 ? 0x0 : 0xff;
	v->fill.expanded = v->udfill.expanded = 0;
}

void
lvsf_bgcolor( VIRTUAL *v, short color )
{
	register short maxcolor, planes;

	planes = v->raster->planes;
	maxcolor = v->colinf->pens;

	if (color < 0)
		color = 0;
	else if (color >= maxcolor)
		color = maxcolor - 1;

	color = v->colinf->color_vdi2hw[color];

	v->fill.bgcol[0] = v->fill.bgcol[1] = v->udfill.bgcol[0] = v->udfill.bgcol[1] = color;
	v->fill.bgcol[2] = v->fill.bgcol[3] = v->udfill.bgcol[2] = v->udfill.bgcol[3] = planes > 8 ? 0xff : 0x0;
	v->fill.expanded = v->udfill.expanded = 0;
}

void
lvsprm_color( VIRTUAL *v, short color )
{
	register short maxcolor, planes;

	planes = v->raster->planes;
	maxcolor = v->colinf->pens;

	if (color < 0)
		color = 0;
	else if (color >= maxcolor)
		color = maxcolor - 1;

	color = v->colinf->color_vdi2hw[color];

	v->perimeter.color[0] = v->perimeter.color[1] = color;
	v->perimeter.color[2] = v->perimeter.color[3] = planes > 8 ? 0x0 : 0xff;
}

void
lvsprm_bgcolor( VIRTUAL *v, short color )
{
	register short maxcolor, planes;

	planes = v->raster->planes;
	maxcolor = v->colinf->pens;

	if (color < 0)
		color = 0;
	else if (color >= maxcolor)
		color = maxcolor - 1;

	color = v->colinf->color_vdi2hw[color];

	v->perimeter.bgcol[0] = v->perimeter.bgcol[1] = color;
	v->perimeter.bgcol[2] = v->perimeter.bgcol[3] = planes > 8 ? 0xff : 0x0;
}

void
lvsf_wrmode( VIRTUAL *v, short wrmode )
{
	set_writingmode( wrmode, &v->fill.wrmode);
	v->fill.expanded = 0;
}

void
lvsprm_wrmode( VIRTUAL *v, short wrmode )
{
	set_writingmode( wrmode, &v->perimeter.wrmode);
}

void
lvsudf_wrmode( VIRTUAL *v, short wrmode )
{
	set_writingmode( wrmode, &v->udfill.wrmode);
	v->udfill.expanded = 0;
}

void
vsf_interior ( VDIPB *pb, VIRTUAL *v )
{

	lvsf_interior(v, pb->intin[0]);
	
	pb->intout[0] = v->currfill->interior;
	pb->contrl[N_INTOUT] = 1;
}

void
lvsf_interior(VIRTUAL *v, short interior)
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
lvsf_perimeter( VIRTUAL *v, short flag)
{
	if (flag)
		v->fill.t.f.perimeter = v->udfill.t.f.perimeter = &v->perimeter;
	else
		v->fill.t.f.perimeter = v->udfill.t.f.perimeter = 0;
}

void
vsf_style( VDIPB *pb, VIRTUAL *v)
{
	set_fill_params( v->fill.interior, pb->intin[0], &v->fill, &v->fill.interior, &v->fill.t.f.style );
	pb->intout[0] = v->fill.t.f.style;

	pb->contrl[N_INTOUT] = 1;
}

void
vsf_udpat( VDIPB *pb, VIRTUAL *v )
{
	set_udfill( v, pb->contrl[N_INTIN] / 16, (short *)&pb->intin[0], 16, 16 );
}

void
v_fillarea( VDIPB *pb, VIRTUAL *v)
{
	RASTER *r = v->raster;
	VDIRECT *clip;

	if (pb->contrl[N_PTSIN] < 2)
		return;

	clip = v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;
	DRAW_FILLEDPOLY(r, v->colinf, &pb->ptsin[0], pb->contrl[N_PTSIN], clip, (short *)&v->spanbuff, v->spanbuffsiz,
			v->currfill );
}

void
vr_recfl( VDIPB *pb, VIRTUAL *v)
{
	RASTER *r = v->raster;
	short coords[4];

	sortcpy_corners(&pb->ptsin[0], &coords[0]);
	DRAW_FILLEDRECT( r, v->colinf, (VDIRECT *)&coords[0], v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1,
			v->currfill );
}

void
set_fill_params(short interior, short style, PatAttr *ptrn, short *iout, short *sout)
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
set_udfill( VIRTUAL *v, short planes, short *ud, short width, short height)
{
	short *src, *dst;
	int i;

	if (ud)
	{
		//log("Setting udfill for %s. Planes %d, width %d, height %d\n", v->procname, planes, width, height);

		src = ud;
		dst = (short *)v->udfill.data;

		if (planes == 1)
			v->udfill.expanded = 0;
		else if (planes == v->raster->planes)
		{
			//log("set_udfill: Multiplane fill for pid %d - %s\n", v->pid, v->procname);
			v->udfill.expanded = 0;
		}
		else
		{
			//log("set_udfill: Forcing ROM pattern for '%s'\n", v->procname);
			src = (short *)&ROM_UD_PATRN;
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
		src = (short *)&ROM_UD_PATRN;
		dst = (short *)v->udfill.data;

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
