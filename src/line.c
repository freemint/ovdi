#include "display.h"
//#include "std_driver.h"
#include "line.h"
#include "math.h"
#include "ovdi_defs.h"
#include "polygon.h"
#include "vdi_defs.h"
#include "vdi_globals.h"

extern int logit;

#if 0
static int Planes2xinc[] = 
{ 1, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4 };
#endif

/* Written by Odd Skancke */
void
pline(RASTER *r, COLINF *c, short *pts, long n, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn)
{
	VDIRECT line, clipped;
	short x, y;
	Fabline dl;

	if ((n -= 2) < 0)
		return;

	x = *pts++;
	y = *pts++;

	dl = DRAW_ABLINE_PTR(r);

	while (n >= 0)
	{

		line.x1 = x;
		line.y1 = y;
		line.x2 = x = *pts++;
		line.y2 = y = *pts++;

		clipped = line;

		if (clip_line(&clipped, clip))
			(*dl)(r, c, &clipped, ptrn);

		if ((ptrn->t.l.beg | ptrn->t.l.end) & LE_ARROW)
			do_arrow( r, c, (short *)&line, 2, clip, points, pointasize, ptrn);

		n--;
	}
}

/* Taken from fVDI (line.c), modified by Odd Skancke */
short
code(POINT *input, VDIRECT *clip)
{
	int ccode = 0;
	
	if (input->x < clip->x1)
		ccode = 1;
	else if (input->x > clip->x2)
		ccode = 2;

	if (input->y < clip->y1)
		ccode |= 4;
	else if (input->y > clip->y2)
		ccode |= 8;

	return ccode;
}

#define BOTTOM	1
#define TOP	2
#define LEFT  4
#define RIGHT 8
static short
ncode(short x, short y, VDIRECT *clip)
{
	short c = 0;

	if (x < clip->x1)
		c |= LEFT;
	else if (x > clip->x2)
		c |= RIGHT;
	if (y < clip->y1)
		c |= TOP;
	else if (y > clip->y2)
		c |= BOTTOM;
	return c;
}

short
clip_line(VDIRECT *input, VDIRECT *clip)
{
	short c0, c1, code;
	short x, y, x1, y1, x2, y2;
	long deltax, deltay;

	x1 = x = input->x1;
	y1 = y = input->y1;
	x2 = input->x2;
	y2 = input->y2;
	
	c0 = ncode(x1, y1, clip);
	c1 = ncode(x2, y2, clip);

	while (c0 || c1)
	{
		if ((c0 & c1))
			return 0;

		if (c0) code = c0;
		else	code = c1;

		deltax = (long)(x2 - x1);
		deltay = (long)(y2 - y1);

		if (code & BOTTOM) {
			y = clip->y2;
			x = x1 + (deltax * (y - y1) / deltay);
		}
		if (code & TOP) {
			y = clip->y1;
			x = x1 + (deltax * (y - y1) / deltay);
		}
		if (code & RIGHT) {
			x = clip->x2;
			y = y1 + (deltay * (long)(x - x1) / deltax);
		}
		if (code & LEFT) {
			x = clip->x1;
			y = y1 + (deltay * (long)(x - x1) / deltax);
		}

		if (code == c0) {
			x1 = x;
			y1 = y;
			c0 = ncode(x1, y1, clip);
		} else {
			x2 = x;
			y2 = y;
			c1 = ncode(x2, y2, clip);
		}
	}

	input->x1 = x1;
	input->y1 = y1;
	input->x2 = x2;
	input->y2 = y2;
	return 1;
}
#if 0
/* Taken from fVDI (line.c), modified by Odd Skancke */
short
clip_line(VDIRECT *input, VDIRECT *clip)
{

	short pts1_flag, pts2_flag, clip_flag;
	short deltax, deltay;
	short *x, *y;

	while ( (pts1_flag = code((POINT *)input, clip)) || (pts2_flag = code((POINT *)&input->x2, clip)) )
	{
		
		
	
	
		if (pts1_flag & pts2_flag)
			return 0;
		
		if (pts1_flag)
		{
			clip_flag = pts1_flag;
			x = &input->x1;
			y = &input->y1;
		}
		else
		{
			clip_flag = pts2_flag;
			x = &input->x2;
			y = &input->y2;
		}

		deltax = input->x2 - input->x1;
		deltay = input->y2 - input->y1;

		if (clip_flag & 1)	/* left ? */
		{
			*y = input->y1 + SMUL_DIV(deltay, (clip->x1 - input->x1), deltax);
			*x = clip->x1;
		}
		else if (clip_flag & 2)	/* right? */
		{
			*y = input->y1 + SMUL_DIV(deltay, (clip->x2 - input->x1), deltax);
			*x = clip->x2;
		}
		else if (clip_flag & 4) /* top? */
		{
			*x = input->x1 + SMUL_DIV(deltax, (clip->y1 - input->y1), deltay);
			*y = clip->y1;
		}
		else if (clip_flag & 8) /* bottom */
		{
			*x = input->x1 + SMUL_DIV(deltax, (clip->y2 - input->y1), deltay);
			*y = clip->y2;
		}
	}
	return 1;
}
#endif

