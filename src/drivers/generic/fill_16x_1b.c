#include "memory.h"
#include "expand.h"
#include "ovdi_defs.h"
#include "drawing.h"

#include "1b_generic.h"

/* data tables needed - in 4b_data.c */
extern unsigned short fillbuff1b[];
extern unsigned short maskbuff1b[];
extern unsigned short shifts1b[];

void _cdecl
fill_16x_1b(RASTER *r, COLINF *c, short *corners, PatAttr *ptrn)
{
	short y;
	int height, wrmode;
	struct fill16x_api f;
	unsigned short slp;
	unsigned short slm;
	/*
	 * check if pattern is expanded and do expand it if it isnt
	*/
	wrmode = ptrn->wrmode;

	if (ptrn->expanded != 1 && ptrn->interior > FIS_SOLID)
	{
		int i;
		unsigned short data, p0;
		unsigned short *m, *s, *d;
		short fc, bc;

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
			(long)ptrn->exp_data = (long)&fillbuff1b;
			(long)ptrn->mask = (long)&maskbuff1b;
			ptrn->expanded = 0;
		}
		else
			ptrn->expanded = r->resfmt.planes;
			
		s = (unsigned short *)ptrn->data;
		m = (unsigned short *)ptrn->mask;
		d = (unsigned short *)ptrn->exp_data;

		fc = ptrn->color[wrmode] & 1;
		bc = ptrn->bgcol[wrmode] & 1;
		height = ptrn->height;
		for (; height > 0; height--)
		{
			data = *s;
			p0 = 0;
			for (i = 0; i < 16; i++)
			{
				p0 <<= 1;
				if (data & 0x8000)
					p0 |= fc;
				else
					p0 |= bc;
				data <<= 1;
			}
			*m++ = *s++;
			*d++ = p0;
		}
	}		

	f.dbpl = r->bypl;

	if (ptrn->interior <= FIS_SOLID)
	{
		if (ptrn->interior == FIS_HOLLOW)
			slp = slm = wrmode = 0;
		else {
			slp = ptrn->color[wrmode] == 0 ? 0 : 0xffff;
			slm = 0xffff;
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
		case 0: f.drawspan = ds_REPLACE_1b;	break;
		case 1: f.drawspan = ds_TRANS_1b;	break;
		case 2: f.drawspan = ds_XOR_1b;		break;
		case 3: f.drawspan = ds_ERASE_1b;	break;
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

		f.sm	= shifts1b[sb];
		f.em	= ~shifts1b[(x2 & 0xf) + 1];

		y	= y1 % ptrn->height;
		f.words	= ((sb + (x2 - x1) + 16) >> 4) - 1;
		x1	>>= 4;
		x1	<<= 1;
		f.d	= (char *)r->base + x1;
		(char *)f.d += (long)y1 * f.dbpl;
		f.m	= (short *)ptrn->mask + y;
		f.s	= (short *)ptrn->exp_data + y;
	}

	SYNC_RASTER(r);
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
			(short *)f.s += 1;
			(short *)f.m += 1;
		}
		(char *)f.d += f.dbpl;
	}
	goto done;
singleline:
	switch (wrmode)
	{
		case 0: f.drawspan = ds_REPLACE_1b;	break;
		case 1: f.drawspan = ds_TRANS_1b;	break;
		case 2: f.drawspan = ds_XOR_1b;		break;
		case 3: f.drawspan = ds_ERASE_1b;	break;
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

		f.sm	= shifts1b[sb];
		f.em	= ~shifts1b[(x2 & 0xf) + 1];

		f.words	= ((sb + (x2 - x1) + 16) >> 4) - 1;
		x1	>>= 4;
		x1	<<= 1;
		f.d	= (char *)r->base + x1;
		(char *)f.d += (long)y1 * f.dbpl;
	}
	SYNC_RASTER(r);
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
