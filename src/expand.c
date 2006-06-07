#include "expand.h"


struct xpnd
{
	void	(*exp_to)(short pen, struct xpnd *xpnd);
	unsigned long	*pixelvals;
	short	*dcols;
	short	dplanes;
	short	splanes;
	short	width;
	short	height;
	short	pmask;
	char	*src;
	char	*dst;
	unsigned short	*mask;
	short	count;
};

static void exp_to_1b (short pen, struct xpnd *xpnd);
static void exp_to_2b (short pen, struct xpnd *xpnd);
static void exp_to_4b (short pen, struct xpnd *xpnd);
static void exp_to_8b (short pen, struct xpnd *xpnd);
static void exp_to_8bTT (short pen, struct xpnd *xpnd);
static void exp_to_16b (short pen, struct xpnd *xpnd);
static void exp_to_24b (short pen, struct xpnd *xpnd);
static void exp_to_32b (short pen, struct xpnd *xpnd);

#if 0
static void xpnd_vdif_1b(MFDB *src, struct xpnd *x);
static void xpnd_vdif_2b(MFDB *src, struct xpnd *x);
static void xpnd_vdif_4b(MFDB *src, struct xpnd *x);

typedef void (*expand_to)(short, struct xpnd *);
static expand_to exp_to[] =
{
	0,
	exp_to_1b,
	exp_to_2b,
	0,
	exp_to_4b,
	0,0,0,
	exp_to_8b,
	0,0,0,0,0,0,
	exp_to_16b,
	exp_to_16b,
	0,0,0,0,0,0,0,
	exp_to_24b,
	0,0,0,0,0,0,0,
	exp_to_32b
};

typedef void (*expand_from)(MFDB *, struct xpnd *);

static expand_from exp_from[] =
{
	0,
	xpnd_vdif_1b, /* 1b, */
	xpnd_vdif_2b, /* 2b, */
	0,
	xpnd_vdif_4b,
	0,0,0,
	0 /* 8b */
};

static void
xpnd_vdif_1b(MFDB *src, struct xpnd *x)
{
	unsigned short data;
	unsigned short *srcfm;
	int i, j, k, plen, scnt, bit;

	srcfm = src->fd_addr;
	plen = src->fd_wdwidth;

	for (i = 0; i < src->fd_h; i++)
	{
		for (j = 0; j < src->fd_wdwidth; j++)
		{
			scnt = 0;
			bit = 0x8000;
			for (k = 0; k < 16; k++)
			{
				data =   (*srcfm & bit) << scnt;
				data >>= 15;

				(*x->exp_to)(data, x);

				scnt++;
				bit >>= 1;
			}
			srcfm++;
		}
	}
}

static void
xpnd_vdif_2b(MFDB *src, struct xpnd *x)
{
	unsigned short data;
	unsigned short *srcfm;
	int i, j, k, plen, scnt, bit;

	srcfm = src->fd_addr;
	plen = src->fd_wdwidth;

	for (i = 0; i < src->fd_h; i++)
	{
		for (j = 0; j < src->fd_wdwidth; j++)
		{
			scnt = 0;
			bit = 0x8000;
			for (k = 0; k < 16; k++)
			{
				data =   (srcfm[ 0   ] & bit) << scnt;
				data >>= 1;
				data |=  (srcfm[ plen ] & bit) << scnt;
				data >>= 14;

				(*x->exp_to)(data, x);

				scnt++;
				bit >>= 1;
			}
			srcfm++;
		}
	}
}

static void
xpnd_vdif_4b(MFDB *src, struct xpnd *x)
{
	unsigned short data;
	unsigned short *srcfm;
	int i, j, k, plen, scnt, bit;

	srcfm = src->fd_addr;
	plen = src->fd_wdwidth;

	for (i = 0; i < src->fd_h; i++)
	{
		for (j = 0; j < src->fd_wdwidth; j++)
		{
			scnt = 0;
			bit = 0x8000;
			for (k = 0; k < 16; k++)
			{
				data =   (srcfm[ 0	 ] & bit) << scnt;
				data >>= 1;
				data |=  (srcfm[ plen	 ] & bit) << scnt;
				data >>= 1;
				data |=  (srcfm[ plen << 1] & bit) << scnt;
				data >>= 1;
				data |=  (srcfm[ plen * 3 ] & bit) << scnt;
				data >>= 12;

				(*x->exp_to)(data, x);

				scnt++;
				bit >>= 1;
			}
			srcfm++;
		}
	}
}
#endif

