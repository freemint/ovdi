#include "memory.h"
#include "expand.h"
#include "ovdi_defs.h"
#include "drawing.h"

#include "4b_generic.h"

/* data tables needed - in 4b_data.c */
extern long col2long4b[];
extern unsigned short fillbuff4b[];
extern unsigned short maskbuff4b[];
extern unsigned short shifts4b[];

void
fill_16x_4b(RASTER *r, COLINF *c, short *corners, PatAttr *ptrn)
{
	int height, y, wrmode;
	struct fill16x_api f;
	unsigned long slp[2];
	unsigned short slm;
	//unsigned long lp0, lp1, lsm, lem, lmask;
	//unsigned short *m;
	//unsigned long *s, *d;

	/*
	 * check if pattern is expanded and do expand it if it isnt
	*/
	wrmode = ptrn->wrmode;

	if (ptrn->expanded != 4 && ptrn->interior > FIS_SOLID)
	{
		//short color;
		//unsigned short data, p0, p1, p2, p3;
		short col[2];

		/*
		 * If there is no pointer to expanded data buffer,
		 * we gotta use a static one. Then we also need to
		 * to make sure pattern does not exceed 16 x 16. We
		 * also do not set the expanded flag so that the
		 * exp_data and mask pointers are cleared upon returning.
		*/
		if (!ptrn->exp_data)
		{
			if (ptrn->width > 16 || ptrn->height > 16)
				return;
			(long)ptrn->exp_data = (long)&fillbuff4b;
			(long)ptrn->mask = (long)&maskbuff4b;
			ptrn->expanded = 0;
		}
		else
			ptrn->expanded = r->res.planes;
#if 1
		col[1] = ptrn->color[wrmode] & 0xff;
		col[0] = ptrn->bgcol[wrmode] & 0xff;

		expand( ptrn->width, ptrn->height,
			ptrn->planes, PF_ATARI, ptrn->data,
			4, PF_ATARI, ptrn->exp_data, (short *)&col, ptrn->mask);
#else
		s = (unsigned long *)ptrn->data;
		d = (unsigned long *)ptrn->exp_data;
		m = ptrn->mask;
		height = ptrn->height;

		if (interior == FIS_HOLLOW)
		{
			col[1] = 0;
			col[0] = 0;
		}
		else
		{
			col[1] = ptrn->color[wrmode];
			col[0] = ptrn->bgcol[wrmode];
		}

		/*
		 * do the expansion
		*/
		for (; height > 0; height--)
		{
			p0 = p1 = p2 = p3 = 0;
			data = *(unsigned short *)((unsigned short *)s)++;
			*m++ = data;
			for (i = 0; i < 16; i++)
			{
				if (data & 1)
					color = col[1];
				else
					color = col[0];
				p0 <<= 1; p0 |= (color & 1), color >>= 1;
				p1 <<= 1, p1 |= (color & 1), color >>= 1;
				p2 <<= 1, p2 |= (color & 1), color >>= 1;
				p3 <<= 1, p3 |= (color & 1);
				data >>= 1;
			}
			*d++ = ((long)p0 << 16) | p1, *d++ = ((long)p2 << 16) | p3;
		}
#endif
	}

	f.dbpl = r->bypl;

	if (ptrn->interior <= FIS_SOLID)
	{
		if (ptrn->interior == FIS_HOLLOW)
			slp[0] = slp[1] = slm = wrmode = 0;
		else {
			short fc = ptrn->color[wrmode] << 1;
			slp[0] = col2long4b[fc++];
			slp[1] = col2long4b[fc];
			slm	= 0xffff;
		}
		f.s = &slp;
		f.m = &slm;
		goto singleline;
	}
	if (ptrn->height == 1)
	{
		f.s = ptrn->exp_data;
		f.m = ptrn->mask;
		goto singleline;
	}

	switch (wrmode)
	{
		case 0: f.drawspan = ds_REPLACE_4b;	break;
		case 1: f.drawspan = ds_TRANS_4b;	break;
		case 2: f.drawspan = ds_XOR_4b;		break;
		case 3: f.drawspan = ds_ERASE_4b;	break;
		default: return;
	}
	{
		register short y1, x1, x2;
		register int sb;

		x1	= *corners++;
		y1	= *corners++;
		x2	= *corners++;
		height	= (*corners - y1) + 1;

		sb	= x1 & 0xf;

		f.sm	= ((long *)shifts4b)[sb];
		f.em	= ~((long *)shifts4b)[(x2 & 0xf) + 1];

		y	= y1 % ptrn->height;
		f.words	= ((sb + (x2 - x1) + 16) >> 4) - 1;
		x1	>>= 4;
		x1	<<= 3;
		f.d	= (char *)r->base + x1;
		(char *)f.d += (long)y1 * f.dbpl;
		f.m	= (short *)ptrn->mask + y;
		f.s	= (char *)ptrn->exp_data + (y << 3);
	}

	for (; height > 0; height--)
	{
		(*f.drawspan)(&f);

		if ((++y) >= ptrn->height)
		{
			y = 0;
			f.s = ptrn->exp_data;
			f.m = ptrn->mask;
		}
		else
		{
			(long *)f.s += 2;
			(short *)f.m += 1;
		}
		(char *)f.d += f.dbpl;
	}
	goto done;
singleline:
	switch (wrmode)
	{
		case 0: f.drawspan = ds_REPLACE_4b;	break;
		case 1: f.drawspan = ds_TRANS_4b;	break;
		case 2: f.drawspan = ds_XOR_4b;		break;
		case 3: f.drawspan = ds_ERASE_4b;	break;
		default: return;
	}
	{
		register short y1, x1, x2;
		register int sb;

		x1	= *corners++;
		y1	= *corners++;
		x2	= *corners++;
		height	= (*corners - y1) + 1;

		sb	= x1 & 0xf;

		f.sm	= ((long *)shifts4b)[sb];
		f.em	= ~((long *)shifts4b)[(x2 & 0xf) + 1];

		f.words	= ((sb + (x2 - x1) + 16) >> 4) - 1;
		x1	>>= 4;
		x1	<<= 3;
		f.d	= (char *)r->base + x1;
		(char *)f.d += (long)y1 * f.dbpl;
	}
	for (; height > 0; height--)
	{
		(*f.drawspan)(&f);
		(char *)f.d += f.dbpl;
	}
done:
	if (!ptrn->expanded)
	{
		ptrn->exp_data = 0;
		ptrn->mask = 0;
	}
}		
