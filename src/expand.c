#include "expand.h"


struct xpnd
{
	void	(*exp_to)(O_Int pen, struct xpnd *xpnd);
	O_u32	*pixelvals;
	O_16	*dcols;
	O_16	dplanes;
	O_16	splanes;
	O_16	width;
	O_16	height;
	O_16	pmask;
	void	*src;
	void	*dst;
	O_u16	*mask;
	O_16	count;
};

static void exp_to_1b (O_Int pen, struct xpnd *xpnd);
static void exp_to_2b (O_Int pen, struct xpnd *xpnd);
static void exp_to_4b (O_Int pen, struct xpnd *xpnd);
static void exp_to_8b (O_Int pen, struct xpnd *xpnd);
static void exp_to_8bTT (O_Int pen, struct xpnd *xpnd);
static void exp_to_16b (O_Int pen, struct xpnd *xpnd);
static void exp_to_24b (O_Int pen, struct xpnd *xpnd);
static void exp_to_32b (O_Int pen, struct xpnd *xpnd);

#if 0
static void xpnd_vdif_1b(MFDB *src, struct xpnd *x);
static void xpnd_vdif_2b(MFDB *src, struct xpnd *x);
static void xpnd_vdif_4b(MFDB *src, struct xpnd *x);

typedef void (*expand_to)(O_Int, struct xpnd *);
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
	O_u16 data;
	O_u16 *srcfm;
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
	O_u16 data;
	O_u16 *srcfm;
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
	O_u16 data;
	O_u16 *srcfm;
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
exp_to_1b(O_Int pen, struct xpnd *xpnd)
{
	O_Int count = xpnd->count;
	O_u16 mask = (O_u16)xpnd->pmask;
	O_u16 *msk = xpnd->mask;
	O_u16 *dst = (O_u16 *)xpnd->dst;

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

	xpnd->dst = dst;
	xpnd->mask = msk;
	xpnd->count = count;
	xpnd->pmask = mask;
}

static void
exp_to_2b(O_Int pen, struct xpnd *xpnd)
{
	O_Int count = xpnd->count;
	O_u16 mask = (O_u16)xpnd->pmask;
	O_u16 *msk = xpnd->mask;
	O_u16 *dst = xpnd->dst;
	O_u16 pix;

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

	xpnd->dst = dst;
	xpnd->mask = msk;
	xpnd->count = count;
	xpnd->pmask = mask;
}

static void
exp_to_4b(O_Int pen, struct xpnd *xpnd)
{
	O_Int count = xpnd->count;
	O_u16 mask = (O_u16)xpnd->pmask, pix;
	O_u16 *msk = xpnd->mask;
	O_u16 *dst = (O_u16 *)xpnd->dst;

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
		(O_u16 *)xpnd->dst += 4;
		((O_u16 *)xpnd->mask)++;
		xpnd->count = 15;
		xpnd->pmask = 0x8000;
	}
	else
	{
		(O_u16)xpnd->pmask >>= 1;
		xpnd->count = count;
	}
}

static void
exp_to_8bTT(O_Int pen, struct xpnd *xpnd)
{
	O_Int count = xpnd->count, i;
	O_u16 mask = (O_u16)xpnd->pmask, pix;
	O_u16 *msk = xpnd->mask;
	O_u16 *dst = (O_u16 *)xpnd->dst;

	pen &= 255;

	if (count == 15)
		(long)((long *)dst)[0] = (long)((long *)dst)[1] = (long)((long *)dst)[2] = (long)((long *)dst)[3] = *msk = 0;

	pix = xpnd->dcols[pen];
	for (i = 0; i < 8; i++)
	{
		dst[i] |= (pix << count) & mask;
		pix >>= 1;
	}
	if (pen)
		*msk |= 1 << count;

	if ((--count) < 0)
	{
		(long *)xpnd->dst += 8;
		((O_u16 *)xpnd->mask)++;
		xpnd->count = 15;
		xpnd->pmask = 0x8000;
	}
	else
	{
		(O_u16)xpnd->pmask >>= 1;
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
exp_to_8b(O_Int pen, struct xpnd *xpnd)
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
	xpnd->mask = (O_u16 *)msk;
}

static void
exp_to_16b(O_Int pen, struct xpnd *xpnd)
{

	*(O_u16 *)((O_u16 *)xpnd->dst)++ = (O_u16)xpnd->pixelvals[xpnd->dcols[pen & 255]];
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
exp_to_24b(O_Int pen, struct xpnd *xpnd)
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
exp_to_32b(O_Int pen, struct xpnd *xpnd)
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
expand(  O_Int width, O_Int height,
	 O_Int splanes, O_Int sfmt, void *_src,
	 O_Int dplanes, O_Int dfmt, void *_dst, O_16 *dcols,
	 O_u16 *mask )
{
	int i, j;
	O_u16 pixel;
	struct xpnd xpnd;

	if (dplanes == splanes)
	{
		switch (dplanes)
		{
			case 1:
			{
				O_u16 *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
					*d++ = *s++;
				break;
			}
			case 2:
			{
				O_u32 *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
					*d++ = *s++;
				break;
			}
			case 4:
			{
				O_u32 *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
					*d++ = *s++, *d++ = *s++;
				break;
			}
			case 8:
			{
				O_u32 *s = _src, *d = _dst;
				for (i = ((width + 15) >> 4) * height; i > 0; i--)
					*d++ = *s++, *d++ = *s++, *d++ = *s++, *d++ = *s++;
				break;
			}
			case 15:
			case 16:
			{
				O_u32 *s = _src, *d = _dst;
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
				O_u32 *s = _src, *d = _dst;
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
			O_u16 p1;
			O_u16 *s = _src;
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
			O_u32 p1;
			O_u32 *s = _src;
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
			O_u32 p1, p2;
			O_u32 *s = _src;
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
				O_u32 p1, p2, p3, p4;
				O_u32 *s = _src;
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
		