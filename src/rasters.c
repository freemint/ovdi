#include <osbind.h>

#include "display.h"
#include "draw.h"
#include "libkern.h"
#include "ovdi_defs.h"
#include "rasters.h"
#include "vdi_defs.h"
#include "vdi_globals.h"

extern O_16 logit;

O_u32 clc_plen(MFDB *r);
O_u32 clc_rlen(MFDB *r);
void conv_vdi2dev( O_u16 *src, O_u16 *dst, O_u32 splen, O_u32 dplen, O_Int planes);
void conv_dev2vdi( O_u16 *src, O_u16 *dst, O_u32 splen, O_u32 dplen, O_Int planes);

O_Int
fix_raster_coords(O_Pos *spts, O_Pos *dpts, O_Pos *c)
{
	O_Pos *p;
	O_Pos sx1, sy1, sx2, sy2;
	O_Pos dx1, dy1, dx2, dy2;
	O_Pos min_x, min_y, max_x, max_y;

	p = spts;

	sx1 = *p++;
	sy1 = *p++;
	sx2 = *p++;
	sy2 = *p++;

	dx1 = *p++;
	dy1 = *p++;
	dx2 = *p++;
	dy2 = *p;

 /* swap coordinates, if necessary */
	if (sx1 > sx2)
	{
		max_x = sx1;
		sx1 = sx2;
		sx2 = max_x;
	}
	if (sy1 > sy2)
	{
		max_x = sy1;
		sy1 = sy2;
		sy2 = max_x;
	}

	if (dx1 > dx2)
	{
		max_x = dx1;
		dx1 = dx2;
		dx2 = max_x;
	}
	if (dy1 > dy2)
	{
		max_x = dy1;
		dy1 = dy2;
		dy2 = max_x;
	}

	min_x = *c++, min_y = *c++;
	max_x = *c++, max_y = *c;

 /* Width */
	if (dx1 < min_x)
	{
		sx1 += (min_x - dx1);

		if (sx1 > sx2)
			goto frc_fail;

		dx1 = min_x;
	}
	else if (dx1 > max_x)
	{
		goto frc_fail;
	}

	dx2 = dx1 + (sx2 - sx1);
	if (dx2 > max_x)
	{
		sx2 -= (dx2 - max_x);

		if (sx2 < 0 || sx1 > sx2)
		{
			goto frc_fail;
		}
		dx2 = max_x;
	}
/* height */
	if (dy1 < min_y)
	{
		sy1 += (min_y - dy1);

		if (sy1 > sy2)
			goto frc_fail;

		dy1 = min_y;
	}
	else if (dy1 > max_y)
	{
		goto frc_fail;
	}
	dy2 = dy1 + (sy2 - sy1);
	if (dy2 > max_y)
	{
		sy2 -= (dy2 - max_y);
		if (sy2 < 0 || sy1 > sy2)
		{
			goto frc_fail;
		}
		dy2 = max_y;
	}

	p = dpts;

	*p++ = sx1;
	*p++ = sy1;
	*p++ = sx2;
	*p++ = sy2;
	
	*p++ = dx1;
	*p++ = dy1;
	*p++ = dx2;
	*p   = dy2;

	return 1;

frc_fail:
	return 0;
}

static O_16 rt2ro[] = { 3, 0, 0, 0 };