static void
exp_to_1b(short pen, struct xpnd *xpnd)
{
	short count = xpnd->count;
	unsigned short mask = (unsigned short)xpnd->pmask;
	unsigned short *msk = xpnd->mask;
	unsigned short *dst = (unsigned short *)xpnd->dst;

	if (pen > 1)
		pen = 1;
	else if (pen < 0)
		pen = 0;

	if (count == 15)
		*dst = 0, *msk = 0;

	pen = xpnd->dcols[pen];
	*dst |= (pen << count) & mask;
	if (pen)
		*msk |= 1 << count;

	if ((--count) < 0)
	{
		dst++;
		msk++;
		count = 15;
		mask = 0x8000;
	}
	else
		mask >>= 1;

	xpnd->dst = (char *)dst;
	xpnd->mask = msk;
	xpnd->count = count;
	xpnd->pmask = mask;
}

static void
exp_to_2b(short pen, struct xpnd *xpnd)
{
	short count = xpnd->count;
	unsigned short mask = (unsigned short)xpnd->pmask;
	unsigned short *msk = xpnd->mask;
	unsigned short *dst = (unsigned short *)xpnd->dst;
	unsigned short pix;

	if (pen > 3)
		pen = 3;
	else if (pen < 0)
		pen = 0;

	if (count == 15)
		dst[0] = dst[1] = *msk = 0;

	pix = xpnd->dcols[pen];
	dst[0] |= (pix << count) & mask;
	pix >>= 1;
	dst[1] |= (pix << count) & mask;
	if (pen)
		*msk |= 1 << count;

	if ((--count) < 0)
	{
		dst += 2;
		msk++;
		count = 15;
		mask = 0x8000;
	}
	else
		mask >>= 1;

	xpnd->dst = (char *)dst;
	xpnd->mask = msk;
	xpnd->count = count;
	xpnd->pmask = mask;
}

static void
exp_to_4b(short pen, struct xpnd *xpnd)
{
	short count = xpnd->count;
	unsigned short mask = (unsigned short)xpnd->pmask, pix;
	unsigned short *msk = xpnd->mask;
	unsigned short *dst = (unsigned short *)xpnd->dst;

	if (pen > 15)
		pen = 15;
	else if (pen < 0)
		pen = 0;

	if (count == 15)
		(long)((long *)dst)[0] = (long)((long *)dst)[1] = *msk = 0;

	pix = xpnd->dcols[pen];
	*dst++	|= (pix << count) & mask;
	pix >>= 1;
	*dst++	|= (pix << count) & mask;
	pix >>= 1;
	*dst++	|= (pix << count) & mask;
	pix >>= 1;
	*dst	|= (pix << count) & mask;
	if (pen)
		*msk |= (1 << count);

	if ((--count) < 0)
	{
		xpnd->dst += 4 * sizeof(short);
		xpnd->mask++;
		xpnd->count = 15;
		xpnd->pmask = 0x8000;
	}
	else
	{
		(unsigned short)xpnd->pmask >>= 1;
		xpnd->count = count;
	}
}

static void
exp_to_8bTT(short pen, struct xpnd *xpnd)
{
	union { long lng; unsigned long *l; unsigned short *s; } da;
	
	short count = xpnd->count, i;
	unsigned short mask = (unsigned short)xpnd->pmask, pix;
	unsigned short *msk = xpnd->mask;
// 	unsigned short *dst = (unsigned short *)xpnd->dst;

	pen &= 255;
	
	da.l = (long *)xpnd->dst;
	
	if (count == 15)
	{
		da.l[0] = da.l[1] = da.l[2] = da.l[3] = 0;
		*msk = 0;
	}
	pix = xpnd->dcols[pen];
	for (i = 0; i < 8; i++)
	{
		da.s[i] |= (pix << count) & mask;
		pix >>= 1;
	}
	if (pen)
		*msk |= 1 << count;

	if ((--count) < 0)
	{
		xpnd->dst += 8 * sizeof(long);
		xpnd->mask++;
		xpnd->count = 15;
		xpnd->pmask = 0x8000;
	}
	else
	{
		xpnd->pmask = (short)((unsigned short)xpnd->pmask >> 1);
		xpnd->count = count;
	}

	//xpnd->dst = dst;
	//xpnd->mask = msk;
	//xpnd->count = count;
	//xpnd->pmask = mask;
}
/*
 * 8-bit packed pixels are the only exception to the fact
 * that the mask is a bit-mask. 8bpp packed format mask
 * is a byte array.
*/
static void
exp_to_8b(short pen, struct xpnd *xpnd)
{
	unsigned char *msk = (char *)xpnd->mask;
	char *dst = (char *)xpnd->dst;

	pen &= 255;
	*dst++ = xpnd->dcols[pen] & 0xff;
	if (pen)
		*msk++ = 0xff;
	else
		*msk++ = 0;

	xpnd->dst = dst;
	xpnd->mask = (unsigned short *)msk;
}

