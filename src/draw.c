#include "display.h"
#include "draw.h"
#include "line.h"
#include "math.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "polygon.h"

void
rectfill( RASTER *r, COLINF *c, VDIRECT *corners, VDIRECT *clip, PatAttr *ptrn)
{
	VDIRECT clipped;

	clipped = *corners;

	if (!clipbox (&clipped, clip))
		return;

	if (ptrn->width == 16 && FILL_16X_PTR(r))
		FILL_16X(r, c, (short *)&clipped, ptrn);
	else
		DRAW_MSPANS( r, c, clipped.x1, clipped.x2, clipped.y1, clipped.y2, ptrn);
#if 0
	{

		if (ptrn->height > 1 && FILL_16X_PTR(r))
			FILL_16X(r, c, (short *)&clipped, interior, ptrn);
		else if (ptrn->height == 1)
		{
			if (DRAW_SOLID_RECT_PTR(r))
				DRAW_SOLID_RECT(r, c, (short *)&clipped, wrmode, interior == FIS_HOLLOW ? ptrn->bgcol[wrmode] : ptrn->color[wrmode]);
			else if (FILL_16X_PTR(r))
				FILL_16X(r, c, (short *)&clipped, interior, ptrn);
			else
				goto slowashell;
		}
		else	
			DRAW_MSPANS( r, c, clipped.x1, clipped.x2, clipped.y1, clipped.y2, ptrn);
	}
	else
slowashell:	DRAW_MSPANS( r, c, clipped.x1, clipped.x2, clipped.y1, clipped.y2, ptrn);
#endif		

#if 0	

	color = *ptrn->data;

	if (	(interior == FIS_SOLID || interior == FIS_HOLLOW
		|| (ptrn->height == 1 && ptrn->width == 16 && (color == 0xffff || color == 0)) )
		&&  DRAW_SOLID_RECT_PTR(r)
	   )
	{
		wrmode	= ptrn->wrmode;
		color	= interior == FIS_SOLID ? ptrn->color[wrmode] : ptrn->bgcol[wrmode];

		DRAW_SOLID_RECT(r, c, (short *)&clipped, wrmode, color);
	}
	else if (FILL_16X_PTR(r))
		FILL_16X(r, c, (short *)&clipped, ptrn->wrmode, ptrn);
	else
		DRAW_MSPANS( r, c, clipped.x1, clipped.x2, clipped.y1, clipped.y2, ptrn);
#endif
}

