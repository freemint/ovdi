#include "display.h"
#include "draw.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "ovdi_dev.h"
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
	pb->intout[0] = v->color_hw2vdi[v->fill.color];
	
	pb->contrl[N_INTOUT] = 1;
	return;
}

void
lvsf_color( VIRTUAL *v, short color )
{
	register short maxcolor, bgcol;

	bgcol = v->color_vdi2hw[0];

	maxcolor = Planes2Pens[v->raster->planes];
	color = color < maxcolor ? color : maxcolor - 1;
	color = v->color_vdi2hw[color];
	v->fill.color = color;
	v->fill.bgcol = bgcol;
	v->pattern.color[0] = v->pattern.color[1] = v->udpat.color[0] = v->udpat.color[1] = color;
	v->pattern.color[2] = v->pattern.color[3] = v->udpat.color[2] = v->udpat.color[3] = 0xff;	/* fixme for HC/TC!! */
	v->pattern.bgcol = bgcol;
	v->udpat.bgcol = bgcol;
	return;
}

/* Set perimeter color */
void
lvsprm_color( VIRTUAL *v, short color)
{
	color = color < Planes2Pens[v->raster->planes] ? color : Planes2Pens[v->raster->planes] - 1;
	color = v->color_vdi2hw[color];

	v->perimdata.bgcol = v->color_vdi2hw[0];
	v->perimdata.color[0] = v->perimdata.color[1] = color;
	v->perimdata.color[2] = v->perimdata.color[3] = 0xff;
	return;
}

void
lvsf_wrmode( VIRTUAL *v, short wrmode )
{
	set_writingmode( wrmode, &v->pattern.wrmode);
	return;
}

void
lvsprm_wrmode( VIRTUAL *v, short wrmode )
{
	set_writingmode( wrmode, &v->perimdata.wrmode);
	return;
}

void
lvsudf_wrmode( VIRTUAL *v, short wrmode )
{
	set_writingmode( wrmode, &v->udpat.wrmode);
	return;
}

void
vsf_interior ( VDIPB *pb, VIRTUAL *v )
{
	set_fill_params (pb->intin[0], v->fill.style, &v->pattern, &v->fill.interior, &v->fill.style);
	pb->intout[0] = v->fill.interior;

	pb->contrl[N_INTOUT] = 1;
	return;
}

void
vsf_perimeter( VDIPB *pb, VIRTUAL *v )
{
	lvsf_perimeter (v, pb->intin[0]);
	pb->intout[0] = v->fill.perimeter;

	pb->contrl[N_INTOUT] = 1;
	return;
}

void
lvsf_perimeter( VIRTUAL *v, short flag)
{
	v->fill.perimeter = flag;
	return;
}

void
vsf_style( VDIPB *pb, VIRTUAL *v)
{
	set_fill_params( v->fill.interior, pb->intin[0], &v->pattern, &v->fill.interior, &v->fill.style );
	pb->intout[0] = v->fill.style;

	pb->contrl[N_INTOUT] = 1;
	return;
}

void
vsf_udpat( VDIPB *pb, VIRTUAL *v )
{
	set_udfill( v, pb->contrl[N_INTIN] / 16, (short *)&pb->intin[0], 16, 16 );
	return;
}

void
v_fillarea( VDIPB *pb, VIRTUAL *v)
{
	//short ptsbuff[10*5*2];

	if (pb->contrl[N_PTSIN] < 2)
		return;

	filled_poly(    v, &pb->ptsin[0], pb->contrl[N_PTSIN], (short *)&v->spanbuff, v->spanbuffsiz, //(short *)&ptsbuff, sizeof(ptsbuff), 
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);

	return;
}

void
vr_recfl( VDIPB *pb, VIRTUAL *v)
{
	short coords[4];

	sortcpy_corners(&pb->ptsin[0], &coords[0]);
	rectfill( v,	(VDIRECT *)&coords[0],
			v->fill.interior == FIS_USER ? &v->udpat : &v->pattern);
	return;
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
			ptrn->mask = 0;
			if (interior == FIS_HOLLOW)
			{
				ptrn->mask = 0;
				ptrn->data = &HOLLOW;
			}
			else
			{
				ptrn->mask = -1;
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
			ptrn->mask = -1;

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
			ptrn->mask = -1;

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
		dst = (short *)&v->udpatdata.edata;

		if (planes == 1)
			v->udpat.expanded = 0;
		else if (planes == v->raster->planes)
		{
			//log("set_udfill: Multiplane fill for pid %d - %s\n", v->pid, v->procname);
			v->udpat.expanded = 1;
		}
		else
		{
			//log("set_udfill: Forcing ROM pattern for '%s'\n", v->procname);
			src = (short *)&ROM_UD_PATRN;
			v->udpat.expanded = 0;
			width = 16;
			height = 16;
			planes = 1;
		}

		v->udpat.width = width;
		v->udpat.height = height;
		v->udpat.wwidth = (width + 15) >> 4; //(width / 16) * planes;
		v->udpat.mask = -1;
		v->udpat.data = dst;

		for (i = (v->udpat.wwidth * planes) * height; i > 0; i--)
			*dst++ = *src++;
	}
	else
	{
		src = (short *)&ROM_UD_PATRN;
		dst = (short *)&v->udpatdata.data;

		v->udpat.expanded = 0;
		v->udpat.width = 16;
		v->udpat.height = 16;
		v->udpat.wwidth = 1;
		v->udpat.planes = 1;
		v->udpat.mask = -1;
		v->udpat.data = dst;

		for (i = 0; i < 16; i++)
			*dst++ = *src++;

	}
}

void
vqf_attributes( VDIPB *pb, VIRTUAL *v)
{

	pb->intout[0] = v->fill.interior;
	pb->intout[1] = v->color_hw2vdi[v->fill.color]; //HW2VDI_colorindex[v->fill.color];
	pb->intout[2] = v->fill.style;
	pb->intout[3] = v->wrmode + 1;
	pb->intout[4] = v->fill.perimeter;

	pb->contrl[N_INTOUT] = 5;
	return;	
}