static void
exp_to_16b(short pen, struct xpnd *xpnd)
{
	unsigned short *dst = (unsigned short *)xpnd->dst;

	*dst++ = (unsigned short)xpnd->pixelvals[xpnd->dcols[pen & 255]];

	if (xpnd->count == 15)
		*xpnd->mask = 0;
	if (pen & 255)
		*xpnd->mask |= 1 << xpnd->count;
	if ((--xpnd->count) < 0)
	{
		xpnd->mask++;
		xpnd->count = 15;
	}
}

static void
exp_to_24b(short pen, struct xpnd *xpnd)
{
	long pixel;
	char *dst = (char *)xpnd->dst;

	pen &= 255;

	pixel  = xpnd->pixelvals[xpnd->dcols[pen]];
	*dst++ = (pixel >> 16) & 0xff;
	*dst++ = (pixel >> 8) & 0xff;
	*dst++ = pixel;
	if (xpnd->count == 15)
		*xpnd->mask = 0;
	if (pen)
		*xpnd->mask |= 1 << xpnd->count;

	if ((--xpnd->count) < 0)
	{
		xpnd->mask++;
		xpnd->count = 15;
	}
}

static void
exp_to_32b(short pen, struct xpnd *xpnd)
{
	long *dst = (long *)xpnd->dst;

	pen &= 255;

	*dst++ = xpnd->pixelvals[xpnd->dcols[pen]];
	if (xpnd->count == 15)
		*xpnd->mask = 0;
	if (pen)
		*xpnd->mask |= 1 << xpnd->count;

	if ((--xpnd->count) < 0)
	{
		xpnd->mask++;
		xpnd->count = 15;
	}
}

