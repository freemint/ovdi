/* This file contains the built in 8 bit per planes support */

#include "display.h"
#include "linea.h"
#include "mouse.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "8b_driver.h"

/* ******************************************************** */
/* *************** DIFFERENT WRMODES **************** */
/* ******************************************************** */
static void ALL_WHITE	(unsigned char *addr, long data);
static void S_AND_D	(unsigned char *addr, long data);
static void S_AND_NOTD	(unsigned char *addr, long data);
static void S_ONLY	(unsigned char *addr, long data);
static void NOTS_AND_D	(unsigned char *addr, long data);
static void D_ONLY	(unsigned char *addr, long data);
static void S_XOR_D	(unsigned char *addr, long data);
static void S_OR_D	(unsigned char *addr, long data);
static void NOT_SORD	(unsigned char *addr, long data);
static void NOT_SXORD	(unsigned char *addr, long data);
static void NOT_D	(unsigned char *addr, long data);
static void S_OR_NOTD	(unsigned char *addr, long data);
static void NOT_S	(unsigned char *addr, long data);
static void NOTS_OR_D	(unsigned char *addr, long data);
static void NOT_SANDD	(unsigned char *addr, long data);
static void ALL_BLACK	(unsigned char *addr, long data);

unsigned long
get_pixel_8b(unsigned char *sb, short bpl, short x, short y)
{
	return ((*(unsigned char *)(sb + (long)(x + ((long)y * bpl)))) & 0xff);
}

void
put_pixel_8b(unsigned char *sb, short bpl, short x, short y, unsigned long pixel)
{
	*(sb + (long)(x + ((long)y * bpl))) = (unsigned char)pixel;
	return;
}

pixel_blit dpf_8b[] = 
{
	S_ONLY,
	S_ONLY,

	S_ONLY,
	0,

	S_XOR_D,
	S_XOR_D,

	0,
	S_XOR_D,

	0,0,0,0,	/* reserved */
	0,0,0,0		/* reserved */
};

pixel_blit rt_ops_8b[] = 
{
	ALL_WHITE,
	S_AND_D,
	S_AND_NOTD,
	S_ONLY,
	NOTS_AND_D,
	D_ONLY,
	S_XOR_D,
	S_OR_D,
	NOT_SORD,
	NOT_SXORD,
	NOT_D,
	S_OR_NOTD,
	NOT_S,
	NOTS_OR_D,
	NOT_SANDD,
	ALL_BLACK
};

static void
ALL_WHITE(unsigned char *addr, long data)
{
	*addr = 0;
	return;
}
static void
S_AND_D(unsigned char *addr, long data)
{
	*addr = (unsigned char)data & *addr;
	return;
}
static void
S_AND_NOTD(unsigned char *addr, long data)
{
	*addr = (unsigned char)data & ~(*addr);
	return;
}
static void
S_ONLY(unsigned char *addr, long data)
{
	*addr = (unsigned char)data;
	return;
}
static void
NOTS_AND_D(unsigned char *addr, long data)
{
	*addr &= (unsigned char)~data;
	return;
}
static void
D_ONLY(unsigned char *addr, long data)
{
	return;
}
static void
S_XOR_D(unsigned char *addr, long data)
{
	*addr ^= (unsigned char)data;
	return;
}
static void
S_OR_D(unsigned char *addr, long data)
{
	*addr |= (unsigned char)data;
	return;
}
static void
NOT_SORD(unsigned char *addr, long data)
{
	*addr = ~((unsigned char)data | *addr);
	return;
}
static void
NOT_SXORD(unsigned char *addr, long data)
{
	*addr = ~((unsigned char)data ^ *addr);
	return;
}
static void
NOT_D(unsigned char *addr, long data)
{
	*addr = ~*addr;
	return;
}
static void
S_OR_NOTD(unsigned char *addr, long data)
{
	*addr = (unsigned char)data | ~*addr;
	return;
}
static void
NOT_S(unsigned char *addr, long data)
{
	*addr = (unsigned char)~data;
	return;
}
static void
NOTS_OR_D(unsigned char *addr, long data)
{
	*addr = (unsigned char)~data | *addr;
	return;
}
static void
NOT_SANDD(unsigned char *addr, long data)
{
	*addr = ~((unsigned char)data & *addr);
	return;
}
static void
ALL_BLACK(unsigned char *addr, long data)
{
	*addr = 0xff;
	return;
}

