#include "display.h"
#include "memory.h"
#include "ovdi_defs.h"
#include "expand.h"
#include "drawing.h"

#include "8b_generic.h"

extern O_u32 nib2long[];
extern void ds_REPLACE_8b	(struct fill16x_api *);
extern void ds_TRANS_8b		(struct fill16x_api *);
extern void ds_XOR_8b		(struct fill16x_api *);
extern void ds_ERASE_8b		(struct fill16x_api *);

static unsigned char fillbuff[16 * 16];
static unsigned char maskbuff[16 * 16];

void
fill_16x_8b(RASTER *r, COLINF *c, O_Pos *corners, PatAttr *ptrn)
{
	int height, y, wrmode, interior;
	struct fill16x_api f;
	O_u32 fill[4];
	O_u32 mask[4];

	/*
	 * check if pattern is expanded and do expand it if it isnt
	*/
	wrmode = ptrn->wrmode;
	interior = ptrn->interior;
	if (ptrn->expanded != 8 && interior > FIS_SOLID)
	{
		O_u16 data;
		O_u32 lp0, lp1, lp2;
		O_u32 *s, *d, *m;

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

			(long)ptrn->exp_data = (long)&fillbuff;
			(long)ptrn->mask = (long)&maskbuff;
			ptrn->expanded = 0;
		}
		else
			ptrn->expanded = 8;
#if 1
		s = (O_u32 *)ptrn->data;
		d = (O_u32 *)ptrn->exp_data;
		m = (O_u32 *)ptrn->mask;
		lp0 = (O_u32)ptrn->color[wrmode];
		lp0 = (lp0 << 24) | (lp0 << 16) | (lp0 << 8) | lp0;
		lp1 = (O_u32)ptrn->bgcol[wrmode];
		lp1 = (lp1 << 24) | (lp1 << 16) | (lp1 << 8) | lp1;
		
		for (height = ptrn->height; height > 0; height--)
		{
			data = *(O_u16 *)((short *)s)++;

			lp2 = nib2long[data & 0xf];
			d[3] = (lp0 & lp2) | (lp1 & ~lp2);
			m[3] = lp2;

			data >>= 4;
			lp2 = nib2long[data & 0xf];
			d[2] = (lp0 & lp2) | (lp1 & ~lp2);
			m[2] = lp2;

			data >>= 4;
			lp2 = nib2long[data & 0xf];
			d[1] = (lp0 & lp2) | (lp1 & ~lp2);
			m[1] = lp2;

			data >>= 4;
			lp2 = nib2long[data & 0xf];
			d[0] = (lp0 & lp2) | (lp1 & ~lp2);
			m[0] = lp2;

			d += 4;
			m += 4;
		}
#else
		col[1] = ptrn->color[wrmode] & 0xff;
		col[0] = ptrn->bgcol[wrmode] & 0xff;

		expand( ptrn->width, ptrn->height,
			ptrn->planes, PF_ATARI, ptrn->data,
			8, PF_PACKED, ptrn->exp_data, (short *)&col, ptrn->mask);
#endif
	}

	f.dbpl = r->bypl;
	f.words = (((corners[0] & 0xf) + (corners[2] - corners[0]) + 16) >> 4);
	height = corners[3] - corners[1] + 1;

	f.sm = corners[0] & 0xf;
	f.em = (corners[2] + 1) & 0xf;

	f.d = (char *)r->base + (long)((corners[0] >> 4) << 4) + ((long)corners[1] * f.dbpl);

	if (f.sm) f.words--;
	if (f.em) f.words--;

	/*
	 * If interior is hollow or solid, we can do things faster,
	 * without expanding pattern.
	*/
	if (interior <= FIS_SOLID)
	{
		if (interior == FIS_HOLLOW) {
			fill[0] = fill[1] = fill[2] = fill[3] = 0;
			mask[0] = mask[1] = mask[2] = mask[3] = 0;
			wrmode = 0;
		} else {
			short fc = ptrn->color[wrmode];
			fill[0] = fill[1] = fill[2] = fill[3] = ((long)fc << 24) | ((long)fc << 16) | ((long)fc << 8) | fc;
			mask[0] = mask[1] = mask[2] = mask[3] = 0xffffffffL; }
		f.s = &fill;
		f.m = &mask;
		goto singleline;
	}


	/* 
	 * If pattern is a single-line...
	*/
	if (ptrn->height == 1)
	{
		f.s = ptrn->exp_data;
		f.m = ptrn->mask;
		goto singleline;
	}
	else
		y = corners[1] % ptrn->height;

	f.s = (O_u32 *)ptrn->exp_data + ((long)y << 2);
	f.m = (O_u32 *)ptrn->mask + ((long)y << 2);

	switch (wrmode)
	{
		case 0: f.drawspan = ds_REPLACE_8b;	break;
		case 1: f.drawspan = ds_TRANS_8b;	break;
		case 2: f.drawspan = ds_XOR_8b;		break;
		case 3: f.drawspan = ds_ERASE_8b;	break;
		default: return;
	}

	SYNC_RASTER(r);

	for (; height > 0; height--)
	{
		(*f.drawspan)(&f);
		y++;
		if (y >= ptrn->height)
			y = 0, f.s = ptrn->exp_data, f.m = ptrn->mask;
		else
		{
			(char *)f.s += 16;
			(char *)f.m += 16;
		}
		(char *)f.d += f.dbpl;
	}
	goto done;

singleline:
	switch (wrmode)
	{
		case 0: f.drawspan = ds_REPLACE_8b;	break;
		case 1: f.drawspan = ds_TRANS_8b;	break;
		case 2: f.drawspan = ds_XOR_8b;		break;
		case 3: f.drawspan = ds_ERASE_8b;	break;
		default: return;
	}

	SYNC_RASTER(r);

	if ((--height) >= 0)
		(*f.drawspan)(&f);

	for (; height > 0; height--)
	{
		(char *)f.d += f.dbpl;
		(*f.drawspan)(&f);
	}

done:	if (!ptrn->expanded)
	{
		ptrn->exp_data = 0;
		ptrn->mask = 0;
	}
}