/*
 * abline - draw a line (general purpose)
 *
 * Taken from EmuTOS (monout.c), almost totally rewritten by Odd Skancke
*/
void
abline (RASTER *r, COLINF *c, struct vdirect *pnts, PatAttr *ptrn)
{
	unsigned char *addr;		/* using void pointer is much faster */
	short x1,y1,x2,y2;	/* the coordinates */
	short dx;			/* width of rectangle around line */
	short dy;			/* height of rectangle around line */
	short xinc;			/* positive increase for each x step */
	short yinc;			/* in/decrease for each y step */
	short planes, fgcol, bgcol;
	//unsigned short msk;
	unsigned short linemask;	/* linestyle bits */
	register pixel_blit dlp_fg;
	register pixel_blit dlp_bg;
	int	eps, e1, e2, loopcnt;


	//if (!(dy = y2 - y1))
	if (pnts->y1 == pnts->y2)
	{
		DRAW_HLINE(r, c, pnts->x1, pnts->x2, pnts->y1, ptrn);
		return;
	}

	//if (!(dx = x2 - x1))
	if (pnts->x1 == pnts->x2)
	{
		DRAW_VLINE(r, c, pnts->y1, pnts->y2, pnts->x1, ptrn);
		return;
	}
	/* Make x axis always goind up */
	if (pnts->x1 > pnts->x2)
	{
		/* if delta x < 0 then draw from point 2 to 1 */
		x1 = pnts->x2;
		y1 = pnts->y2;
		x2 = pnts->x1;
		y2 = pnts->y1;
	}
	else
	{
		/* positive, start with first point */
		x1 = pnts->x1;
		y1 = pnts->y1;
		x2 = pnts->x2;
		y2 = pnts->y2;
	}
	dx = x2 - x1;
	dy = y2 - y1;
	/* calculate increase values for x and y to add to actual address */
	if (dy < 0)
	{
		dy = -dy;				/* make dy absolute */
		yinc = (long) -1 * r->bypl;	/* sub one line of bytes */
	}
	else
	{
		yinc = (long) r->bypl;		/* add one line of bytes */
	}

	
	linemask = (x1+y1+1) & 0xf ? (*(unsigned short *)ptrn->data << ((x1 & 0xf))) | ( *(unsigned short *)ptrn->data >> (16-(x1 & 0xf)) ) : *(unsigned short *)ptrn->data;
	//linemask = *ptrn->data;
	planes = ptrn->wrmode;

	if (planes == MD_ERASE)
		linemask = ~linemask;

	fgcol = ptrn->color[planes];
	bgcol = ptrn->bgcol[planes];
	planes <<= 1;
	dlp_fg = r->drawers->dlp[planes];
	dlp_bg = 0; //r->drawers->dlp[planes + 1];
	planes = r->res.planes;

	SYNC_RASTER(r);

	if (planes < 8)
	{
		int shift, bitcount;

		xinc = planes << 1;
		addr = (unsigned char *)r->base + ((long)y1 * r->bypl) + ( (long)(x1 >> 4) * xinc);
		
		shift	= x1 & 0xf;
		bitcount = 16 - shift;

		if (dx >= dy)
		{
			e1	= dy * 2;
			eps	= -dx;
			e2	= dx * 2;

			for (loopcnt = dx; loopcnt >= 0; loopcnt--)
			{
				linemask = linemask << 1 | linemask >> 15;
				if (linemask & 1)
				{
					if (dlp_fg)
						(*dlp_fg)(addr, (long)(((long)shift<<16)|fgcol));
				}
				else if (dlp_bg)
					(*dlp_bg)(addr, (long)(((long)shift<<16)|fgcol));

				bitcount--;

				if (bitcount <= 0)
				{
					bitcount = 16;
					addr += xinc;
					shift = 0;
				}
				else
					shift++;

				eps += e1;
				if (eps >= 0)
				{
					eps -= e2;
					addr += yinc;
				}
			}
		}
		else
		{
			e1	= dx * 2;
			eps	= -dy;
			e2	= dy * 2;

			for (loopcnt = dy; loopcnt >= 0; loopcnt--)
			{
				linemask = linemask << 1 | linemask >> 15;
				if (linemask & 1)
				{
					if (dlp_fg)
						(*dlp_fg)(addr, (long)(((long)shift<<16)|fgcol));
				}
				else if (dlp_bg)
					(*dlp_bg)(addr, (long)(((long)shift<<16)|fgcol));
				
				addr += yinc;
				eps += e1;
				if ( eps >= 0 )
				{
					eps -= e2;
					bitcount--;
					if (bitcount <= 0)
					{
						bitcount = 16;
						addr += xinc;
						shift = 0;
					}
					else
						shift++;
				}
			}
		}
	}
	else /* if (planes >= 8) */
	{
		register long fcol, bcol;

		if (r->res.clut)
		{
			fcol = (long)fgcol;
			bcol = (long)bgcol;
		}
		else
		{
			fcol = c->pixelvalues[fgcol];
			bcol = c->pixelvalues[bgcol];
		}

		xinc = Planes2xinc[planes - 8];

		addr = (unsigned char *)r->base + ((long)y1 * r->bypl) + ((long)x1 * xinc);

		if (dx >= dy)
		{
			e1	= dy * 2;
			eps	= -dx;
			e2	= dx * 2;

			for (loopcnt = dx; loopcnt >= 0; loopcnt--)
			{
				linemask = linemask << 1 | linemask >> 15;

				if (linemask & 1)
				{
					if (dlp_fg)
						(*dlp_fg)(addr, fcol); /**addr = color;*/
				}
				else if (dlp_bg)
					(*dlp_bg)(addr, bcol);

				addr += xinc;
				eps += e1;
				if (eps >= 0)
				{
					eps -= e2;
					addr += yinc;
				}
			}
		}
		else
		{
			e1	= dx * 2;
			eps	= -dy;
			e2	= dy * 2;

			for (loopcnt = dy; loopcnt >= 0; loopcnt--)
			{

				linemask = linemask << 1 | linemask >> 15;

				if (linemask & 1)
				{
					if (dlp_fg)
						(*dlp_fg)(addr, fcol); /* *addr = color;*/
				}
				else if (dlp_bg)
					(*dlp_bg)(addr, bcol);
				
				addr += yinc;
				eps += e1;
				if ( eps >= 0 )
				{
					eps -= e2;
					addr += xinc;
				}
			}
		}
	}
}