void
sortcpy_corners( short *source, short *dest )
{
	register short x1, y1, x2, y2, tmp;

	x1 = *source++;
	y1 = *source++;
	x2 = *source++;
	y2 = *source;

	if (x1 > x2)
	{
		tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if (y1 > y2)
	{
		tmp = y2;
		y2 = y1;
		y1 = tmp;
	}

	*dest++	= x1;
	*dest++	= y1;
	*dest++	= x2;
	*dest	= y2;

	return;
}

short
clipbox( VDIRECT *corners, VDIRECT *clip)
{
	register short x1, y1, x2, y2, tmp;
	register short *co;

	x1 = corners->x1;
	x2 = corners->x2;

	/* clip x coordinates */

	tmp = clip->x1;
	if ( x1 < tmp)
	{
		if (x2 < tmp)
		{
			return 0;		/* clipped box is null */
		}
		x1 = tmp;
	}

	tmp = clip->x2;
	if ( x2 > tmp)
	{
		if (x1 > tmp)
		{
			return 0;		/* clipped box is null */
		}
		x2 = tmp;
	}

	/* clip y coordinates */

	tmp = clip->y1;
	y1 = corners->y1;
	y2 = corners->y2;
	if ( y1 < tmp )
	{
		if (y2 < tmp )
		{
			return 0;		/* clipped box is null */
		}
		y1 = tmp;
	}

	tmp = clip->y2;
	if ( y2 > tmp )
	{
		if (y1 > tmp )
		{
			return 0;		/* clipped box is null */
		}
		y2 = tmp;
	}

	co = (short *)corners;
	*co++ = x1;
	*co++ = y1;
	*co++ = x2;
	*co = y2;

	return 1;
}

void
draw_arc(VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn)
{
	short del_ang, yrad, n_steps;
	RASTER *r = v->raster;

	del_ang = end_ang - beg_ang;
	if (del_ang < 0)
		del_ang += 3600;

	yrad = SMUL_DIV(xrad, r->wpixel, r->hpixel);
	n_steps = clc_nsteps(xrad, yrad);
	n_steps = SMUL_DIV(del_ang, n_steps, 3600);
	if (!n_steps)
		return;

	clc_arc(v, 2, xc, yc, xrad, yrad, beg_ang, end_ang, del_ang, n_steps, points, ptrn);
	return;
}

void
draw_pieslice( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn)
{
	short del_ang, yrad, n_steps;
	RASTER *r = v->raster;

	del_ang = end_ang - beg_ang;
	if (del_ang < 0)
		del_ang += 3600;

	yrad = SMUL_DIV(xrad, r->wpixel, r->hpixel);
	n_steps = clc_nsteps(xrad, yrad);
	n_steps = SMUL_DIV(del_ang, n_steps, 3600);
	if (!n_steps)
		return;

	clc_arc(v, 3, xc, yc, xrad, yrad, beg_ang, end_ang, del_ang, n_steps, points, ptrn);

	return;
}
void
draw_circle( VIRTUAL *v, short xc, short yc, short xrad, short *points, PatAttr *ptrn)
{
	short yrad, n_steps;
	RASTER *r = v->raster;

	yrad = SMUL_DIV(xrad, r->wpixel, r->hpixel);
	n_steps = clc_nsteps(xrad, yrad);
	clc_arc( v, 4, xc, yc, xrad, yrad, 0, 3600, 3600, n_steps, points, ptrn);
	return;
}

void
draw_ellipse( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short *points, PatAttr *ptrn)
{
	short n_steps;

	n_steps = clc_nsteps(xrad, yrad);

	clc_arc( v, 5, xc, yc, xrad, yrad, 0, 0, 3600, n_steps, points, ptrn);
	return;

}
void
draw_ellipsearc( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn)
{
	short del_ang, n_steps;

	del_ang = end_ang - beg_ang;
	if (del_ang < 0)
		del_ang += 3600;

	n_steps = clc_nsteps(xrad, yrad);
	n_steps = SMUL_DIV(del_ang, n_steps, 3600);
	if (!n_steps)
		return;

	clc_arc( v, 6, xc, yc, xrad, yrad, beg_ang, end_ang, del_ang, n_steps, points, ptrn);
	return;
}

void
draw_ellipsepie( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn)
{

	short del_ang, n_steps;

	del_ang = end_ang - beg_ang;
	if (del_ang < 0)
		del_ang += 3600;

	n_steps = clc_nsteps(xrad, yrad);
	n_steps = SMUL_DIV(del_ang, n_steps, 3600);
	if (!n_steps)
		return;

	clc_arc( v, 7, xc, yc, xrad, yrad, beg_ang, end_ang, del_ang, n_steps, points, ptrn);
	return;
}
/*
 * clc_nsteps - calculates
 */
short
clc_nsteps( short xrad, short yrad)
{
	short n_steps;

	if (xrad > yrad)
		n_steps = xrad;
	else
		n_steps = yrad;

	n_steps = n_steps >> 2;

	if (n_steps < 16)
		n_steps = 16;
	else if (n_steps > MAX_ARC_CT)
		n_steps = MAX_ARC_CT;

	return n_steps;
}

void
clc_arc(VIRTUAL *v, short gdp_code, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang,
	short del_ang, short n_steps, short *points, PatAttr *ptrn)
{
	RASTER *r;
	COLINF *c;
	VDIRECT *clip;
	short i, j, start, angle;
	register short *pts;

	r = v->raster;
	c = v->colinf;

	clip = v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;

	pts = points;

	if (((xc + xrad) < clip->x1) || ((xc - xrad) > clip->x2) ||
	    ((yc + yrad) < clip->y1) || ((yc - yrad) > clip->y2))
		return;

	start = angle = beg_ang;

	*pts++ = SMUL_DIV(Icos(angle), xrad, 32767) + xc;
	*pts++ = yc - SMUL_DIV(Isin(angle), yrad, 32767);

	for ( i = 1, j = 2; i < n_steps; i++, j += 2)
	{
		angle = SMUL_DIV(del_ang, i, n_steps) + start;
		*pts++ = SMUL_DIV(Icos(angle), xrad, 32767) + xc;
		*pts++ = yc - SMUL_DIV(Isin(angle), yrad, 32767);
	}

	angle = end_ang;
	*pts++ = SMUL_DIV(Icos(angle), xrad, 32767) + xc;
	*pts++ = yc - SMUL_DIV(Isin(angle), yrad, 32767);

	/* If pie wedge, draw to center and then close.
	 * If arc or circle, do nothing because loop should close circle.
	 */

	if ((gdp_code == 3) || (gdp_code == 7))
	{	/* Pie wedge */
		n_steps++;
		*pts++ = xc;
		*pts++ = yc;
	}

	if ((gdp_code == 2) || (gdp_code == 6))	/* Open arc */
		DRAW_PLINE( r, c, points, n_steps + 1, clip, (short *)&v->spanbuff, v->spanbuffsiz, ptrn);
	else
	{
		DRAW_FILLEDPOLY( r, c, points, n_steps + 1, clip, (short *)&v->spanbuff, v->spanbuffsiz, ptrn);

		if (ptrn->t.f.perimeter)
			DRAW_PLINE( r, c, points, n_steps +1, clip, (short *)&v->spanbuff, v->spanbuffsiz, ptrn->t.f.perimeter);
	}
}


/* Assumes coordinates have been sorted */
void
draw_rbox(VIRTUAL *v, short gdp_code, VDIRECT *corners, PatAttr *ptrn)
{
	RASTER *r;
	COLINF *c;
	VDIRECT *clip;
	short i, j;
	short rdeltax, rdeltay;
	short xc, yc, xrad, yrad;
	short n_steps;
	short points[42];

	r = v->raster;
	c = v->colinf;
	clip = v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;

	rdeltax = (corners->x2 - corners->x1) / 2;
	rdeltay = (corners->y2 - corners->y1) / 2;

	xrad = r->w >> 6; //wk->screen.mfdb.width >> 6;
	if (xrad > rdeltax)
	    xrad = rdeltax;

	yrad = SMUL_DIV(xrad, r->wpixel, r->hpixel);
	if (yrad > rdeltay)
	{
		yrad = rdeltay;
		xrad = SMUL_DIV(yrad, r->wpixel, r->hpixel);
	}
	yrad = -yrad;

	n_steps = clc_nsteps(xrad, yrad);

	for(i = 0; i < 5; i++)
	{
		points[i * 2]     = SMUL_DIV(Icos(900 - 225 * i), xrad, 32767);
		points[i * 2 + 1] = SMUL_DIV(Isin(900 - 225 * i), yrad, 32767);
	}

	xc = corners->x2 - xrad;
	yc = corners->y1 - yrad;
	j = 10;

	for(i = 9; i >= 0; i--)
	{ 
		points[j + 1] = yc + points[i--];
		points[j] = xc + points[i];
		j += 2;
	}

	xc = corners->x1 + xrad; 
	j = 20;

	for(i = 0; i < 10; i++)
	{ 
		points[j++] = xc - points[i++];
		points[j++] = yc + points[i];
	}

	yc = corners->y2 + yrad;
	j = 30;

	for(i = 9; i >= 0; i--)
	{ 
		points[j + 1] = yc - points[i--];
		points[j] = xc - points[i];
		j += 2;
	}

	xc = corners->x2 - xrad;
	j = 0;

	for(i = 0; i < 10; i++)
	{ 
		points[j++] = xc + points[i++];
		points[j++] = yc - points[i];
	}

	points[40] = points[0];
	points[41] = points[1]; 

	if (gdp_code == 8)
	{
		DRAW_PLINE( r, c, (short *)&points, 21, clip, (short *)&v->spanbuff, v->spanbuffsiz, ptrn);
	}
	else
	{
		DRAW_FILLEDPOLY( r, c, (short *)&points, 21, clip, (short *)&v->spanbuff, v->spanbuffsiz, ptrn);
		if (ptrn->t.f.perimeter)
			DRAW_PLINE( r, c, (short *)&points, 21, clip, (short *)&v->spanbuff, v->spanbuffsiz, ptrn->t.f.perimeter);
	}
}