void
draw_solid_rect_8b(RASTER *r, short *corners, PatAttr *ptrn)
{
	unsigned char *dst, *d;
	short i, bypl, dx, dy, wrmode;
	unsigned long pixel;

	wrmode = ptrn->wrmode;
	bypl = r->bypl;
	pixel = ptrn->color[wrmode] & 0xff;
	pixel |= ((long)pixel << 24) | ((long)pixel << 16) | (pixel << 8);
	dx = corners[2] - corners[0] + 1;
	dy = corners[3] - corners[1] + 1;

	d = (unsigned char *)r->base + corners[0] + (long)corners[1] * bypl;

	switch (wrmode)
	{
		case 0: /* MD_REPLACE */
		case 1: /* MD_TRANS */
		{
			for (; dy > 0; dy--)
			{
				dst = d;

				for (i = dx >> 4; i > 0; i--)
				{
					*(unsigned long *)((unsigned long *)dst)++ = (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ = (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ = (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ = (long)pixel;
				}
				for (i = dx & 0xf; i > 0; i--)
					*(unsigned char *)((unsigned char *)dst)++ = (unsigned char)pixel;

				d += bypl;
			}
			break;
		}
		case 2: /* MD_EOR */
		{
			for (; dy > 0; dy--)
			{
				dst = d;

				for (i = dx >> 4; i > 0; i--)
				{
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
				}
				for (i = dx & 0xf; i > 0; i--)
					*(unsigned char *)((unsigned char *)dst)++ ^= (unsigned char)pixel;

				d += bypl;
			}
			break;
		}
		case 3:
		{
			for (; dy > 0; dy--)
			{
				dst = d;

				for (i = dx >> 4; i > 0; i--)
				{
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
				}
				for (i = dx & 0xf; i > 0; i--)
					*(unsigned char *)((unsigned char *)dst)++ ^= (unsigned char)pixel;

				d += bypl;
			}
			break;
		}
	}
}

/* *************** RASTER OPERATIONS **************** */
static void rb_ALL_WHITE	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_S_AND_D		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_S_AND_NOTD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_S_ONLY		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_NOTS_AND_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_D_ONLY		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_S_XOR_D		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_S_OR_D		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_NOT_SORD		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_NOT_SXORD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_NOT_D		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_S_OR_NOTD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_NOT_S		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_NOTS_OR_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_NOT_SANDD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
static void rb_ALL_BLACK	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);

raster_blit rops_8b[] =
{
	rb_ALL_WHITE,
	rb_S_AND_D,
	rb_S_AND_NOTD,
	rb_S_ONLY,
	rb_NOTS_AND_D,
	rb_D_ONLY,
	rb_S_XOR_D,
	rb_S_OR_D,
	rb_NOT_SORD,
	rb_NOT_SXORD,
	rb_NOT_D,
	rb_S_OR_NOTD,
	rb_NOT_S,
	rb_NOTS_OR_D,
	rb_NOT_SANDD,
	rb_ALL_BLACK
};



static void
rb_ALL_WHITE(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char /**srcp,*/ *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			//srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = 0x0;

			//srcptr += srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			//srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = 0x0;

			//srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}

static void
rb_S_AND_D(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- &= *srcp--;

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ &= *srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_S_AND_NOTD(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = *srcp-- & (~*dstp);

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = *srcp++ & (~*dstp);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_S_ONLY(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (void *)((char *)srcptr + 1);
			dstp = (void *)((char *)dstptr + 1);
			
			for (j = width >> 4; j > 0; j--)
			{
				*(long *)--((long *)dstp) = *(long *)--((long *)srcp);
				*(long *)--((long *)dstp) = *(long *)--((long *)srcp);
				*(long *)--((long *)dstp) = *(long *)--((long *)srcp);
				*(long *)--((long *)dstp) = *(long *)--((long *)srcp);
			}
			for (j = width & 0xf; j > 0; j--)
				*(char *)--dstp = *(char *)--srcp;
#if 0			
			for (j = width; j > 0; j--)
				*dstp-- = *srcp--;
#endif
			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width >> 4; j > 0; j--)
			{
				*(long *)((long *)dstp)++ = *(long *)((long *)srcp)++;
				*(long *)((long *)dstp)++ = *(long *)((long *)srcp)++;
				*(long *)((long *)dstp)++ = *(long *)((long *)srcp)++;
				*(long *)((long *)dstp)++ = *(long *)((long *)srcp)++;
			}
			for (j = width & 0xf; j > 0; j--)
				*dstp++ = *srcp++;
#if 0
			for (j = width; j > 0; j--)
				*dstp++ = *srcp++;
#endif
			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_NOTS_AND_D(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- &= ~(*srcp--);

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ &= ~(*srcp++);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_D_ONLY(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	return;
}
static void
rb_S_XOR_D(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- ^= *srcp--;

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ ^= *srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_S_OR_D(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- |= *srcp--;

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ |= *srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_NOT_SORD(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = ~(*srcp-- | *dstp);

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = ~(*srcp++ | *dstp);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_NOT_SXORD(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = ~(*srcp-- ^ *dstp);

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = ~(*srcp++ ^ *dstp);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_NOT_D(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char /**srcp,*/ *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

//			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = ~*dstp;

//			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

//			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = ~*dstp;

//			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_S_OR_NOTD(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = *srcp-- | ~(*dstp);

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = *srcp++ | ~(*dstp);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_NOT_S(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = ~(*srcp--);

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = ~(*srcp++);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_NOTS_OR_D(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- |= ~(*srcp--);

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ |= ~(*srcp++);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_NOT_SANDD(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = ~(*srcp-- & *dstp);

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = ~(*srcp++ & *dstp);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
static void
rb_ALL_BLACK(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned char /**srcp,*/ *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

//			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = 0;

//			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

//			srcp = (unsigned char *)srcptr;
			dstp = (unsigned char *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = 0xff;

//			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}

void
draw_mousecurs_8b(register XMFORM *mf, register short x, register short y)
{

	register short width, height, xoff, yoff, bypl;
	register XMSAVE *ms;

	//display("x %d, y %d, mx %d, my %d\n", x, y, mx, my);

	width = mf->width;
	height = mf->height;
	ms = mf->save;
	bypl = mf->bypl;

	xoff = yoff = 0;

	if ((x -= mf->xhot) < 0)
	{
		if ( (width += x) < 0)
		{
			ms->valid = 0;
			return;
		}
		xoff = -x;
		x = 0;
	}
	else
	{
		register short x2 = x + width - 1;
		register short mx = mf->mx;

		if (x2 > mx)
		{
			if ( (width -= x2 - mx) < 0)
			{
				ms->valid = 0;
				return;
			}
		}
	}

	if ((y -= mf->yhot) < 0)
	{
		if ((height += y) < 0)
		{
			ms->valid = 0;
			return;
		}
		yoff = -y;
		y = 0;
	}
	else
	{
		register short y2 = y + height - 1;
		register short my = mf->my;

		if (y2 > my)
		{
			if ((height -= (y2 - my)) < 0)
			{
				ms->valid = 0;
				return;
			}
		}
	}

	{
		register unsigned char *src, *dst;
		register short i, j, nl;

		dst = (unsigned char *)ms->save;
		src = mf->scr_base + (long)(((long)y * bypl) + x);

		ms->width = width;
		ms->height = height;
		ms->bypl = bypl;
		ms->valid = 1;
		ms->src = src;

		nl = bypl - width;

		for (i = height; i > 0; i--)
		{

			for (j = width; j > 0; j--)
				*dst++ = *src++;

			src += nl;
		}
	}

	if (mf->planes == 1)
	{
		register short i, j, nl, mnl, w;
		register unsigned long fgc, bgc;
		register unsigned short mask, data, ovl;
		register unsigned short *ma;
		register unsigned char *dst;


		nl = bypl - width;
		dst = mf->scr_base + (long)(((long)y * bypl) + x);
		ma = (unsigned short *)mf->data + (long)(yoff * mf->mfbypl) + (long)((xoff >> 4) << 1);
		xoff &= 0xf;
		mnl = ((mf->width - (xoff + width)) >> 4) << 1;
		fgc = mf->fg_pix;
		bgc = mf->bg_pix;

		for (i = height; i > 0; i--)
		{
			mask = *ma++;
			data = *ma++;
			ovl = 0x8000 >> xoff;
			w = width;

			if ( (16 - xoff) < width)
			{
				for (j = 16 - xoff; j > 0; j--)
				{
					if (mask & ovl)
					{
						if (data & ovl)
							*dst = (unsigned char)fgc;
						else
							*dst = (unsigned char)bgc;
					}
					dst++;
					ovl >>= 1;
				}
				mask = *ma++;
				data = *ma++;
				ovl = 0x8000;
				w -= (16 - xoff);
			}

			while (w > 16)
			{
				for (j = 16; j > 0; j--)
				{
					if (mask & ovl)
					{
						if (data & ovl)
							*dst = (unsigned char)fgc;
						else
							*dst = (unsigned char)bgc;
					}
					dst++;
					ovl >>= 1;
				}
				mask = *ma++;
				data = *ma++;
				ovl = 0x8000;
				w -= 16;
			}

			for (j = w; j > 0; j--)
			{
				if (mask & ovl)
				{
					if (data & ovl)
						*dst = (unsigned char)fgc;
					else
						*dst = (unsigned char)bgc;
				}
				dst++;
				ovl >>= 1;
			}
			dst += nl;
			ma += mnl;
		}
	}
	else
	{
		register short i, j, dnl, snl, mnl, w;
		register unsigned short mask, ovl;
		register unsigned short *ma;
		register unsigned char *src, *dst;


		dnl = bypl - width;
		snl = mf->mfbypl;
		dst = mf->scr_base + (long)(((long)y * bypl) + x);
		src = mf->data + (long)(((long)yoff * snl) + (long)xoff);
		snl -= width;
		ma = (unsigned short *)mf->mask + (long)(yoff * ((mf->width >> 4) +1)) + (long)(xoff >> 4);
		xoff &= 0xf;
		mnl = (mf->width - (xoff + width)) >> 4;

		for (i = height; i > 0; i--)
		{
			mask = *ma++;
			ovl = 0x8000 >> xoff;
			w = width;

			if ( (16 - xoff) < width)
			{
				for (j = 16 - xoff; j > 0; j--)
				{
					if (mask & ovl)
						*dst = *src;
					dst++;
					src++;
					ovl >>= 1;
				}
				mask = *ma++;
				ovl = 0x8000;
				w -= (16 - xoff);
			}

			while (w > 16)
			{
				for (j = 16; j > 0; j--)
				{
					if (mask & ovl)
						*dst = *src;
					dst++;
					src++;
					ovl >>= 1;
				}
				mask = *ma++;
				ovl = 0x8000;
				w -= 16;
			}

			for (j = w; j > 0; j--)
			{
				if (mask & ovl)
					*dst = *src;
				dst++;
				src++;
				ovl >>= 1;
			}
			dst += dnl;
			src += snl;
			ma += mnl;
		}
	}	

	//display("x %d, y %d, widht %d, height %d, xoff %d, yoff %d\n", x, y, width, height, xoff, yoff);

	return;
}

void
restore_msave_8b(XMSAVE *ms)
{
	register short width, w, height, nl;
	register unsigned char *src, *dst;

	if (ms->valid)
	{
		src = (unsigned char *)ms->save;
		dst = (unsigned char *)ms->src;
		width = ms->width;
		nl = ms->bypl - width;
		for (height = ms->height; height > 0; height--)
		{
			for (w = width; w > 0; w--)
				*dst++ = *src++;
			dst += nl;
		}
		ms->valid = 0;
	}
	return;
}