/*
 * habline - draw a horizontal line
 * Written by Odd Skancke
 */
void
habline (RASTER *r, COLINF *c, short x1, short x2, short y, PatAttr *ptrn)
{
	short x;
	short dx, xinc, bit;
	short planes, bypl;
	short bgcol, fgcol;
	unsigned short linemask;
	unsigned char *addr;
	int i;
	register pixel_blit dpf_fg;
	register pixel_blit dpf_bg;

	if (x2 > x1)
	{
		dx = x2 - x1 + 1;
		x  = x1;
	}
	else
	{
		dx = x1 - x2 + 1;
		x  = x2;
	}


	linemask = *(unsigned short *)ptrn->data;
	bit = x & 0xf;
	if (bit)
		linemask = (linemask << bit) | (linemask >> (16 - bit));

	planes	= ptrn->wrmode;

	if (planes == MD_ERASE)
		linemask = ~linemask;

	bgcol	= ptrn->bgcol[planes];
	fgcol	= ptrn->color[planes];
	planes <<= 1;
	dpf_fg	= r->drawers->dlp[planes];
	dpf_bg	= 0; //r->drawers->dlp[planes + 1];
	planes	= r->res.planes;
	bypl	= r->bypl;

	SYNC_RASTER(r);

	if (planes < 8)
	{
		int bitcount, shift;

		xinc = planes << 1;
		shift = x & 0xf;
		bitcount = 16 - shift;

		addr = (unsigned char *)r->base + (((long)x >> 4) * xinc) + ((long)y * r->bypl);

		for (i = 0; i < dx; i++)
		{
			if (linemask & 0x8000)
			{
				if (dpf_fg)
					(*dpf_fg)(addr, (long)shift << 16 | fgcol);
			}
			else if (dpf_bg)
				(*dpf_bg)(addr, (long)shift << 16 | bgcol);

			bitcount--;
			if (bitcount <= 0)
			{
				bitcount = 16;
				addr += xinc;
				shift = 0;
			}
			else
				shift++;

			linemask = linemask >> 15 | linemask << 1;
		}
	}
	else /* (planes >= 8) */
	{
		register long fcol, bcol;

		if (r->res.clut)
		{
			fcol = (long)fgcol;
			bcol = (long)bgcol;
		}
		else
		{
			fcol = c->pixelvalues[fgcol];
			bcol = c->pixelvalues[bgcol];
		}

		xinc = Planes2xinc[planes - 8];

		addr = (unsigned char *)r->base + ((long)x * xinc) + ((long)y * r->bypl);
		for (i = 0; i < dx; i++)
		{
			if (linemask & 0x8000)
			{
				if (dpf_fg)
					(*dpf_fg)(addr, fcol);
			}
			else if (dpf_bg)
				(*dpf_bg)(addr, bcol);

			addr += xinc;
			linemask = linemask >> 15 | linemask << 1;
		}

	}
}
/*
 * vabline - draw a vertical line
 * Written by Odd Skancke
 */
void
vabline (RASTER *r, COLINF *c, short y1, short y2, short x, PatAttr *ptrn)
{
	int i;
	short y;
	short dy, bypl, xinc;
	short planes, bgcol, fgcol;
	unsigned short linemask;
	unsigned char *addr;
	pixel_blit dpf_fg;
	pixel_blit dpf_bg;

	if (y2 > y1)
	{
		dy = y2 - y1 + 1;
		y  = y1;
	}
	else
	{
		dy = y1 - y2 + 1;
		y  = y2;
	}


	linemask = *(unsigned short *)ptrn->data;
#if 0
	bit = x & 0xf;
	if (bit)
		linemask = (linemask << bit) | (linemask >> (16 - bit));
#endif

	planes	= ptrn->wrmode;

	if (planes == MD_ERASE)
		linemask = ~linemask;

	bgcol	= ptrn->bgcol[planes];
	fgcol	= ptrn->color[planes];
	planes <<= 1;
	dpf_fg	= r->drawers->dlp[planes];
	dpf_bg	= 0; //r->drawers->dlp[planes + 1];
	planes	= r->res.planes;
	bypl	= r->bypl;

	SYNC_RASTER(r);

	if (planes < 8)
	{
		int shift;

		shift = x & 0xf;
		xinc = planes << 1;
		addr = (unsigned char *)r->base + (((long)x >> 4) * xinc) + ((long)y * r->bypl);

		for (i = 0; i < dy; i++)
		{
			linemask = linemask << 1 | linemask >> 15;
			if (linemask & 1)
			{
				if (dpf_fg)
					(*dpf_fg)(addr, (long)shift << 16 | fgcol);
			}
			else if (dpf_bg)
				(*dpf_bg)(addr, (long)shift << 16 | bgcol);

			addr += bypl;
		}
			

	}
	else /* (planes >= 8) */
	{
		register long fcol, bcol;

		xinc = Planes2xinc[planes - 8];

		if (r->res.clut)
		{
			fcol = (long)fgcol;
			bcol = (long)bgcol;
		}
		else
		{
			fcol = c->pixelvalues[fgcol];
			bcol = c->pixelvalues[bgcol];
		}

		addr = (unsigned char *)r->base + ((long)x * xinc) + ((long)y * r->bypl);

		for (i = 0; i < dy; i++)
		{
			if (linemask & 0x8000)
			{
				if (dpf_fg)
					(*dpf_fg)(addr, fcol);
			}
			else if (dpf_bg)
				(*dpf_bg)(addr, bcol);
			addr += bypl;
			linemask = linemask >> 15 | linemask << 1;
		}

	}
}

