#include "display.h"
#include "line.h"
#include "math.h"
#include "ovdi_defs.h"
#include "polygon.h"

void
filled_poly(RASTER *r, COLINF *c, short *pts, short n, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn)
{
	int i, j;
	short tmp, y;
	short miny, maxy;
	short x1, y1;
	short x2, y2;
	short ints;
	long spans;
	long max_spans;
	short *coords;

	if (!n)
		return;

	max_spans = ((pointasize / 5)/2) - 1;

	if ((pts[0] == pts[(n << 1) - 2]) && (pts[1] == pts[(n << 1) - 1]))
		n--;

	/* Scan through coordinates, finding max and min y coordinate */
	/* This will find the number of scanlines used by the polygon */

	miny = maxy = pts[1];
	coords = &pts[3];

	for(i = 1; i < n; i++)
	{
		y = *coords;
		coords += 2;		/* Skip to next y */

		if (y < miny)
			miny = y;

		if (y > maxy)
			maxy = y;

	}

	if (miny < clip->y1)
		miny = clip->y1;
	if (maxy > clip->y2)
		maxy = clip->y2;

	spans = 0;
	coords = &points[n];

	for(y = miny; y <= maxy; y++)
	{
		ints = 0;
		x1 = pts[(n << 1) - 2]; //p[n - 1][0];
		y1 = pts[(n << 1) - 1]; //p[n - 1][1];

		for(i = 0; i < n; i++)
		{
			x2 = pts[i << 1];	//p[i][0];
			y2 = pts[(i << 1) + 1]; //p[i][1];

			if (y1 < y2)
			{
				if ((y >= y1) && (y < y2))
				{
					points[ints++] = SMUL_DIV((y - y1), (x2 - x1), (y2 - y1)) + x1;
				}
			}
			else if (y1 > y2)
			{
				if ((y >= y2) && (y < y1))
				{
					points[ints++] = SMUL_DIV((y - y2), (x1 - x2), (y1 - y2)) + x2;
				}
			}
			x1 = x2;
			y1 = y2;
		}

		for(i = 0; i < ints - 1; i++)
		{
			for(j = i + 1; j < ints; j++)
			{
				if (points[i] > points[j])
				{
					tmp = points[i];
					points[i] = points[j];
					points[j] = tmp;
				}
			}
		}

#if 1
		if (spans > max_spans)
		{			/* Should really check against size of points array! */
			for (i = n; i < (n+(spans*3)); i += 3)
				draw_spans(r, c, points[i+1], points[i+2], points[i], ptrn);

			spans = 0;
			coords = &points[n];
		}
#endif

		x1 = clip->x1;
		x2 = clip->x2;

		for(i = 0; i < ints - 1; i += 2)
		{
			short x_1, x_2;

			x_1 = points[i];
			x_2 = points[i + 1];

			if (x_1 < x1)
				x_1 = x1;
			if (x_2 > x2)
				x_2 = x2;

			if (x_1 <= x_2)
			{
				*coords++ = y;
				*coords++ = x_1;
				*coords++ = x_2;
				spans++;
			}
		}
	}
#if 1
	if (spans)
	{
		for (i = n; i < (n+(spans*3)); i += 3)
			draw_spans(r, c, points[i+1], points[i+2], points[i], ptrn);
	}
#endif
}

