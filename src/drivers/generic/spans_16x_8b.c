#include "display.h"
#include "memory.h"
#include "ovdi_defs.h"
#include "expand.h"
#include "drawing.h"
#include "8b_generic.h"

void ds_REPLACE_8b	(struct fill16x_api *);
void ds_TRANS_8b	(struct fill16x_api *);
void ds_XOR_8b		(struct fill16x_api *);
void ds_ERASE_8b	(struct fill16x_api *);

static unsigned char fillbuff[16 * 16];
static unsigned char maskbuff[16 * 16];

unsigned long nib2long[] = 
{
	0x00000000,
	0x000000ff,
	0x0000ff00,
	0x0000ffff,

	0x00ff0000,
	0x00ff00ff,
	0x00ffff00,
	0x00ffffff,

	0xff000000,
	0xff0000ff,
	0xff00ff00,
	0xff00ffff,

	0xffff0000,
	0xffff00ff,
	0xffffff00,
	0xffffffff,
};

void
ds_REPLACE_8b(struct fill16x_api *f)
{
	register union { void *v; char *c; long *l; } d;
	int i;
	register char *s = (char *)f->s;

	d.v = f->d;

	if (f->words < 0)
	{
		if ((i = f->sm))
			s += i, d.c += i;
		for (i = f->em - i; i > 0; i--)
			*d.c++ = *s++;
	}
	else
	{
		if ((i = f->sm))
		{
			register char *src = s + i;
			d.c += i;
			for (i = 16 - i; i > 0; i--)
				*d.c++ = *src++;
		}
		if ((i = f->words) > 0)
		{
			register unsigned long *src = (long *)s;
			register unsigned long lp0, lp1, lp2, lp3;
			lp0 = *src++, lp1 = *src++, lp2 = *src++, lp3 = *src;
			for (; i > 0; i--)
			{
				*d.l++ = lp0;
				*d.l++ = lp1;
				*d.l++ = lp2;
				*d.l++ = lp3;
			}
		}
		if ((i = f->em) > 0)
		{
			for (; i > 0; i--)
				*d.c++ = *s++;
		}
	}
}
void
ds_TRANS_8b(struct fill16x_api *f)
{
	int i;
	register union { void *v; char *c; long *l; } d;

	if (f->words < 0)
	{
		register char *s = (char *)f->s;
		register char *m = (char *)f->m;
		d.v = f->d;

		i = f->sm;
		if (i)
			s += i, m += i, d.c += i;
		for (i = f->em - i; i > 0; i--)
		{
			if (*m++)
				*d.c++ = *s++;
			else
				d.c++, s++;
		}
	}
	else
	{
		register char *m = (char *)f->m;

		d.v = f->d;

		if ((i = f->sm))
		{
			register char *s = (char *)f->s + i;
			register char *msk = m + i;
			d.c += i;
			for (i = 16 - i; i > 0; i--)
			{
				if (*msk++)
					*d.c++ = *s++;
				else
					d.c++, s++;
			}
		}
		if ((i = f->words) > 0)
		{
			register long *s = (long *)f->s, *msk = (long *)f->m;
			register long lp0, lp1, lp2, lp3, t0, t1;
			lp0 = *s++, lp1 = *s++, lp2 = *s++, lp3 = *s;
			for (; i > 0; i--)
			{
				t0 = *msk;
				t1 = ~t0;
				t1 &= *d.l;
				t1 |= t0 & lp0;
				*d.l++ = t1;
				
				t0 = msk[1];
				t1 = ~t0;
				t1 &= *d.l;
				t1 |= t0 & lp1;
				*d.l++ = t1;

				t0 = msk[2];
				t1 = ~t0;
				t1 &= *d.l;
				t1 |= t0 & lp2;
				*d.l++ = t1;

				t0 = msk[3];
				t1 = ~t0;
				t1 &= *d.l;
				t1 |= t0 & lp3;
				*d.l++ = t1;
			}
		}
		if ((i = f->em))
		{
			register char *s = (char *)f->s, *msk = m;
			for (; i > 0; i--)
			{
				if (*msk++)
					*d.c++ = *s++;
				else
					d.c++, s++;
			}
		}
	}
}