/* Taken from fVDI (line.c), adapted by Odd Skancke */
short
wide_setup(RASTER *r, short width, short *q_circle)
{
	int i, j;
	short x, y, low, high;
	short xsize, ysize;
	short num_qc_lines;
	long d;

	/* Limit the requested line width to a reasonable value. */

	if (width < 1)
		width = 1;
	else if (width > MAX_L_WIDTH)
		width = MAX_L_WIDTH;

	/* Make the line width an odd number (one less, if even). */

	width = (width - 1) | 1;

	/* Set the line width internals and the return parameters.  
	 * Return if the line width is being set to one.
	 */
	if (width == 1)
		return 0;

	/* Initialize the circle DDA.  "y" is set to the radius. */

	x = 0;
	y = (width + 1) >> 1; // / 2;
	d = 3 - 2 * y;

	for(i = 0; i < MAX_L_WIDTH; i++)
	{
		q_circle[i] = 0 ;
	}

	/* Do an octant, starting at north.  
	 * The values for the next octant (clockwise) will
	 * be filled by transposing x and y.
	 */
	while (x < y)
	{
		q_circle[y] = x;
		q_circle[x] = y;

		if (d < 0)
		{
			//d = d + (4 * x) + 6;
			d += ((long)x << 2) + 6;
		}
		else
		{
			//d = d + (4 * (x - y)) + 10;
			d += ((long)(x - y) << 2) + 10;
			y--;
		}
		x++;
	}

	if (x == y)
		q_circle[x] = x;

	 /* Calculate the number of vertical pixels required. */

	xsize = r->wpixel;
	ysize = r->hpixel;
	num_qc_lines = (((long)width * xsize / ysize) >> 1) + 1; // / 2 + 1;

	/* Fake a pixel averaging when converting to 
	 * non-1:1 aspect ratio.
	 */

	low = 0;
	for(i = 0; i < num_qc_lines; i++)
	{
		high = ((long)(2 * i + 1) * ysize / xsize) / 2;
		d = 0;

		for (j = low; j <= high; j++)
		{
			d += q_circle[j];
		}

		q_circle[i] = d / (high - low + 1);
		low = high + 1;
	}

	return num_qc_lines;
}

/* Taken from fVDI (line.c), adapted by Odd Skancke*/
void
quad_xform(short quad, short x, short y, short *tx, short *ty)
{
	if (quad & 2)
		*tx = -x;		/* 2, 3 */
	else
		*tx = x;		/* 1, 4 */

	if (quad > 2)
		*ty = -y;		/* 3, 4 */
	else
		*ty = y;		/* 1, 2 */
}

/* Taken from fVDI, adapted by Odd Skancke */
void
perp_off(short *vx, short *vy, short *q_circle, short num_qc_lines)
{
	/*int x, y, u, v, quad, magnitude, min_val, x_val, y_val;*/
	short x, y, u, v, x_val, y_val;
	short quad;
	//int min_val, magnitude;
	long min_val, magnitude;

	/* Mirror transform the vector so that it is in the first quadrant. */
	if (*vx >= 0)
		quad = (*vy >= 0) ? 1 : 4;
	else
		quad = (*vy >= 0) ? 2 : 3;

	quad_xform(quad, *vx, *vy, &x, &y);

	/* Traverse the circle in a dda-like manner and find the coordinate pair
	 * (u, v) such that the magnitude of (u*y - v*x) is minimized.  In case of
	 * a tie, choose the value which causes (u - v) to be minimized.  If not
	 * possible, do something.
	 */
	min_val = 32767L;
	x_val = u = q_circle[0];		/* x_val/y_val new here */
	y_val = v = 0;
	while (1)
	{
		/* Check for new minimum, same minimum, or finished. */
		if (((magnitude = ABS((long)u * y - (long)v * x)) < min_val ) ||
		    ((magnitude == min_val) && (ABS(x_val - y_val) > ABS(u - v))))
		{
			min_val = magnitude;
			x_val = u;
			y_val = v;
		}
		else
			break;

		/* Step to the next pixel. */
		if (v == num_qc_lines - 1)
		{
			if (u == 1)
				break;
			else
				u--;
		}
		else
		{
			if (q_circle[v + 1] >= u - 1)
			{
				v++;
				u = q_circle[v];
			}
			else
			{
				u--;
			}
		}
	}

	/* Transform the solution according to the quadrant. */
	quad_xform(quad, x_val, y_val, vx, vy);
}