void
expand(  short width, short height,
	 short splanes, short sfmt, void *_src,
	 short dplanes, short dfmt, void *_dst, short *dcols,
	 unsigned short *mask )
{
	int i, j;
	unsigned short pixel;
	struct xpnd xpnd;

	if (dplanes == splanes)
	{
		switch (dplanes)
		{
			case 1:
			{
				unsigned short *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
					*d++ = *s++;
				break;
			}
			case 2:
			{
				unsigned long *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
					*d++ = *s++;
				break;
			}
			case 4:
			{
				unsigned long *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
					*d++ = *s++, *d++ = *s++;
				break;
			}
			case 8:
			{
				unsigned long *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
					*d++ = *s++, *d++ = *s++, *d++ = *s++, *d++ = *s++;
				break;
			}
			case 15:
			case 16:
			{
				unsigned long *s = _src, *d = _dst;
				for (i = (((width + 15) >> 4) * height) << 1; i > 0; i--)
					*d++ = *s++, *d++ = *s++, *d++ = *s++, *d++ = *s++;
				break;
			}
			case 24:
			{
				int I;
				unsigned char *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
				{
					for (I = 0; I < 16; I++)
						*d++ = *s++, *d++ = *s++, *d++ = *s++;
				}
				break;
			}
			case 32:
			{
				int I;
				unsigned long *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
				{
					for (I = 0; I < 4; I++)
						*d++ = *s++, *d++ = *s++, *d++ = *s++, *d++ = *s++;
				}
				break;
			}
		}
		return;
	}
	xpnd.splanes = splanes;
	xpnd.dplanes = dplanes;
	xpnd.src = (char *)_src;
	xpnd.dst = (char *)_dst;
	xpnd.count = 15;
	xpnd.pmask = 0x8000;
	xpnd.mask = mask;
	xpnd.width = width;
	xpnd.dcols = dcols;

	switch (dplanes)
	{
		case 1:
			xpnd.exp_to = exp_to_1b;
			break;
		case 2:
			xpnd.exp_to = exp_to_2b;
			break;
		case 4:
			xpnd.exp_to = exp_to_4b;
			break;
		case 8:
		{
			if (dfmt == PF_ATARI)
				xpnd.exp_to = exp_to_8bTT;
			else if (dfmt == PF_PACKED)
				xpnd.exp_to = exp_to_8b;
			else
				return;
			break;
		}
		case 15:
		case 16:
			xpnd.exp_to = exp_to_16b;
			break;
		case 24:
			xpnd.exp_to = exp_to_24b;
			break;
		case 32:
			xpnd.exp_to = exp_to_32b;
			break;
		default: 
			return;
	}

	switch (splanes)
	{
		case 1:
		{
			unsigned short p1;
			unsigned short *s = _src;
			for (; height > 0; height--)
			{
				for (j = (width + 15) >> 4; j > 0; j--)
				{
					p1 = *s++;
					for (i = 0; i < 16; i++)
					{
						pixel = p1 & 0x8000 ? 1 : 0;
						(*xpnd.exp_to)(pixel, &xpnd);
						p1 <<= 1;
					}
				}
			}
			break;
		}
		case 2:
		{
			unsigned long p1;
			unsigned long *s = _src;
			for (; height > 0; height--)
			{
				for (j = (width + 15) >> 4; j > 0; j--)
				{
					p1 = *s++;
					for (i = 0; i < 16; i++)
					{
						pixel = p1 & 0x8000 ? 1 : 0;
						pixel <<= 1;
						pixel |= p1 & 0x80000000L ? 1 : 0;
						(*xpnd.exp_to)(pixel, &xpnd);
						p1 <<= 1;
					}
				}
			}
			break;
		}
		case 4:
		{
			unsigned long p1, p2;
			unsigned long *s = _src;
			for (; height > 0; height--)
			{
				for (j = (width + 15) >> 4; j > 0; j--)
				{
					p1 = *s++;
					p2 = *s++;
					for (i = 0; i < 16; i++)
					{
						pixel = p2 & 0x8000 ? 1 : 0;
						pixel <<= 1;
						pixel |= p2 & 0x80000000 ? 1 : 0;
						pixel <<= 1;
						pixel |= p1 & 0x8000 ? 1 : 0;
						pixel <<= 1;
						pixel |= p1 & 0x80000000 ? 1 : 0;
						(*xpnd.exp_to)(pixel, &xpnd);
						p1 <<= 1, p2 <<= 1;
					}
				}
			}
			break;
		}
		case 8:
		{
			if (sfmt == PF_ATARI)
			{
				unsigned long p1, p2, p3, p4;
				unsigned long *s = _src;
				for (; height > 0; height--)
				{
					for (j = (width + 15) >> 4; j > 0; j--)
					{
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						p4 = *s++;
						for (i = 0; i < 16; i++)
						{
							pixel |= p4 & 0x8000 ? 1 : 0;
							pixel <<= 1;
							pixel |= p4 & 0x80000000 ? 1 : 0;
							pixel <<= 1;
							pixel |= p3 & 0x8000 ? 1 : 0;
							pixel <<= 1;
							pixel |= p3 & 0x80000000 ? 1 : 0;
							pixel <<= 1;
							pixel = p2 & 0x8000 ? 1 : 0;
							pixel <<= 1;
							pixel |= p2 & 0x80000000 ? 1 : 0;
							pixel <<= 1;
							pixel |= p1 & 0x8000 ? 1 : 0;
							pixel <<= 1;
							pixel |= p1 & 0x80000000 ? 1 : 0;
							(*xpnd.exp_to)(pixel, &xpnd);
							p1 <<= 1, p2 <<= 1, p3 <<= 1, p4 <<= 1;
						}
					}
				}
			}
			else if (sfmt == PF_PACKED)
			{
				unsigned char *s = _src;
				for (; height > 0; height--)
				{
					for (j = (width + 15) >> 4; j > 0; j--)
					{
						for (i = 0; i < 16; i++)
						{
							pixel = *s++;
							(*xpnd.exp_to)(pixel, &xpnd);
						}
					}
				}
				
			}
			break;
		}
	} /* switch (planes */
}
		