void
ds_XOR_8b(struct fill16x_api *f)
{
	register union { void *v; char *c; long *l; } d;
	int i;
	register unsigned long lp0 = 0xffffffffL;
// 	register char *d = (char *)f->d;

	d.v = f->d;

	if (f->words < 0)
	{
		if ((i = f->sm))
			d.c += i;
		for (i = f->em - i; i > 0; i--)
				*d.c++ ^= (char)lp0;
	}
	else
	{
		if ((i = f->sm))
		{
			d.c += i;
			for (i = 16 - i; i > 0; i--)
				*d.c++ ^= (char)lp0;
		}
		if ((i = f->words) > 0)
		{
			for (; i > 0; i--)
			{
				*d.l++ ^= lp0;
				*d.l++ ^= lp0;
				*d.l++ ^= lp0;
				*d.l++ ^= lp0;
			}
		}
		if ((i = f->em))
		{
			for (; i > 0; i--)
				*d.c++ ^= (char)lp0;
		}
	}
}
void
ds_ERASE_8b(struct fill16x_api *f)
{
	register union { void *v; char *c; long *l; } d;
	int i;
	register unsigned long lp0 = 0xffffffffL;
// 	register char *d = (char *)f->d;
	register char *m = (char *)f->m;

	d.v = f->d;

	if (f->words < 0)
	{
		if ((i = f->sm))
			m += i, d.c += i;
		for (i = f->em - i; i > 0; i--)
		{
			if (*m++)
				d.c++;
			else
				*d.c++ ^= (char)lp0;
		}
	}
	else
	{
		if ((i = f->sm))
		{
			char *msk = m + i;

			d.c += i;
			for (i = 16 - i; i > 0; i--)
			{
				if (*msk++)
					d.c++;
				else
					*d.c++ ^= (char)lp0;
			}
		}
		if ((i = f->words) > 0)
		{
			register unsigned long lp1, lp2, lp3;
			register unsigned long *msk = (long *)m;
			lp0 = *msk++, lp1 = *msk++, lp2 = *msk++, lp3 = *msk;
			for (; i > 0; i--)
			{
				*d.l++ = (*d.l & lp0) | (*d.l ^ ~lp0);
				*d.l++ = (*d.l & lp1) | (*d.l ^ ~lp1);
				*d.l++ = (*d.l & lp2) | (*d.l ^ ~lp2);
				*d.l++ = (*d.l & lp3) | (*d.l ^ ~lp3);
			}
		}
		if ((i = f->em))
		{
			lp0 = 0xff;
			for (; i > 0; i--)
			{
				if (*m++)
					d.c++;
				else
					*d.c++ ^= (char )lp0;
			}
		}
	}
}

void
spans_16x_8b(RASTER *r, COLINF *c, short *spans, short n, PatAttr *ptrn)
{
	int y, wrmode;
	struct fill16x_api f;
	unsigned long fill[4];
	unsigned long mask[4];

	/*
	 * check if pattern is expanded and do expand it if it isnt
	*/
	wrmode = ptrn->wrmode & 0x3;
	if (ptrn->expanded != 8 && ptrn->interior > FIS_SOLID)
	{
		int height;
		unsigned short data, *s;
		unsigned long *d, *m;
		unsigned long lp0, lp1, lp2;

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

			ptrn->exp_data = (unsigned short *)&fillbuff;
			ptrn->mask = (unsigned short *)&maskbuff;
			ptrn->expanded = 0;
		}
		else
			ptrn->expanded = 8;

		s = ptrn->data;
		d = (unsigned long *)ptrn->exp_data;
		m = (unsigned long *)ptrn->mask;
		lp0 = (unsigned long)ptrn->color[wrmode];
		lp0 = (lp0 << 24) | (lp0 << 16) | (lp0 << 8) | lp0;
		lp1 = (unsigned long)ptrn->bgcol[wrmode];
		lp1 = (lp1 << 24) | (lp1 << 16) | (lp1 << 8) | lp1;
		
		for (height = ptrn->height; height > 0; height--)
		{
			data = *s++;

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
	}

	f.dbpl = r->bypl;

	/*
	 * If interior is hollow or solid, we can do things faster,
	 * without expanding pattern.
	*/
	if (ptrn->interior <= FIS_SOLID)
	{
		if (ptrn->interior == FIS_HOLLOW) {
			fill[0] = fill[1] = fill[2] = fill[3] = 0;
			mask[0] = mask[1] = mask[2] = mask[3] = 0;
			wrmode = 0;
		} else {
			short fc = ptrn->color[wrmode];
			fill[0] = fill[1] = fill[2] = fill[3] = ((long)fc << 24) | ((long)fc << 16) | ((long)fc << 8) | fc;
			mask[0] = mask[1] = mask[2] = mask[3] = 0xffffffffL;
		}
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

	switch (wrmode)
	{
		case 0: f.drawspan = ds_REPLACE_8b;	break;
		case 1: f.drawspan = ds_TRANS_8b;	break;
		case 2: f.drawspan = ds_XOR_8b;		break;
		case 3: f.drawspan = ds_ERASE_8b;	break;
		default: return;
	}

	SYNC_RASTER(r);

	for (; n > 0; n--)
	{
		register short y1,x1,x2;
		register int sb;

		y1 = *spans++;
		x1 = *spans++;
		x2 = *spans++;
		sb = x1 & 0xf;
		y = y1 % ptrn->height;
		f.words = (((sb) + (x2 - x1) + 16) >> 4);
		f.sm = sb;
		f.em = (x2 + 1) & 0xf;
		f.d = (char *)r->base + (long)((x1 >> 4) << 4) + ((long)y1 * f.dbpl);
		if (f.sm) f.words--;
		if (f.em) f.words--;
		f.s = (unsigned long *)ptrn->exp_data + ((long)y << 2);
		f.m = (unsigned long *)ptrn->mask + ((long)y << 2);
		(*f.drawspan)(&f);
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

	for (; n > 0; n--)
	{
		register short y1, x1, x2;
		register int sb;

		y1 = *spans++;
		x1 = *spans++;
		x2 = *spans++;
		sb = x1 & 0xf;

		f.words = (((sb) + (x2 - x1) + 16) >> 4);
		f.sm = sb;
		f.em = (x2 + 1) & 0xf;
		f.d = (char *)r->base + (long)((x1 >> 4) << 4) + ((long)y1 * f.dbpl);
		if (f.sm) f.words--;
		if (f.em) f.words--;
		(*f.drawspan)(&f);
	}

done:
	if (!ptrn->expanded)
	{
		ptrn->exp_data = NULL;
		ptrn->mask = NULL;
	}
}