/* if wrmode has bit 15 set, wrmode is interpreted to be a vdi writing mode, not a blitblt */
void
rt_cpyfm(RASTER *r, COLINF *c, MFDB *src, MFDB *dst, O_Pos *pnts, VDIRECT *clip, O_Int fgcol, O_Int bgcol, O_Int wrmode)
{
	int i, j, k;
	O_Pos dst_w, dst_h, dst_x1, dst_y1, width, height;
	int planes, bypl, startbit, headbits, groups, tailbits;
	int xinc;
	int d_is_screen;
	unsigned char *addr, *adr;
	O_u16 *srcptr, *sourceptr;
	O_u16 data;
	O_Pos *pts;
	struct ovdi_drawers *drawers;
	pixel_blit dpf_fg, dpf_bg;
	O_Pos clp[4];
	O_Pos points[8];

	if (wrmode > 3 || src->fd_nplanes != 1)
		return;

	wrmode <<= 1;

	if ( dst->fd_addr == 0)
	{	/* destination screen! */
		if (r->res.planes == 1 && !wrmode) //rt2ro[wrmode] != 0)
		{
			ro_cpyfm(r, src, dst, pnts, clip, rt2ro[wrmode]);
			return;
		}
		drawers 	= r->drawers;
		planes		= r->res.planes;
		bypl		= r->bypl;
		addr		= r->base;
		dst_w		= r->w;
		dst_h		= r->h;
		d_is_screen = 1;
	}
	else
	{
		planes		= dst->fd_nplanes;
		drawers		= r->odrawers[planes];
		bypl		= (dst->fd_wdwidth << 1) * planes;
		addr		= dst->fd_addr;
		dst_w		= dst->fd_w;
		dst_h		= dst->fd_h;
		d_is_screen	= 0;
	}

	if (drawers)
	{
		dpf_fg	= drawers->drp[wrmode];
		dpf_bg	= drawers->drp[wrmode + 1];
	}
	else
		return;


	pts = (O_Pos *)&clp;
	if (d_is_screen)
	{
		*pts++	= clip->x1;
		*pts++	= clip->y1; 
		*pts++	= clip->x2;
		*pts	= clip->y2;
	}
	else
	{
		*pts++	= 0;
		*pts++	= 0; 
		*pts++	= dst_w - 1;
		*pts	= dst_h - 1;
	}

	pts = (O_Pos *)&points;
	if ( !fix_raster_coords(pnts, pts, (O_Pos *)&clp[0]) )
		return;

	dst_x1 = pts[4];
	dst_y1 = pts[5];

	if (planes < 8)
	{
		/* OH god .. how slow can we go? hehe */
		int shift, bitcount, shft, bc;


		shft = dst_x1 & 0xf;
		bc = 16 - shft;

		xinc = planes << 1;
		addr += (long)(dst_x1 >> 4) * xinc;
		addr += (long)dst_y1 * bypl;
		sourceptr = (O_u16 *)src->fd_addr;
		sourceptr += pts[0] >> 4;
		sourceptr += (long)pts[1] * src->fd_wdwidth;

		height = (pts[3] - pts[1]) + 1;
		width = (pts[2] - pts[0]) + 1;
		startbit = pts[0] & 0xf;
		headbits = (16 - startbit) & 0xf;

		width -= headbits;
		
		if (width <= 0)
		{
			headbits = headbits + width;
			groups = tailbits = 0;
		}
		else if (width > 15)
		{
			tailbits = (pts[2] + 1) & 0xf;
			groups = (width - tailbits) >> 4;
		}
		else
		{
			groups = 0;
			tailbits = width;
		}

		
		if (d_is_screen)
			SYNC_RASTER(r);

		for (i = 0; i < height; i++) //i = height; i > 0; i--)
		{

			srcptr = sourceptr;
			adr = addr;

			bitcount = bc;
			shift = shft;

			if (startbit || headbits)
			{
				data = *srcptr++ << startbit;
				for (j = headbits; j > 0; j--)
				{
					if (data & 0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(adr, (long)shift<<16 | fgcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(adr, (long)shift << 16 | bgcol);

					data <<= 1;

					bitcount--;

					if (bitcount <= 0)
					{
						bitcount = 16;
						adr += xinc;
						shift = 0;
					}
					else
						shift++;
				}
			}
			if (groups)
			{
				for (j = 0; j < groups; j++)
				{
					data = *srcptr++;

					for (k = 16; k > 0; k--)
					{
						if (data &0x8000)
						{
							if (dpf_fg)
								(*dpf_fg)(adr, (long)shift << 16 | fgcol);
						}
						else if (dpf_bg)
							(*dpf_bg)(adr, (long)shift << 16 | bgcol);

						data <<= 1;

						bitcount--;

						if (bitcount <= 0)
						{
							bitcount = 16;
							adr += xinc;
							shift = 0;
						}
						else
							shift++;
					}
				}
			}
			if (tailbits)
			{
				data = *srcptr;
				for (j = tailbits; j > 0; j--)
				{
					if (data &0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(adr, (long)shift << 16 | fgcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(adr, (long)shift << 16 | bgcol);

					data <<= 1;

					bitcount--;

					if (bitcount <= 0)
					{
						bitcount = 16;
						adr += xinc;
						shift = 0;
					}
					else
						shift++;
				}
			}
			sourceptr += src->fd_wdwidth;
			addr += bypl;
		}
	}
	else	/* (planes >= 8) */
	{
		register long fcol, bcol;

		if (r->res.clut)
		{
			fcol = fgcol;
			bcol = bgcol;
		}
		else
		{
			fcol = c->pixelvalues[fgcol];
			bcol = c->pixelvalues[bgcol];
		}
			

		xinc = Planes2xinc[planes - 8];
		addr += (long)dst_x1 * xinc;
		addr += (long)dst_y1 * bypl;
		sourceptr = (O_u16 *)src->fd_addr;
		sourceptr += pts[0] >> 4;
		sourceptr += (long)pts[1] * src->fd_wdwidth;

		height = (pts[3] - pts[1]) + 1;
		width = (pts[2] - pts[0]) + 1;
		startbit = pts[0] & 0xf;
		headbits = (16 - startbit) & 0xf;

		width -= headbits;
		
		if (width <= 0)
		{
			headbits = headbits + width;
			groups = tailbits = 0;
		}
		else if (width > 15)
		{
			tailbits = (pts[2] + 1) & 0xf;
			groups = (width - tailbits) >> 4;
		}
		else
		{
			groups = 0;
			tailbits = width;
		}

		if (d_is_screen)
			SYNC_RASTER(r);

		for (i = 0; i < height; i++) //i = height; i > 0; i--)
		{

			srcptr = sourceptr;
			adr = addr;

			if (startbit || headbits)
			{
				data = *srcptr++ << startbit;
				for (j = headbits; j > 0; j--)
				{
					if (data & 0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(adr, fcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(adr, bcol);

					data <<= 1;
					adr += xinc;
				}
			}
			if (groups)
			{
				for (j = 0; j < groups; j++)
				{
					data = *srcptr++;

					for (k = 16; k > 0; k--)
					{
						if (data &0x8000)
						{
							if (dpf_fg)
								(*dpf_fg)(adr, fcol);
						}
						else if (dpf_bg)
							(*dpf_bg)(adr, bcol);

						data <<= 1;
						adr += xinc;
					}
				}
			}
			if (tailbits)
			{
				data = *srcptr;
				for (j = tailbits; j > 0; j--)
				{
					if (data &0x8000)
					{
						if (dpf_fg)
							(*dpf_fg)(adr, fcol);
					}
					else if (dpf_bg)
						(*dpf_bg)(adr, bcol);

					data <<= 1;
					adr += xinc;
				}
			}
			sourceptr += src->fd_wdwidth;
			addr += bypl;
		}
	}
}

void
ro_cpyfm(RASTER *r, MFDB *src, MFDB *dst, O_Pos *pts, VDIRECT *clip, O_Int wrmode)
{
	int srcplanes, dstplanes;
	O_Pos *p;
	raster_blit rop;
	struct ovdi_drawers *drawers;
	ROP_PB	*rpb;
	ROP_PB	roppb;
	O_Pos	clp[4];

	rpb = &roppb;

	if ( !dst->fd_addr || (O_u32)dst->fd_addr == (O_u32)r->base)
	{	/* destination screen! */
		dstplanes	= r->res.planes;
		rpb->d_bypl	= r->bypl;
		rpb->d_addr	= r->base;
		rpb->d_w	= r->w;
		rpb->d_h	= r->h;
		rpb->d_is_scrn	= 1;
	}
	else
	{
		dstplanes	= dst->fd_nplanes;
		rpb->d_bypl	= (dst->fd_wdwidth << 1) * dst->fd_nplanes;
		rpb->d_addr	= dst->fd_addr;
		rpb->d_w	= dst->fd_w;
		rpb->d_h	= dst->fd_h;
		rpb->d_is_scrn	= 0;
		
	}

	if ( !src->fd_addr || (O_u32)src->fd_addr == (O_u32)r->base)
	{	/* Source is screen */
		srcplanes	= r->res.planes;
		rpb->s_bypl	= r->bypl;
		rpb->s_addr	= r->base;
		rpb->s_w	= r->w;
		rpb->s_h	= r->h;
		rpb->s_is_scrn	= 1;
	}
	else
	{
		srcplanes	= src->fd_nplanes;
		rpb->s_bypl	= (src->fd_wdwidth << 1) * src->fd_nplanes;
		rpb->s_addr	= src->fd_addr;
		rpb->s_w	= src->fd_w;
		rpb->s_h	= src->fd_h;
		rpb->s_is_scrn	= 0;
	}
	
	if (dstplanes != srcplanes)
		return;
	else if (rpb->d_is_scrn || rpb->s_is_scrn)
		drawers = r->drawers;
	else
		drawers = r->odrawers[srcplanes];

	if (!drawers)
		return;

	p = (O_Pos *)&clp;
	if (rpb->d_is_scrn)
	{
		*p++	= clip->x1;
		*p++	= clip->y1;
		*p++	= clip->x2;
		*p	= clip->y2;
	}
	else
	{
		*p++	= 0;
		*p++	= 0;
		*p++	= rpb->d_w - 1;
		*p	= rpb->d_h - 1;
	}
	
	p = (O_Pos *)&rpb->sx1;
	if ( !fix_raster_coords(pts, p, (O_Pos *)&clp) )
		return;

	rop = drawers->raster_blits[wrmode];

#if 0
	if (rpb->d_is_scrn)
		SYNC_RASTER(r);
#endif
	if (rop)
	{
		if (rpb->d_is_scrn)
			SYNC_RASTER(r);
		(*rop)(rpb);
	}
}

void
trnfm(MFDB *src, MFDB *dst)
{
	O_u16 *source, *dest;
	O_u32 rlen, plen;

	if (src->fd_nplanes == 1)
	{
		if (src->fd_addr != dst->fd_addr)
			memcpy(dst->fd_addr, src->fd_addr, clc_rlen(src) );
		if (!src->fd_stand)
			dst->fd_stand = 1;
		else
			dst->fd_stand = 0;
	}
	else /* source planes != 1 */
	{
		rlen = clc_rlen(src);
		plen = clc_plen(src);

		if (src->fd_addr == dst->fd_addr)
		{
			dest = (O_u16 *)Malloc(rlen + 4);
			if (!dest)
				return;
		}
		else
			dest = dst->fd_addr;

		source = (O_u16 *)src->fd_addr;

		if (!src->fd_stand)		/* DEV_SPEC to VDI */
		{
			conv_dev2vdi(source, dest, plen, plen, src->fd_nplanes);
			dst->fd_stand = 1;
			
		}
		else				/* VDI to DEV_SPEC */
		{
			conv_vdi2dev(source, dest, plen, plen, src->fd_nplanes);
			dst->fd_stand = 0;
		}

		dst->fd_w	= src->fd_w;
		dst->fd_h	= src->fd_h;
		dst->fd_wdwidth	= src->fd_wdwidth;
		dst->fd_nplanes	= src->fd_nplanes;

		if (src->fd_addr == dst->fd_addr)
		{
			memcpy(src->fd_addr, dest, rlen);
			Mfree(dest);
		}
	}
}

O_u32
clc_plen( MFDB *r)
{
	return ((O_u32) (((O_u32)r->fd_wdwidth << 1) * (O_u32)r->fd_h));
}

O_u32
clc_rlen( MFDB *r)
{
	return ((O_u32)(((O_u32)r->fd_wdwidth << 1) * (O_u32)r->fd_h * (O_u32)r->fd_nplanes));
}


void
conv_vdi2dev( O_u16 *src, O_u16 *dst, O_u32 splen, O_u32 dplen, O_Int planes)
{
	int i, j;
	O_32 len;
	O_u32 pixel;

	splen >>= 1;
	len = splen;

	if (planes == 1)
	{
		memcpy( dst, src, splen << 1);
	}
	else if (planes == 2)
	{
		while (len > 0)
		{
			*dst++ = src[0];
			*dst++ = src[splen];
			src++;
			len--;
		}
			
	}
	else if (planes == 4)
	{
		while (len > 0)
		{
			*(long *)((long *)dst)++ = (O_u32)((long)src[0] << 16) | src[splen];
			*(long *)((long *)dst)++ = (O_u32)((long)src[splen <<1 ] << 16) | src[splen * 3];
#if 0
			*dst++ = src[0];
			*dst++ = src[splen];
			*dst++ = src[splen << 1];
			*dst++ = src[(splen << 1) + splen];
#endif
			src++;
			len--;
		}
	}
	else if (planes == 8)
	{
		unsigned char *d;

		d = (unsigned char *)dst;

		while (len > 0)
		{
			for (j = 15; j > -1; j--)
			{
				pixel = 0;
				for (i = 0; i < 8; i++)
				{
					pixel >>= 1;
					pixel |= (src[splen * i] & (1 << j)) << (15 - j);
				}
				*d++ = (unsigned char)(pixel >> 8);
			}
			len--;
			src++;
		}
	}
	else if (planes == 16)
	{
		while (len > 0)
		{
			for (j = 15; j > -1; j--)
			{
				pixel = 0;
				for (i = 0; i < 16; i++)
				{
					pixel >>= 1;
					pixel |= ( (src[splen * i] & (1 << j)) << (15 - j) );
				}
				*dst++ = (O_u16)pixel;
			}
			len--;
			src++;
		}
	}
	else if (planes == 24)
	{
		while (len > 0)
		{
			for (j = 15; j > -1; j--)
			{
				pixel = 0;
				for (i = 0; i < 24; i++)
				{
					pixel >>= 1;
					pixel |= ( (src[splen * i] & (1 << j)) << (15 - j) );
				}
				*dst++ = (unsigned char)((O_u32)pixel >> 16);
				*dst++ = (unsigned char)((O_u32)pixel >> 8);
				*dst++ = (unsigned char)pixel;
			}
			len--;
			src++;
		}
	}
	else if (planes == 32)
	{
		while (len > 0)
		{
			for (j = 15; j > -1; j--)
			{
				pixel = 0;
				for (i = 0; i < 32; i++)
				{
					pixel >>= 1;
					pixel |= ( (src[splen * i] & (1 << j)) << (15 - j) );
				}
				*dst++ = (O_u32)pixel;
			}
			len--;
			src++;
		}
	}
	return;
}
/************************************************************************/
/************************************************************************/
void
conv_vdi2dev_1b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	memcpy( dst, src, splen);
	return;
}
void
conv_vdi2dev_2b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	long len;

	splen >>= 1;
	len = splen;

	while (len > 0)
	{
		*dst++ = src[0];
		*dst++ = src[splen];
		src++;
		len--;
	}
	return;
}
void
conv_vdi2dev_4b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	long len;

	splen >>= 1;
	len = splen;

	while (len > 0)
	{
		*dst++ = src[0];
		*dst++ = src[splen];
		*dst++ = src[splen << 1];
		*dst++ = src[(splen << 1) + splen];
		src++;
		len--;
	}
	return;
}
void
conv_vdi2dev_8b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	int i, j;
	long len;
	O_u16 pixel;
	unsigned char *d;

	splen >>= 1;
	len = splen;

	d = (unsigned char *)dst;

	while (len > 0)
	{
		for (j = 15; j > -1; j--)
		{
			pixel = 0;
			for (i = 0; i < 8; i++)
			{
				pixel >>= 1;
				pixel |= (src[splen * i] & (1 << j)) << (15 - j);
			}
			*d++ = (unsigned char)(pixel >> 8);
		}
		len--;
		src++;
	}
	return;
}
void
conv_vdi2dev_16b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	int i, j;
	long len;
	O_u16 pixel;

	splen >>= 1;
	len = splen;

	while (len > 0)
	{
		for (j = 15; j > -1; j--)
		{
			pixel = 0;
			for (i = 0; i < 16; i++)
			{
				pixel >>= 1;
				pixel |= ( (src[splen * i] & (1 << j)) << (15 - j) );
			}
			*dst++ = (O_u16)pixel;
		}
		len--;
		src++;
	}
	return;
}
void
conv_vdi2dev_24b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	int i, j;
	long len;
	O_u32 pixel;
	unsigned char *d;

	splen >>= 1;
	len = splen;

	d = (unsigned char *)dst;

	while (len > 0)
	{
		for (j = 15; j > -1; j--)
		{
			pixel = 0;
			for (i = 0; i < 24; i++)
			{
				pixel >>= 1;
				pixel |= ( (src[splen * i] & (1 << j)) << (15 - j) );
			}
			*d++ = (unsigned char)((O_u32)pixel >> 16);
			*d++ = (unsigned char)((O_u32)pixel >> 8);
			*d++ = (unsigned char)pixel;
		}
		len--;
		src++;
	}
	return;
}
void
conv_vdi2dev_32b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	int i, j;
	long len;
	O_u32 pixel;
	O_u32 *d;

	splen >>= 1;
	len = splen;

	d = (O_u32 *)dst;

	while (len > 0)
	{
		for (j = 15; j > -1; j--)
		{
			pixel = 0;
			for (i = 0; i < 32; i++)
			{
				pixel >>= 1;
				pixel |= ( (src[splen * i] & (1 << j)) << (15 - j) );
			}
			*d++ = pixel;
		}
		len--;
		src++;
	}
	return;
}
/************************************************************************/
/************************************************************************/
void
conv_dev2vdi( O_u16 *src, O_u16 *dst, O_u32 splen, O_u32 dplen, O_Int planes)
{

	int i, j;
	long len;
	O_u32 pixel;

	splen >>= 1;
	len = splen;

	if (planes == 1)
	{
		memcpy( dst, src, splen << 1);
	}
	else if (planes == 2)
	{
		while (len > 0)
		{

			dst[0] = *src++;
			dst[splen] = *src++;
			dst++;
			len--;
		}
	}
	else if (planes == 4)
	{
		while (len > 0)
		{
			dst[0]		= *src++;
			dst[splen]	= *src++;
			dst[splen << 1] = *src++;
			dst[(splen << 1) + splen] = *src++;
			dst++;
			len--;
		}
	}
	else if (planes == 8)
	{
		unsigned char *s;

		s = (unsigned char *)src;

		while (len > 0)
		{
			for (i = 0; i < 8; i++)
				dst[splen * i] = 0;

			for (j = 15; j > -1; j--)
			{
				(unsigned char)pixel = *s++;
				for (i = 0; i < 8; i++)
				{
					dst[splen * i] |= (pixel & 1) << j;
					pixel >>= 1;
				}
			}
			dst++;
			len--;
		}
	}
	else if (planes == 16)
	{
		while (len > 0)
		{
			for (i = 0; i < 16; i++)
				dst[splen * i] = 0;

			for (j = 15; j > -1; j--)
			{
				pixel = (O_u16)*src++;
				for (i = 0; i < 16; i++)
				{
					dst[splen * i] |= (pixel & 1) << j;
					pixel >>= 1;
				}
			}
			dst++;
			len--;
		}
	}
	else if (planes == 24)
	{
		unsigned char *s;

		s = (unsigned char *)src;

		while (len > 0)
		{
			for (i = 0; i < 24; i++)
				dst[splen * i] = 0;

			for (j = 15; j > -1; j--)
			{
				pixel = (unsigned char)*s++;
				pixel = (O_u32)((pixel << 8) | (unsigned char)*s++);
				pixel = (O_u32)((pixel << 8) | (unsigned char)*s++);
				for (i = 0; i < 24; i++)
				{
					dst[splen * i] |= (pixel & 1) << j;
					pixel >>= 1;
				}
			}
			dst++;
			len--;
		}
	}
	else if (planes == 32)
	{
		O_u32 *s;

		s = (O_u32 *)src;

		while (len > 0)
		{
			for (i = 0; i < 32; i++)
				dst[splen * i] = 0;

			for (j = 15; j > -1; j--)
			{
				pixel = *s++;
				for (i = 0; i < 32; i++)
				{
					dst[splen * i] |= (pixel & 1) << j;
					pixel >>= 1;
				}
			}
			dst++;
			len--;
		}
	}
	return;
}
/************************************************************************/
/************************************************************************/
void
conv_dev2vdi_1b( O_u16 *src, O_u16 *dst, O_u32 splen)
{

	memcpy( dst, src, splen << 1);
	return;
}
void
conv_dev2vdi_2b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	long len;

	splen >>= 1;
	len = splen;

	while (len > 0)
	{
		dst[0] = *src++;
		dst[splen] = *src++;
		dst++;
		len--;
	}
	return;
}
void
conv_dev2vdi_4b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	long len;

	splen >>= 1;
	len = splen;

	while (len > 0)
	{
		dst[0] = *src++;
		dst[splen] = *src++;
		dst[splen << 1] = *src++;
		dst[(splen << 1) + splen] = *src++;
		dst++;
		len--;
	}
	return;
}
void
conv_dev2vdi_8b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	int i, j;
	long len;
	O_u16 pixel;
	unsigned char *s;

	splen >>= 1;
	len = splen;

	s = (unsigned char *)src;

	while (len > 0)
	{
		for (i = 0; i < 8; i++)
			dst[splen * i] = 0;

		for (j = 15; j > -1; j--)
		{
			(unsigned char)pixel = *s++;
			for (i = 0; i < 8; i++)
			{
				dst[splen * i] |= (pixel & 1) << j;
				pixel >>= 1;
			}
		}
		dst++;
		len--;
	}
	return;
}
void
conv_dev2vdi_16b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	int i, j;
	long len;
	O_u16 pixel;

	splen >>= 1;
	len = splen;

	while (len > 0)
	{
		for (i = 0; i < 16; i++)
			dst[splen * i] = 0;

		for (j = 15; j > -1; j--)
		{
			pixel = *src++;
			for (i = 0; i < 16; i++)
			{
				dst[splen * i] |= (pixel & 1) << j;
				pixel >>= 1;
			}
		}
		dst++;
		len--;
	}
	return;
}
void
conv_dev2vdi_24b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	int i, j;
	long len;
	O_u32 pixel;
	unsigned char *s;

	splen >>= 1;
	len = splen;

	s = (unsigned char *)src;

	while (len > 0)
	{
		for (i = 0; i < 24; i++)
			dst[splen * i] = 0;

		for (j = 15; j > -1; j--)
		{
			pixel = (unsigned char)*s++;
			pixel = (O_u32)((pixel << 8) | (unsigned char)*s++);
			pixel = (O_u32)((pixel << 8) | (unsigned char)*s++);
			for (i = 0; i < 24; i++)
			{
				dst[splen * i] |= (pixel & 1) << j;
				pixel >>= 1;
			}
		}
		dst++;
		len--;
	}
	return;
}
void
conv_dev2vdi_32b( O_u16 *src, O_u16 *dst, O_u32 splen)
{
	int i, j;
	long len;
	O_u32 pixel;
	O_u32 *s;

	splen >>= 1;
	len = splen;

	s = (O_u32 *)src;

	while (len > 0)
	{
		for (i = 0; i < 32; i++)
			dst[splen * i] = 0;

		for (j = 15; j > -1; j--)
		{
			pixel = *s++;
			for (i = 0; i < 32; i++)
			{
				dst[splen * i] |= (pixel & 1) << j;
				pixel >>= 1;
			}
		}
		dst++;
		len--;
	}
	return;
}
/************************************************************************/
/************************************************************************/
