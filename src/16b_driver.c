#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "16b_driver.h"

void dlp_REP_fg_16b(unsigned char *addr, long data);
void dlp_REP_bg_16b(unsigned char *addr, long data);
void dlp_TRANS_fg_16b(unsigned char *addr, long data);
void dlp_XOR_fg_16b(unsigned char *addr, long data);
void dlp_XOR_bg_16b(unsigned char *addr, long data);
void dlp_ERAS_bg_16b(unsigned char *addr, long data);


draw_pixel dpf_16b[] =
{
	dlp_REP_fg_16b,
	dlp_REP_bg_16b,

	dlp_TRANS_fg_16b,
	0,

	dlp_XOR_fg_16b,
	dlp_XOR_bg_16b,

	0,
	dlp_ERAS_bg_16b
};
	
/* ************* REPLACE ********** */
void
dlp_REP_fg_16b(unsigned char *addr, long data)
{
	*(unsigned short *)addr = (unsigned short)data;
}
void
dlp_REP_bg_16b(unsigned char *addr, long data)
{
	*(unsigned short *)addr = (unsigned short)data;
}

/* **************** TRANSPARENT *********** */
void
dlp_TRANS_fg_16b(unsigned char *addr, long data)
{
	*(unsigned short *)addr = (unsigned short)data;
}
/* **************** XOR ********** */

void
dlp_XOR_fg_16b(unsigned char *addr, long data)
{
	*(unsigned short *)addr ^= (unsigned short)data;
}
void
dlp_XOR_bg_16b(unsigned char *addr, long data)
{
	*(unsigned short *)addr ^= (unsigned short)data;
}

/* *************** ERASE ************ */
void
dlp_ERAS_bg_16b(unsigned char *addr, long data)
{
	*(unsigned short *)addr = (unsigned short)data;
}

/* *************** RASTER OPERATIONS **************** */
void ro_16b_ALL_WHITE	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_S_AND_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_S_AND_NOTD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_S_ONLY	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_NOTS_AND_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_D_ONLY	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_S_XOR_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_S_OR_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_NOT_SORD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_NOT_SXORD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_NOT_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_S_OR_NOTD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_NOT_S	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_NOTS_OR_D	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_NOT_SANDD	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
void ro_16b_ALL_BLACK	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);

raster_op rops_16b[] =
{
	ro_16b_ALL_WHITE,
	ro_16b_S_AND_D,
	ro_16b_S_AND_NOTD,
	ro_16b_S_ONLY,
	ro_16b_NOTS_AND_D,
	ro_16b_D_ONLY,
	ro_16b_S_XOR_D,
	ro_16b_S_OR_D,
	ro_16b_NOT_SORD,
	ro_16b_NOT_SXORD,
	ro_16b_NOT_D,
	ro_16b_S_OR_NOTD,
	ro_16b_NOT_S,
	ro_16b_NOTS_OR_D,
	ro_16b_NOT_SANDD,
	ro_16b_ALL_BLACK
};


void
ro_16b_ALL_WHITE(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short /**srcp,*/ *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			//srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = 0xffff;

			//srcptr += srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			//srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = 0xffff;

			//srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}

void
ro_16b_S_AND_D(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

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

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ &= *srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_S_AND_NOTD(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

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

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = *srcp++ & (~*dstp);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_S_ONLY(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

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

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = *srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_NOTS_AND_D(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- &= ~*srcp--;

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ &= ~*srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_D_ONLY(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	return;
#if 0
	register unsigned short *srcp, *dstp;

	for (i = height; i > 0; i--)
	{

		srcp = (unsigned short *)srcptr;
		dstp = (unsigned short *)dstptr;

		for (j = width; j > 0; j--)
			*dstp++ = *srcp++;

		srcptr += srcbypl;
		dstptr += dstbypl;
	}
#endif
}
void
ro_16b_S_XOR_D(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

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

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ ^= *srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_S_OR_D(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

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

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ |= *srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_NOT_SORD(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

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

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = ~(*srcp++ | *dstp);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_NOT_SXORD(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

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

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = ~(*srcp++ ^ *dstp);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_NOT_D(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short /**srcp,*/ *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

//			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp = ~*dstp--;

//			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

//			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp = ~*dstp++;

//			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_S_OR_NOTD(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = *srcp-- | ~*dstp;

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = *srcp++ | ~*dstp;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_NOT_S(	unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp-- = ~*srcp--;

			srcptr -= srcbypl;
			dstptr -= dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = ~*srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_NOTS_OR_D(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ |= ~*srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
	else
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ |= ~*srcp++;

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_NOT_SANDD(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short *srcp, *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

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

			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = ~(*srcp++ & *dstp);

			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
void
ro_16b_ALL_BLACK(unsigned char *srcptr, register short srcbypl,
		unsigned char *dstptr, register short dstbypl,
		short width, short height, short dir)
{
	register unsigned short /**srcp,*/ *dstp;
	register short i, j;

	if (dir)
	{
		for (i = height; i > 0; i--)
		{

//			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

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

//			srcp = (unsigned short *)srcptr;
			dstp = (unsigned short *)dstptr;

			for (j = width; j > 0; j--)
				*dstp++ = 0;

//			srcptr += srcbypl;
			dstptr += dstbypl;
		}
	}
}
