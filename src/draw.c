#include "ovdi_types.h"
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

	
	if (ptrn->width == 16 && FILL_16X_PTR(r) )
		FILL_16X(r, c, (O_Pos *)&clipped, ptrn);
	else
		DRAW_MSPANS( r, c, clipped.x1, clipped.x2, clipped.y1, clipped.y2, ptrn);

#if 0
	{

		if (ptrn->height > 1 && FILL_16X_PTR(r))
			FILL_16X(r, c, (int *)&clipped, interior, ptrn);
		else if (ptrn->height == 1)
		{
			if (DRAW_SOLID_RECT_PTR(r))
				DRAW_SOLID_RECT(r, c, (int *)&clipped, wrmode, interior == FIS_HOLLOW ? ptrn->bgcol[wrmode] : ptrn->color[wrmode]);
			else if (FILL_16X_PTR(r))
				FILL_16X(r, c, (int *)&clipped, interior, ptrn);
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

		DRAW_SOLID_RECT(r, c, (int *)&clipped, wrmode, color);
	}
	else if (FILL_16X_PTR(r))
		FILL_16X(r, c, (int *)&clipped, ptrn->wrmode, ptrn);
	else
		DRAW_MSPANS( r, c, clipped.x1, clipped.x2, clipped.y1, clipped.y2, ptrn);
#endif
}