/* Taken from fVDI (line.c), adapted by Odd Skancke*/
void
arrow(RASTER *r, COLINF *c, short *xy, short inc, short numpts, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn)
{
	int i;
	short arrow_len, arrow_wid, line_len;
	short *xybeg;
	short dx, dy;
	short base_x, base_y, ht_x, ht_y;
	long arrow_len2, line_len2;
	short xsize, ysize;
	short polygon[6];

	xsize = r->wpixel;
	ysize = r->hpixel;

	/* Set up the arrow-head length and width as a function of line width. */

	arrow_len = ptrn->t.l.width == 1 ? 8 : 3 * ptrn->t.l.width - 1;

	arrow_len2 = arrow_len * arrow_len;
	arrow_wid = arrow_len / 2;

	/* Initialize the beginning pointer. */
	xybeg = xy;

	/* Find the first point which is not so close to the end point that it
	 * will be obscured by the arrowhead.
	 */
	for(i = 1; i < numpts; i++)
	{
		/* Find the deltas between the next point and the end point.
		 * Transform to a space such that the aspect ratio is uniform
		 * and the x axis distance is preserved.
		 */
		xybeg += inc;
		dx = *xy - *xybeg;
		dy = SMUL_DIV(*(xy + 1) - *(xybeg + 1), ysize, xsize);

		/* Get the length of the vector connecting the point with the end point.
		 * If the vector is of sufficient length, the search is over.
		 */
		line_len2 = (long)dx * dx + (long)dy * dy;
		if (line_len2 >= arrow_len2)
			break;
	}

	/* If the longest vector is insufficiently long, don't draw an arrow. */
	if (line_len2 < arrow_len2)
		return;

	line_len = isqrt(line_len2);
	
	/* Rotate the arrow-head height and base vectors.
	 * Perform calculations in 1000x space.
	 */
	ht_x = SMUL_DIV(arrow_len, SMUL_DIV(dx, 1000, line_len), 1000);
	ht_y = SMUL_DIV(arrow_len, SMUL_DIV(dy, 1000, line_len), 1000);
	base_x = SMUL_DIV(arrow_wid, SMUL_DIV(dy, -1000, line_len), 1000);
	base_y = SMUL_DIV(arrow_wid, SMUL_DIV(dx, 1000, line_len), 1000);

	/* Transform the y offsets back to the correct aspect ratio space. */
	ht_y = SMUL_DIV(ht_y, xsize, ysize);
	base_y = SMUL_DIV(base_y, xsize, ysize);

	/* Build a polygon to send to plygn.  Build into a local array
	 * first since xy will probably be pointing to the PTSIN array.
	 */

	polygon[0] = *xy + base_x - ht_x;
	polygon[1] = *(xy + 1) + base_y - ht_y;
	polygon[2] = *xy - base_x - ht_x;
	polygon[3] = *(xy + 1) - base_y - ht_y;
	polygon[4] = *xy;
	polygon[5] = *(xy + 1);
	DRAW_FILLEDPOLY(r, c, (short *)&polygon, 3, clip, points, pointasize, ptrn);

	/* Adjust the end point and all points skipped. */
	*xy -= ht_x;
	*(xy + 1) -= ht_y;
	while ((xybeg -= inc) != xy)
	{
		*xybeg = *xy;
		*(xybeg + 1) = *(xy + 1);
	}
}

/* Taken from fVDI (line.c), adapted by Odd Skancke*/
void
do_arrow(RASTER *r, COLINF *c, short *pts, short numpts, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn)
{
	short x_start, y_start, new_x_start, new_y_start;

	/* Function "arrow" will alter the end of the line segment.
	 * Save the starting point of the polyline in case two calls to "arrow"
	 * are necessary.
	 */
	new_x_start = x_start = pts[0];
	new_y_start = y_start = pts[1];

	if (ptrn->t.l.beg & LE_ARROW)
	{
		arrow(r, c, &pts[0], 2, numpts, clip, points, pointasize, ptrn);
		new_x_start = pts[0];
		new_y_start = pts[1];
	}

	if (ptrn->t.l.end & LE_ARROW)
	{
		pts[0] = x_start;
		pts[1] = y_start;
		arrow(r, c, &pts[2 * numpts - 2], -2, numpts, clip, points, pointasize, ptrn);
		pts[0] = new_x_start;
		pts[1] = new_y_start;
	}
}

/* Taken from fVDI (line.c), modified by Odd Skancke*/
void
wide_line(RASTER *r, COLINF *c, short *pts, long numpts, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn)
{
	int i, j, k;
	short wx1, wy1, wx2, wy2, vx, vy;
	short *q_circle, *misc;
	short num_qc_lines;
	short xsize, ysize;
//	int polygon[8];
//	int q_circleb[MAX_L_WIDTH];

	/* Don't attempt wide lining on a degenerate polyline. */
	if (numpts < 2)
		return;

	q_circle = points;
	points += MAX_L_WIDTH;
	pointasize -= ((MAX_L_WIDTH * sizeof(short)) + 16);
//	q_circle = (int *)&q_circleb;

	num_qc_lines = wide_setup(r, ptrn->t.l.width, q_circle);

#if 1
	/* If the ends are arrowed, output them. */
	if ((ptrn->t.l.beg | ptrn->t.l.end) & LE_ARROW)
		do_arrow(r, c, pts, numpts, clip, points, pointasize, ptrn);
#endif

	/* Initialize the starting point for the loop. */
	j = 0;
	wx1 = pts[j++];
	wy1 = pts[j++];

	/* Loop over the number of points passed in. */
	for(i = 1; i < numpts; i++)
	{
		/* Get the ending point for the line segment and the vector from the
		 * start to the end of the segment.
		 */
		wx2 = pts[j++];
		wy2 = pts[j++];   

		vx = wx2 - wx1;
		vy = wy2 - wy1;

		/* Ignore lines of zero length. */
		if ((vx == 0) && (vy == 0))
			continue;

		/* Calculate offsets to fatten the line.  If the line segment is
		 * horizontal or vertical, do it the simple way.
		 */
		if (vx == 0)
		{
			vx = q_circle[0];
			vy = 0;
		}
		else if (vy == 0)
		{
			vx = 0;
			vy = num_qc_lines - 1;
		}
		else
		{
			/* Find the offsets in x and y for a point perpendicular to the line
			 * segment at the appropriate distance.
			 */

			xsize = r->wpixel;
			ysize = r->hpixel;

			k = SMUL_DIV(-vy, ysize, xsize);
			vy = SMUL_DIV(vx, xsize, ysize);
			vx = k;
			perp_off(&vx, &vy, q_circle, num_qc_lines);
		}

		/* Prepare the points parameters for the polygon call. */
		misc = points; //(int *)&polygon; //points;

		*misc++ = wx1 + vx;
		*misc++ = wy1 + vy;
		*misc++ = wx1 - vx;
		*misc++ = wy1 - vy;
		*misc++ = wx2 - vx;
		*misc++ = wy2 - vy;
		*misc++ = wx2 + vx;
		*misc++ = wy2 + vy;

		DRAW_FILLEDPOLY(r, c, points/*(int *)&polygon*/, 4, clip, misc /*points*/, pointasize, ptrn);

		/* The line segment end point becomes the starting point for the next
		 * line segment.
		 */
		wx1 = wx2;
		wy1 = wy2;
	}
}

