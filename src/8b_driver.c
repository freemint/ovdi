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
void ALL_WHITE_8b	(unsigned char *addr, long data);
void S_AND_D_8b		(unsigned char *addr, long data);
void S_AND_NOTD_8b	(unsigned char *addr, long data);
void S_ONLY_8b		(unsigned char *addr, long data);
void NOTS_AND_D_8b	(unsigned char *addr, long data);
void D_ONLY_8b		(unsigned char *addr, long data);
void S_XOR_D_8b		(unsigned char *addr, long data);
void S_OR_D_8b		(unsigned char *addr, long data);
void NOT_SORD_8b	(unsigned char *addr, long data);
void NOT_SXORD_8b	(unsigned char *addr, long data);
void NOT_D_8b		(unsigned char *addr, long data);
void S_OR_NOTD_8b	(unsigned char *addr, long data);
void NOT_S_8b		(unsigned char *addr, long data);
void NOTS_OR_D_8b	(unsigned char *addr, long data);
void NOT_SANDD_8b	(unsigned char *addr, long data);
void ALL_BLACK_8b	(unsigned char *addr, long data);

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

#if 0
void dlp_REP_fg_8b(unsigned char *addr, long data);
void dlp_REP_bg_8b(unsigned char *addr, long data);
void dlp_TRANS_fg_8b(unsigned char *addr, long data);
void dlp_XOR_fg_8b(unsigned char *addr, long data);
void dlp_XOR_bg_8b(unsigned char *addr, long data);
void dlp_ERAS_bg_8b(unsigned char *addr, long data);
#endif 

draw_pixel dpf_8b[] =
{
	S_ONLY_8b,	/*dlp_REP_fg_8b,*/
	S_ONLY_8b,	/*dlp_REP_bg_8b,*/

	S_ONLY_8b,	/* dlp_TRANS_fg_8b,*/
	0,

	S_XOR_D_8b,	/*dlp_XOR_fg_8b,*/
	S_XOR_D_8b,    /* dlp_XOR_bg_8b, */

	0,
	S_XOR_D_8b	/* dlp_ERAS_bg_8b*/
};

draw_pixel rt_ops_8b[] = 
{
	ALL_WHITE_8b,
	S_AND_D_8b,
	S_AND_NOTD_8b,
	S_ONLY_8b,
	NOTS_AND_D_8b,
	D_ONLY_8b,
	S_XOR_D_8b,
	S_OR_D_8b,
	NOT_SORD_8b,
	NOT_SXORD_8b,
	NOT_D_8b,
	S_OR_NOTD_8b,
	NOT_S_8b,
	NOTS_OR_D_8b,
	NOT_SANDD_8b,
	ALL_BLACK_8b
};

void
ALL_WHITE_8b(unsigned char *addr, long data)
{
	*addr = 0;
	return;
}
void
S_AND_D_8b(unsigned char *addr, long data)
{
	*addr = (unsigned char)data & *addr;
	return;
}
void
S_AND_NOTD_8b(unsigned char *addr, long data)
{
	*addr = (unsigned char)data & ~(*addr);
	return;
}
void
S_ONLY_8b(unsigned char *addr, long data)
{
	*addr = (unsigned char)data;
	return;
}
void
NOTS_AND_D_8b(unsigned char *addr, long data)
{
	*addr &= (unsigned char)~data;
	return;
}
void
D_ONLY_8b(unsigned char *addr, long data)
{
	return;
}
void
S_XOR_D_8b(unsigned char *addr, long data)
{
	*addr ^= (unsigned char)data;
	return;
}
void
S_OR_D_8b(unsigned char *addr, long data)
{
	*addr |= (unsigned char)data;
	return;
}
void
NOT_SORD_8b(unsigned char *addr, long data)
{
	*addr = ~((unsigned char)data | *addr);
	return;
}
void
NOT_SXORD_8b(unsigned char *addr, long data)
{
	*addr = ~((unsigned char)data ^ *addr);
	return;
}
void
NOT_D_8b(unsigned char *addr, long data)
{
	*addr = ~*addr;
	return;
}
void
S_OR_NOTD_8b(unsigned char *addr, long data)
{
	*addr = (unsigned char)data | ~*addr;
	return;
}
void
NOT_S_8b(unsigned char *addr, long data)
{
	*addr = (unsigned char)~data;
	return;
}
void
NOTS_OR_D_8b(unsigned char *addr, long data)
{
	*addr = (unsigned char)~data | *addr;
	return;
}
void
NOT_SANDD_8b(unsigned char *addr, long data)
{
	*addr = ~((unsigned char)data & *addr);
	return;
}
void
ALL_BLACK_8b(unsigned char *addr, long data)
{
	*addr = 0xff;
	return;
}


/* *************** RASTER OPERATIONS **************** */
void ro_8b_ALL_WHITE	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_S_AND_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_S_AND_NOTD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_S_ONLY	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_NOTS_AND_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_D_ONLY	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_S_XOR_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_S_OR_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_NOT_SORD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_NOT_SXORD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_NOT_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_S_OR_NOTD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_NOT_S	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_NOTS_OR_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_NOT_SANDD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_8b_ALL_BLACK	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);

raster_op rops_8b[] =
{
	ro_8b_ALL_WHITE,
	ro_8b_S_AND_D,
	ro_8b_S_AND_NOTD,
	ro_8b_S_ONLY,
	ro_8b_NOTS_AND_D,
	ro_8b_D_ONLY,
	ro_8b_S_XOR_D,
	ro_8b_S_OR_D,
	ro_8b_NOT_SORD,
	ro_8b_NOT_SXORD,
	ro_8b_NOT_D,
	ro_8b_S_OR_NOTD,
	ro_8b_NOT_S,
	ro_8b_NOTS_OR_D,
	ro_8b_NOT_SANDD,
	ro_8b_ALL_BLACK
};



void
ro_8b_ALL_WHITE(unsigned char *srcptr, register short srcbypl,
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

void
ro_8b_S_AND_D(	unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_S_AND_NOTD(unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_S_ONLY(	unsigned char *srcptr, register short srcbypl,
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
				*dstp-- = *srcp--;

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
				*dstp++ = *srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_8b_NOTS_AND_D(unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_D_ONLY(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	return;
}
void
ro_8b_S_XOR_D(	unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_S_OR_D(	unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_NOT_SORD(	unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_NOT_SXORD(unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_NOT_D(	unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_S_OR_NOTD(unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_NOT_S(	unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_NOTS_OR_D(unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_NOT_SANDD(unsigned char *srcptr, register short srcbypl,
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
void
ro_8b_ALL_BLACK(unsigned char *srcptr, register short srcbypl,
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