void
sortcpy_corners( O_16 *source, O_Pos *dest )
{
	O_Pos x1, y1, x2, y2, tmp;

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

O_Int
clipbox( VDIRECT *corners, VDIRECT *clip)
{
	O_Pos x1, y1, x2, y2, tmp;
	O_Pos *co;

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

	co = (O_Pos *)corners;
	*co++ = x1;
	*co++ = y1;
	*co++ = x2;
	*co = y2;

	return 1;
}

void
draw_arc(VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn)
{
	O_Int del_ang, n_steps;
	O_Pos yrad;
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
draw_pieslice( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn)
{
	O_Int del_ang, n_steps;
	O_Pos yrad;
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

#if 0
void
draw_circle( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos *points, PatAttr *ptrn)
{
	O_Pos yrad;
	O_Int n_steps;
	RASTER *r = v->raster;

	yrad = SMUL_DIV(xrad, r->wpixel, r->hpixel);
	n_steps = clc_nsteps(xrad, yrad);
	clc_arc( v, 4, xc, yc, xrad, yrad, 0, 3600, 3600, n_steps, points, ptrn);
	return;
}
#else
//bcircle(RASTER *r, COLINF *c, int sx, int sy, int r, VDIRECT *clip, int *spanbuff, long spanbuffsiz, PatAttr *ptrn)
void
draw_circle( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos *points, PatAttr *ptrn)
{
	VDIRECT *clip;
	RASTER *r = v->raster;

	clip = v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1;
	brescircle(v->raster, v->colinf, xc, yc, xrad, clip, (O_Pos *)&v->spanbuff, v->spanbuffsiz, v->currfill);
}
#endif

void
draw_ellipse( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Pos *points, PatAttr *ptrn)
{
	O_Int n_steps;

	n_steps = clc_nsteps(xrad, yrad);

	clc_arc( v, 5, xc, yc, xrad, yrad, 0, 0, 3600, n_steps, points, ptrn);
	return;

}
void
draw_ellipsearc( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn)
{
	O_Int del_ang, n_steps;

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
draw_ellipsepie( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn)
{

	O_Int del_ang, n_steps;

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
extern long arc_split;
extern O_Int arc_max;
extern O_Int arc_min;

O_Int
clc_nsteps( O_Pos xrad, O_Pos yrad)
{

	O_Int n_steps;

	if (xrad > yrad)
		n_steps = xrad;
	else
		n_steps = yrad;

	n_steps = (arc_split * n_steps) >> 16; //(long)n_steps * arc_split) >> 16;

	if (n_steps < arc_min)
		n_steps = arc_min;
	if (n_steps > arc_max)
		n_steps = arc_max;

#if 0
	n_steps = n_steps >> 2;

	if (n_steps < 16)
		n_steps = 16;
	else if (n_steps > MAX_ARC_CT)
		n_steps = MAX_ARC_CT;

#endif
	return n_steps;
}

static O_Int
clipspan(O_Pos y, O_Pos x1, O_Pos x2, O_Pos *spans, VDIRECT *clip)
{
	if (y < clip->y1)
		return 0;
	if (y > clip->y2)
		return 0;
	if (x2 < clip->x1)
		return 0;
	if (x1 > clip->x2)
		return 0;

	if (x1 < clip->x1)
		x1 = clip->x1;
	if (x2 > clip->x2)
		x2 = clip->x2;

	*spans++ = y;
	*spans++ = x1;
	*spans++ = x2;
	return 1;
}	
void
brescircle( RASTER *r, COLINF *c, O_Pos sx, O_Pos sy, O_Pos rad, VDIRECT *clip, O_Pos *spanbuff, long spanbuffsiz, PatAttr *ptrn)
{
	O_Pos *spans;
	O_Int n_spans, max_spans;
	register long x = 0, y = rad, d = 2L * (1 - rad);
	register long pw = r->wpixel, ph = r->hpixel;

	if (rad < 1)
	{
		//gl_setpixel(sx, sy, c);
		return;
	}

	if (sx + rad < clip->x1 || sx - rad > clip->x2 || sy + rad < clip->y1 || sy - rad > clip->y2)
		return;

	spans = spanbuff;
	max_spans = ((spanbuffsiz / 3) >> 1) - 3;
	n_spans = 0;

	while (y >= 0)
	{
		if (clipspan(sy + y, sx - x, sx + x, spans, clip))
			n_spans++, spans += 3;
		if (clipspan(sy - y, sx - x, sx + x, spans, clip))
			n_spans++, spans += 3;
		if (n_spans > max_spans)
		{
			spans = spanbuff;
			if (SPANS_16X_PTR(r))
				SPANS_16X(r, c, spans, n_spans, ptrn);
			n_spans = 0;
		}
#if 0
		if (fill == 0)
		{
			gl_setpixel(sx + x, sy + y, c);
			gl_setpixel(sx + x, sy - y, c);
			gl_setpixel(sx - x, sy + y, c);
			gl_setpixel(sx - x, sy - y, c);
		}
		else
		{
			gl_hline(sx - x, sy + y, sx + x, c);
			gl_hline(sx - x, sy - y, sx + x, c);
		}
#endif
		if ((d + y) > 0)
		{
			y--;
			d -= (2L * y * pw / ph) - 1; //d -= (2 * y * WIDTH / HEIGHT) - 1;
		}
		if (x > d)
		{
			x++;
			d += (2L * x) + 1;
		}
	}
	if (n_spans && SPANS_16X_PTR(r))
		SPANS_16X(r, c, spanbuff, n_spans, ptrn);
}

void
clc_arc(VIRTUAL *v, O_Int gdp_code, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Int beg_ang, O_Int end_ang,
	O_Int del_ang, O_Int n_steps, O_Pos *points, PatAttr *ptrn)
{
	RASTER *r;
	COLINF *c;
	VDIRECT *clip;
	int i, j;
	O_Int start, angle;
	register O_Pos *pts;

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
		DRAW_PLINE( r, c, points, n_steps + 1, clip, (O_Pos *)&v->spanbuff, v->spanbuffsiz, ptrn);
	else
	{
		DRAW_FILLEDPOLY( r, c, points, n_steps + 1, clip, (O_Pos *)&v->spanbuff, v->spanbuffsiz, ptrn);

		if (ptrn->t.f.perimeter)
			DRAW_PLINE( r, c, points, n_steps +1, clip, (O_Pos *)&v->spanbuff, v->spanbuffsiz, ptrn->t.f.perimeter);
	}
}


/* Assumes coordinates have been sorted */
void
draw_rbox(VIRTUAL *v, O_Int gdp_code, VDIRECT *corners, PatAttr *ptrn)
{
	RASTER *r;
	COLINF *c;
	VDIRECT *clip;
	int i, j;
	O_Pos rdeltax, rdeltay;
	O_Pos xc, yc, xrad, yrad;
	O_Int n_steps;
	O_Pos points[42];

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
		DRAW_PLINE( r, c, (O_Pos *)&points, 21, clip, (O_Pos *)&v->spanbuff, v->spanbuffsiz, ptrn);
	}
	else
	{
		DRAW_FILLEDPOLY( r, c, (O_Pos *)&points, 21, clip, (O_Pos *)&v->spanbuff, v->spanbuffsiz, ptrn);
		if (ptrn->t.f.perimeter)
			DRAW_PLINE( r, c, (O_Pos *)&points, 21, clip, (O_Pos *)&v->spanbuff, v->spanbuffsiz, ptrn->t.f.perimeter);
	}
}