/* pmarker taken from fVDI (line.c), modified by Odd Skancke */
void
pmarker( RASTER *r, COLINF *c, POINT *center, VDIRECT *clip, short type, short size, short w_in, short h_in, PatAttr *ptrn)
{
	Fabline dl;
	short i, j;
	short x_center, y_center;
	short num_lines, num_points;
	signed char *m_ptr;
	short w, h;
	signed char nwidth[5], width[5], nheight[5], height[5];
	short tmp;
	short x1, y1, x2, y2;

	dl = DRAW_ABLINE_PTR(r); //r->drawers->draw_abline;

	for(i = 0; i <= 4; i++)
	{
		if (!w_in)
		{
#if 0
			tmp = (short)((short)(((short)size * 30 + 11) / 22) * i * 4 + 15) / 30 + 1;
#else
			tmp = (short)((short)size * i * 4 + 11) / 22 + 1;
#endif
		}
		else
			tmp = ((short)w_in * i + 2) / 4;
		nwidth[i] = -(tmp / 2);
		width[i] = tmp + nwidth[i] - 1;
		if (!h_in)
			tmp = (short)((short)size * i * 4 + 11) / 22 + 1;
		else
			tmp = (short)((short)h_in * i * 2 + 3) / 6;
		nheight[i] = -(tmp / 2);
		height[i] = tmp + nheight[i] - 1;
	}

	w = width[4] - nwidth[4] + 1;
	h = height[3] - nheight[3] + 1;
	x_center = w / 2;
	y_center = h / 2;

	m_ptr = marker[type];
	num_lines = *m_ptr++;
	x1 = y1 = 0;    /* To make the compiler happy */
	for(i = 0; i < num_lines; i++)
	{
		num_points = *m_ptr++;
		for(j = 0; j < num_points; j++)
		{
			x2 = *m_ptr++;
			y2 = *m_ptr++;

			if (x2 <= 0)
				x2 = nwidth[-x2] + x_center;
			else
				x2 = width[x2] + x_center;
			if (y2 <= 0)
				y2 = nheight[-y2] + y_center;
			else
				y2 = height[y2] + y_center;

			if (j > 0)
			{
				short *line;
				short coords[4];

				line = (short *)&coords;
				*line++ = x1 + center->x, *line++ = y1 + center->y;
				*line++ = x2 + center->x, *line   = y2 + center->y;

				if (clip_line((VDIRECT *)&coords, clip))
					(*dl)( r, c, (VDIRECT *)&coords, ptrn);
			}
			x1 = x2;
			y1 = y2;
		}
	}
}

/* Writen by Odd Skancke */
void
draw_spans(RASTER *r, COLINF *c, short x1, short x2, short y, PatAttr *ptrn)
{
	short x;
	short dx, xinc;
	short planes, bypl;
	short bgcol, fgcol;
	short wrmode;
	unsigned short pattern, bit;
	short i, j;
	pixel_blit dpf_fg;
	pixel_blit dpf_bg;

	if (x1 > x2)
	{
		x = x1;
		x1 = x2;
		x2 = x;
	}

	x = x1;
	dx = x2 - x1 + 1;

	wrmode = ptrn->wrmode;
	fgcol = ptrn->color[wrmode];
	bgcol	= ptrn->bgcol[wrmode];

	wrmode <<= 1;
	dpf_fg	= r->drawers->dlp[wrmode];
	dpf_bg	= r->drawers->dlp[wrmode + 1];
	planes	= r->res.planes;
	bypl	= r->bypl;

	SYNC_RASTER(r);

	if (planes < 8)
	{
		unsigned char *addr;
		unsigned char *patrn;
		int left, right, groups, bitcount, shift;

		xinc	= planes << 1;
		shift	= x1 & 0xf;
		bitcount = 16 - shift;

		addr = (unsigned char *)r->base + ((long)(x >> 4) * xinc) + ((long)y * r->bypl);

		patrn = (unsigned char *)(long)ptrn->data + ((y % ptrn->height)/*(y % ptrn->height)*/ * (ptrn->wwidth << 1));

		left = 16 - (x1 & 0xf);
		dx -= left;

		if ( dx <= 0 )
		{
			left = left + dx;
			groups = 0;
			right = 0;
		}
		else if (dx > 15)
		{
			right = (x2 & 0xf) + 1;
			groups = (dx - right) >> 4;
		}
		else
		{
			groups = 0;
			right = dx;
		}

		if (left)
		{
			pattern = x1 & 0xf ? (*(unsigned short *)patrn << ((x1 & 0xf))) | ( *(unsigned short *)patrn >> (16 - (x1 & 0xf)) ) : *(unsigned short *)patrn;

			for (i = 0; i < left; i++)
			{
				if (pattern & 0x8000)
				{
					if (dpf_fg)
						(*dpf_fg)(addr, (long)shift << 16 | fgcol);
				}
				else if (dpf_bg)
					(*dpf_bg)(addr, (long)shift << 16 | bgcol);

				bitcount--;

				if (bitcount <= 0)
				{
					bitcount = 16;
					addr += xinc;
					shift = 0;
				}
				else
					shift++;

				pattern <<= 1; //pattern >> 15 | pattern << 1;
			}
		}

		if (groups)
		{
			bit = *(unsigned short *)patrn;

			for (i = 0; i < groups; i++)
			{
				pattern = bit;
				for (j = 0; j < 16; j++)
				{
					//pattern = pattern >> 15 | pattern <<1;
					if (pattern & 0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(addr, (long)shift << 16 | fgcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(addr, (long)shift << 16 | bgcol);

					bitcount--;

					if (bitcount <= 0)
					{
						bitcount = 16;
						addr += xinc;
						shift = 0;
					}
					else
						shift++;

					pattern <<= 1;
				}
			}
		}
		if (right)
		{
			pattern = *(unsigned short *)patrn;
			for (i = 0; i < right; i++)
			{
				if (pattern & 0x8000)
				{
					if (dpf_fg)
						(*dpf_fg)(addr, (long)shift << 16 | fgcol);
				}
				else if (dpf_bg)
					(*dpf_bg)(addr, (long)shift << 16 | bgcol);

				bitcount--;

				if (bitcount <= 0)
				{
					bitcount = 16;
					addr += xinc;
					shift = 0;
				}
				else
					shift++;

				pattern <<= 1;
			}
		}	
	}
	else /* (planes >= 8) */
	{
		unsigned char *addr, *patrn;
		//int xind, pw;

		xinc = Planes2xinc[planes - 8];
		addr = (unsigned char *)r->base + ((long)x * xinc) + ((long)y * r->bypl);

#if 0
		if (ptrn->expanded)
		{
			pw = ptrn->width;
			xind = x1 % pw;
			patrn = (unsigned char *)ptrn->exp_data + ((y % ptrn->height) * (pw * xinc));

			for (i = 0; i < dx; i++)
			{

				fgcol = *(unsigned char *)(patrn + (xind * xinc));

				xind++;

				if (xind >= pw)
					xind = 0;

				if (dpf_fg)
					(*dpf_fg)(addr, (long)fgcol);
#if 0
				if (pattern & 1)
				{
					if (dpf_fg)
						(*dpf_fg)(addr, (long)fgcol);
				}
				else if (dpf_bg)
					(*dpf_bg)(addr, (long)bgcol);
#endif 0

				addr += xinc;
			}
		}
		else
#endif
		{
			int left, right, groups;
			register long fcol, bcol;

			if (r->res.clut)
			{
				fcol = (long)fgcol;
				bcol = (long)bgcol;
			}
			else
			{
				fcol = c->pixelvalues[fgcol];
				bcol = c->pixelvalues[bgcol];
			}

			patrn = (unsigned char *)(long)ptrn->data + ((y % ptrn->height)/*(y % ptrn->height)*/ * (ptrn->wwidth << 1));

			left = 16 - (x1 & 0xf);
			dx -= left;

			if ( dx <= 0 )
			{
				left = left + dx;
				groups = 0;
				right = 0;
			}
			else if (dx > 15)
			{
				right = (x2 & 0xf) + 1;
				groups = (dx - right) >> 4;
			}
			else
			{
				groups = 0;
				right = dx;
			}

			if (left)
			{
				pattern = x1 & 0xf ? (*(unsigned short *)patrn << ((x1 & 0xf))) | ( *(unsigned short *)patrn >> (16 - (x1 & 0xf)) ) : *(unsigned short *)patrn;

				for (i = 0; i < left; i++)
				{
					if (pattern & 0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(addr, fcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(addr, bcol);
					addr += xinc;
					pattern <<= 1; //pattern >> 15 | pattern << 1;
				}
			}

			if (groups)
			{
				bit = *(unsigned short *)patrn;

				for (i = 0; i < groups; i++)
				{
					pattern = bit;
					for (j = 0; j < 16; j++)
					{
						//pattern = pattern >> 15 | pattern <<1;
						if (pattern & 0x8000)
						{
							if (dpf_fg)
								(*dpf_fg)(addr, fcol);
						}
						else if (dpf_bg)
							(*dpf_bg)(addr, bcol);
						addr += xinc;
						pattern <<= 1;
					}
				}
			}
			if (right)
			{
				pattern = *(unsigned short *)patrn;
				for (i = 0; i < right; i++)
				{
					if (pattern & 0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(addr, fcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(addr, bcol);
					addr += xinc;
					pattern <<= 1;
				}
			}	
		}
	}
}
/* Writen by Odd Skancke */
void
draw_mspans(RASTER *r, COLINF *c, short x1, short x2, short y1, short y2, PatAttr *ptrn)
{
	short x;
	int dx, xinc, dy;
	int planes, bypl;
	int bgcol, fgcol;
	int wrmode;
	unsigned short pattern, bit;
	int i, j;
	pixel_blit dpf_fg;
	pixel_blit dpf_bg;

	if (x1 > x2)
	{
		x = x1;
		x1 = x2;
		x2 = x;
	}

	x = x1;
	dx = x2 - x1 + 1;
	dy = y2 - y1 + 1;

	wrmode = ptrn->wrmode;
	fgcol = ptrn->color[wrmode];
	bgcol	= ptrn->bgcol[wrmode];

	wrmode <<= 1;
	dpf_fg	= r->drawers->dlp[wrmode];
	dpf_bg	= r->drawers->dlp[wrmode + 1];
	planes	= r->res.planes;
	bypl	= r->bypl;

	SYNC_RASTER(r);

	if (planes < 8)
	{
		unsigned char *addr, *a;
		unsigned char *patrn;
		int left, right, groups, shft, bc, shift, bitcount;

		xinc = planes << 1;
		a = (unsigned char *)r->base + ((long)(x >> 4) * xinc) + ((long)y1 * r->bypl);

		left = 16 - (x1 & 0xf);
		dx -= left;

		shft = x1 & 0xf;
		bc = 16 - shft;

		if ( dx <= 0 )
		{
			left = left + dx;
			groups = 0;
			right = 0;
		}
		else if (dx > 15)
		{
			right = (x2 & 0xf) + 1;
			groups = (dx - right) >> 4;
		}
		else
		{
			groups = 0;
			right = dx;
		}

		for (; dy > 0; dy--)
		{

			patrn = (unsigned char *)(long)ptrn->data + ((y1 % ptrn->height) * (ptrn->wwidth << 1));
			addr = a;
			shift = shft;
			bitcount = bc;

			if (left)
			{
				pattern = x1 & 0xf ? (*(unsigned short *)patrn << ((x1 & 0xf))) | ( *(unsigned short *)patrn >> (16 - (x1 & 0xf)) ) : *(unsigned short *)patrn;

				for (i = 0; i < left; i++)
				{
					if (pattern & 0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(addr, (long)shift << 16 | fgcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(addr, (long)shift << 16 | bgcol);

					bitcount--;

					if (bitcount <= 0)
					{
						bitcount = 16;
						addr += xinc;
						shift = 0;
					}
					else
						shift++;

					pattern <<= 1;
				}
			}

			if (groups)
			{
				bit = *(unsigned short *)patrn;
				for (i = 0; i < groups; i++)
				{
					pattern = bit;
					for (j = 0; j < 16; j++)
					{
						if (pattern & 0x8000)
						{
							if (dpf_fg)
								(*dpf_fg)(addr, (long)shift << 16 | fgcol);
						}
						else if (dpf_bg)
							(*dpf_bg)(addr, (long)shift << 16 | bgcol);
					bitcount--;

					if (bitcount <= 0)
					{
						bitcount = 16;
						addr += xinc;
						shift = 0;
					}
					else
						shift++;

						pattern <<= 1;
					}
				}
			}
			if (right)
			{
				pattern = *(unsigned short *)patrn;
				for (i = 0; i < right; i++)
				{
					if (pattern & 0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(addr, (long)shift << 16 | fgcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(addr, (long)shift << 16 | bgcol);

					bitcount--;

					if (bitcount <= 0)
					{
						bitcount = 16;
						addr += xinc;
						shift = 0;
					}
					else
						shift++;

					pattern <<= 1;
				}
			}
			y1++;
			a += bypl;
		}
	}
	else /* (planes >= 8) */
	{
		unsigned char *addr, *patrn, *a;
		//int xind, pw;

		xinc = Planes2xinc[planes - 8];
		a = (unsigned char *)r->base + ((long)x * xinc) + ((long)y1 * r->bypl);

#if 0
		if (ptrn->expanded)
		{
			addr = a;
			pw = ptrn->width;
			xind = x1 % pw;
			patrn = (unsigned char *)ptrn->exp_data + ((y1 % ptrn->height) * (pw * xinc));

			for (i = 0; i < dx; i++)
			{

				fgcol = *(unsigned char *)(patrn + (xind * xinc));

				xind++;

				if (xind >= pw)
					xind = 0;

				if (dpf_fg)
					(*dpf_fg)(addr, (long)fgcol);
#if 0
				if (pattern & 1)
				{
					if (dpf_fg)
						(*dpf_fg)(addr, (long)fgcol);
				}
				else if (dpf_bg)
					(*dpf_bg)(addr, (long)bgcol);
#endif 0

				addr += xinc;
			}
		}
		else
#endif
		{
			int left, right, groups;
			register long fcol, bcol;

			if (r->res.clut)
			{
				fcol = (long)fgcol;
				bcol = (long)bgcol;
			}
			else
			{
				fcol = c->pixelvalues[fgcol];
				bcol = c->pixelvalues[bgcol];
			}

			left = 16 - (x1 & 0xf);
			dx -= left;

			if ( dx <= 0 )
			{
				left = left + dx;
				groups = 0;
				right = 0;
			}
			else if (dx > 15)
			{
				right = (x2 & 0xf) + 1;
				groups = (dx - right) >> 4;
			}
			else
			{
				groups = 0;
				right = dx;
			}

			for (; dy > 0; dy--)
			{
				patrn = (unsigned char *)(long)ptrn->data + ((y1 % ptrn->height) * (ptrn->wwidth << 1));
				addr = a;

				if (left)
				{
					pattern = x1 & 0xf ? (*(unsigned short *)patrn << ((x1 & 0xf))) | ( *(unsigned short *)patrn >> (16 - (x1 & 0xf)) ) : *(unsigned short *)patrn;

					for (i = 0; i < left; i++)
					{
						if (pattern & 0x8000)
						{
							if (dpf_fg)
								(*dpf_fg)(addr, fcol);
						}
						else if (dpf_bg)
							(*dpf_bg)(addr, bcol);
						addr += xinc;
						pattern <<= 1;
					}
				}

				if (groups)
				{
					bit = *(unsigned short *)patrn;

					for (i = 0; i < groups; i++)
					{
						pattern = bit;
						for (j = 0; j < 16; j++)
						{
							if (pattern & 0x8000)
							{
								if (dpf_fg)
									(*dpf_fg)(addr, fcol);
							}
							else if (dpf_bg)
								(*dpf_bg)(addr, bcol);
							addr += xinc;
							pattern <<= 1;
						}
					}
				}
				if (right)
				{
					pattern = *(unsigned short *)patrn;
					for (i = 0; i < right; i++)
					{
						if (pattern & 0x8000)
						{
							if (dpf_fg)
								(*dpf_fg)(addr, fcol);
						}
						else if (dpf_bg)
							(*dpf_bg)(addr, bcol);
						addr += xinc;
						pattern <<= 1;
					}
				}
				y1++;
				a += bypl;
			}
		}
	}
}

