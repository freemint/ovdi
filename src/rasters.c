#include <osbind.h>

#include "display.h"
#include "draw.h"
#include "libkern.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "patch_gem.h"
#include "rasters.h"
#include "vdi_defs.h"
#include "vdi_globals.h"

extern short logit;

unsigned long clc_plen(MFDB *r);
unsigned long clc_rlen(MFDB *r);
void conv_vdi2dev( unsigned short *src, unsigned short *dst, unsigned long splen, unsigned long dplen, short planes);
void conv_dev2vdi( unsigned short *src, unsigned short *dst, unsigned long splen, unsigned long dplen, short planes);

struct xpnd
{
	void	(*exp_to)(short pen, struct xpnd *xpnd);
	long	*pixelvals;
	short	dplanes;
	short	splanes;
	short	width;
	short	height;
	short	wwidth;
	char	*src;
	char	*dst;
	short	mask;
	short	count;
};

void exp_to_1b (short pen, struct xpnd *xpnd);
void exp_to_2b (short pen, struct xpnd *xpnd);
void exp_to_4b (short pen, struct xpnd *xpnd);
void exp_to_8b (short pen, struct xpnd *xpnd);
void exp_to_16b (short pen, struct xpnd *xpnd);
void exp_to_24b (short pen, struct xpnd *xpnd);
void exp_to_32b (short pen, struct xpnd *xpnd);

void xpnd_vdif_1b(MFDB *src, struct xpnd *x);
void xpnd_vdif_2b(MFDB *src, struct xpnd *x);
void xpnd_vdif_4b(MFDB *src, struct xpnd *x);




void
exp_to_1b(short pen, struct xpnd *xpnd)
{
	short mask = xpnd->mask;
	short count = xpnd->count;
	short *dst = (short *)xpnd->dst;

	*dst++ = ((*dst)&mask) | ((pen & 1) << count);

	count--;
	mask = mask >> 1;

	if (!count)
	{
		count = 15;
		xpnd->dst += 2;
		mask = 0x8000;
	}

	xpnd->count = count;
	xpnd->mask = mask;
}

void
exp_to_2b(short pen, struct xpnd *xpnd)
{
	short mask = xpnd->mask;
	short count = xpnd->count;
	short *dst = (short *)xpnd->dst;

	*dst++ = ((*dst)&mask) | (((pen >> 1 ) & 1) << count);
	*dst = ((*dst)&mask) | ((pen & 1) << count);

	count--;
	mask = mask >> 1;

	if (!count)
	{
		count = 15;
		xpnd->dst += 2;
		mask = 0x8000;
	}

	xpnd->count = count;
	xpnd->mask = mask;
}

void
exp_to_4b(short pen, struct xpnd *xpnd)
{
	short mask = xpnd->mask;
	short count = xpnd->count;
	short *dst = (short *)xpnd->dst;

	*dst++ = ((*dst)&mask) | (((pen >> 3 ) & 1) << count);
	*dst++ = ((*dst)&mask) | (((pen >> 2 ) & 1) << count);
	*dst++ = ((*dst)&mask) | (((pen >> 1 ) & 1) << count);
	*dst   = ((*dst)&mask) | (((pen      ) & 1) << count);

	count--;
	mask = mask >> 1;

	if (!count)
	{
		count = 15;
		xpnd->dst += 4;
		mask = 0x8000;
	}

	xpnd->count = count;
	xpnd->mask = mask;
}

void
exp_to_8b(short pen, struct xpnd *xpnd)
{
	char *dst = xpnd->dst;

	*dst++ = pen;
	xpnd->dst = dst;
	return;
}

void
exp_to_16b(short pen, struct xpnd *xpnd)
{
	short *dst = (short *)xpnd->dst;

	*dst++ = xpnd->pixelvals[pen];

	xpnd->dst = (char *)dst;
	return;
}

void
exp_to_24b(short pen, struct xpnd *xpnd)
{
	long pixel;
	char *dst = xpnd->dst;

	pixel = xpnd->pixelvals[pen];

	*dst++ = (pixel >> 16) & 0xff;
	*dst++ = (pixel >> 8)  & 0xff;
	*dst++ = pixel & 0xff;

	xpnd->dst = dst;
	return;
}

void
exp_to_32b(short pen, struct xpnd *xpnd)
{
	long *dst = (long *)xpnd->dst;

	*dst++ = xpnd->pixelvals[pen];

	xpnd->dst = (char *)dst;
	return;
}

void
expand( long *pixelvalues,
	short splanes, unsigned short *src,
	short dplanes, unsigned short *dst,
	short width, short pheight )
{
	struct xpnd xpnd;
	short vdipen, x, y, i;
	unsigned short p1, p2, p3, p4, mask;

	if (dplanes == splanes)
	{
		if (dplanes < 8)
		{
			for (x = 0; x < ((( width >> 4 ) + 1) * dplanes * pheight); x++)
				*src++ = *dst++;
		}
		else if (dplanes == 8)
		{
			for (x = 0; x < (width * pheight); x++)
				*(char *)dst++ = *(char *)src++;
		}
		else if (dplanes == 15 || dplanes == 16)
		{
			for (x = 0; x < (width * pheight); x++)
				*dst++ = *src++;
		}
		else if (dplanes == 24)
		{
			for (x = 0; x < (width * pheight); x++)
			{
				*(char *)dst++ = *(char *)src++;
				*(char *)dst++ = *(char *)src++;
				*(char *)dst++ = *(char *)src++;
			}
		}
		else if (dplanes == 32)
		{
			for (x = 0; x < (width * pheight); x++)
				*(long *)dst++ = *(long *)src++;
		}
		return;
	}
		
	xpnd.splanes = splanes;
	xpnd.dplanes = dplanes;
	xpnd.src = (char *)src;
	xpnd.dst = (char *)dst;
	xpnd.count = 15;
	xpnd.mask = 0x8000;
	xpnd.width = width;
	xpnd.pixelvals = pixelvalues;

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
			xpnd.exp_to = exp_to_8b;
			break;
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
			for (x = 0; x < pheight; x++)
			{
				for (y = 0; y < (width >> 4); y++)
				{
					p1 = *src++;
					for (i = 0; i < 16; i++)
					{
						p1 = p1 >> 15 | p1 << 1;
						vdipen = p1 & 1;
						(*xpnd.exp_to)(vdipen, &xpnd);
					}
				}

				i = width & 15;

				if (i)
				{
					p1 = *src++;

					for (y = 0; y < i; y++)
					{
						p1 = p1 >> 15 | p1 << 1;
						vdipen = p1 & 1;

						(*xpnd.exp_to)(vdipen, &xpnd);
					}
				}
			}
			break;
		}
		case 2:
		{
			for (x = 0; x < pheight; x++)
			{
				for (y = 0; y < (width >> 4); y++)
				{
					mask = 0x8000;
					p1 = *src++;
					p2 = *src++;

					for (i = 0; i < 16; i++)
					{
						vdipen = ((p1 & mask)>>i)<<1 |
							 ((p2 & mask)>>i);

						(*xpnd.exp_to)(vdipen, &xpnd);
						mask >>= 1;
					}
				}

				i = width & 15;

				if (i)
				{
					mask = 0x8000;
					p1 = *src++;
					p2 = *src++;

					for (y = 0; y < i; y++)
					{
						vdipen = ((p1 & mask)>>y)<<1 |
							 ((p2 & mask)>>y);

						(*xpnd.exp_to)(vdipen, &xpnd);
						mask >>= 1;
					}
				}
			}
			break;
		}
		case 4:
		{
			for (x = 0; x < pheight; x++)
			{
				for (y = 0; y < (width >> 4); y++)
				{
					mask = 0x8000;
					p1 = *src++;
					p2 = *src++;
					p3 = *src++;
					p4 = *src++;

					for (i = 0; i < 16; i++)
					{
						vdipen = ((p1 & mask)>>i)<<3 |
							 ((p2 & mask)>>i)<<2 |
							 ((p3 & mask)>>i)<<1 |
							 ((p4 & mask)>>i);

						(*xpnd.exp_to)(vdipen, &xpnd);
						mask >>= 1;
					}
				}

				i = width & 15;

				if (i)
				{
					mask = 0x8000;
					p1 = *src++;
					p2 = *src++;
					p3 = *src++;
					p4 = *src++;

					for (y = 0; y < i; y++)
					{
						vdipen = ((p1 & mask)>>y)<<3 |
							 ((p2 & mask)>>y)<<2 |
							 ((p3 & mask)>>y)<<1 |
							 ((p4 & mask)>>y);

						(*xpnd.exp_to)(vdipen, &xpnd);
						mask >>= 1;
					}
				}
			}
			break;
		}
		case 8:
		{
			vdipen = *src;
			break;
		}
	}

}

short
fix_raster_coords(short *spts, short *dpts, short *c)
{
	short *p;
	short sx1, sy1, sx2, sy2;
	short dx1, dy1, dx2, dy2;
	short min_x, min_y, max_x, max_y;

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

static short rt2ro[] = { 3, 0, 0, 0 };

/* if wrmode has bit 15 set, wrmode is interpreted to be a vdi writing mode, not a blitblt */
void
rt_cpyfm(VIRTUAL *v, MFDB *src, MFDB *dst, short *pnts, short fgcol, short bgcol, short wrmode)
{
	int i, j, k;
	short dst_w, dst_h, dst_x1, dst_y1, planes, bypl, width, height, startbit, headbits, groups, tailbits;
	short xinc;
	short d_is_screen;
	unsigned char *addr, *adr;
	unsigned short *srcptr, *sourceptr;
	unsigned short data;
	short *pts;
	RASTER *r;
	struct ovdi_drawers *drawers;
	pixel_blit dpf_fg, dpf_bg;
	VDIRECT clip;
	short points[8];

	if (wrmode > 3 || src->fd_nplanes != 1)
		return;

	if (logit && v->func == 121)
		log("%s: rt - wrmode %d\n", v->procname, wrmode);

	wrmode <<= 1;
	r = v->raster;

	if ( dst->fd_addr == 0)
	{	/* destination screen! */

		if (r->planes == 1 && !wrmode) //rt2ro[wrmode] != 0)
		{
			ro_cpyfm(v, src, dst, pnts, rt2ro[wrmode]);
			return;
		}
			
		drawers = r->drawers;
		planes		= r->planes;
		bypl		= r->bypl;
		addr		= r->base;
		dst_w		= r->w;
		dst_h		= r->h;
		//log("sw %d, sh %d, sp %d\n", src->fd_w, src->fd_h, src->fd_nplanes);
		//log("dw %d, dh %d, dp %d\n", dst->fd_w, dst->fd_h, dst->fd_nplanes);
		d_is_screen = 1;
	}
	else
	{
		planes		= dst->fd_nplanes;
		drawers		= v->odrawers[planes];
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


	if (v->clip_flag && d_is_screen)
	{
		clip = v->clip;
		//log("clip %d, %d - %d, %d\n", clip.x1, clip.y1, clip.x2, clip.y2);
	}
	else
	{
		clip.x1 = clip.y1 = 0;
		clip.x2 = dst_w - 1;
		clip.y2 = dst_h - 1;
	}

	pts = (short *)&points;
	if ( !fix_raster_coords(pnts, pts, (short *)&clip) )
		return;

	//log("org src %d %d %d %d\n",points[0], points[1], points[2], points[3]);
	//log("org dst %d %d %d %d\n",points[4], points[5], points[6], points[7]);

	//log("clp src  %d %d %d %d\n",pts[0], pts[1], pts[2], pts[3]);
	//log("clp dst  %d %d %d %d\n",pts[4], pts[5], pts[6], pts[7]);

	dst_x1 = pts[4];
	dst_y1 = pts[5];

	if (planes < 8)
	{
		/* OH god .. how slow can we go? hehe */
		short shift, bitcount, shft, bc;


		shft = dst_x1 & 0xf;
		bc = 16 - shft;

		xinc = planes << 1;
		addr += (long)(dst_x1 >> 4) * xinc;
		addr += (long)dst_y1 * bypl;
		sourceptr = (unsigned short *)src->fd_addr;
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

		if (r->clut)
		{
			fcol = fgcol;
			bcol = bgcol;
		}
		else
		{
			fcol = r->pixelvalues[fgcol];
			bcol = r->pixelvalues[bgcol];
		}
			

		xinc = Planes2xinc[planes - 8];
		addr += (long)dst_x1 * xinc;
		addr += (long)dst_y1 * bypl;
		sourceptr = (unsigned short *)src->fd_addr;
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
	//log("\n");
	return;
}

void
ro_cpyfm(VIRTUAL *v, MFDB *src, MFDB *dst, short *pts, short wrmode)
{
	short	srcplanes, dstplanes;
	short *p;
	raster_blit rop;
	RASTER *r;
	struct ovdi_drawers *drawers;
	VDIRECT clip;
	ROP_PB	*rpb;
	ROP_PB	roppb;

	r = v->raster;
	rpb = &roppb;

	if (logit && v->func == 109)
		log("%s: ro - wrmode %d\n", v->procname, wrmode);

	if ( !dst->fd_addr || (unsigned long)dst->fd_addr == (unsigned long)r->base)
	{	/* destination screen! */
		dstplanes	= r->planes;
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

	if ( !src->fd_addr || (unsigned long)src->fd_addr == (unsigned long)r->base)
	{	/* Source is screen */
		srcplanes	= r->planes;
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
		drawers = v->odrawers[srcplanes];

	if (!drawers)
		return;

#if 0
	if (wrmode == 0 || wrmode == 15)
	{
		short interior;
		RASTER rst;

		clip = v->clip;
		v->raster = &rst;

		rst.base	= rpb->d_addr;
		rst.flags	= 0;
		rst.format	= r->format;
		rst.w		= rpb->d_w;
		rst.h		= rpb->d_h;
		rst.clut	= r->clut;
		rst.planes	= r->planes;
		rst.bypl	= rpb->d_bypl;
		rst.hpixel	= r->hpixel;
		rst.wpixel	= r->wpixel;
		rst.pixelformat = r->pixelformat;
		rst.pixelvalues = r->pixelvalues;
		rst.drawers	= drawers;

		v->clip.x1 = v->clip.y1 = 0;
		v->clip.x2 = rst.w - 1;
		v->clip.y2 = rst.h - 1;

		rpb->sx1 = pts[4];
		rpb->sy1 = pts[5];
		rpb->sx2 = pts[4] + (pts[2] - pts[0]);
		rpb->sy2 = pts[5] + (pts[3] - pts[1]);

		interior = v->fill.interior;
		v->fill.interior = FIS_SOLID;
		rectfill(v, (VDIRECT *)&rpb->sx1, wrmode == 0 ? &WhiteRect : &BlackRect);

		v->fill.interior = interior;
		v->clip = clip;
		v->raster = r;

		return;
	}
#endif

	if (v->clip_flag && rpb->d_is_scrn)
		clip = v->clip;
	else
	{
		clip.x1 = clip.y1 = 0;
		clip.x2 = rpb->d_w - 1;
		clip.y2 = rpb->d_h - 1;
	}
	
	p = (short *)&rpb->sx1;
	if ( !fix_raster_coords(pts, p, (short *)&clip) )
		return;

	rop = drawers->raster_blits[wrmode];

	if (rop)
		(*rop)(rpb);
}

typedef void (*expand_to)(short, struct xpnd *);
expand_to exp_to[] =
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

expand_from exp_from[] =
{
	0,
	xpnd_vdif_1b, /* 1b, */
	xpnd_vdif_2b, /* 2b, */
	0,
	xpnd_vdif_4b,
	0,0,0,
	0 /* 8b */
};

void
xpnd_vdif_1b(MFDB *src, struct xpnd *x)
{
	unsigned short data;
	unsigned short *srcfm;
	short i, j, k, plen, scnt, bit;

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

void
xpnd_vdif_2b(MFDB *src, struct xpnd *x)
{
	unsigned short data;
	unsigned short *srcfm;
	short i, j, k, plen, scnt, bit;

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

void
xpnd_vdif_4b(MFDB *src, struct xpnd *x)
{
	unsigned short data;
	unsigned short *srcfm;
	short i, j, k, plen, scnt, bit;

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

void
trnfm(VIRTUAL *v, MFDB *src, MFDB *dst)
{
	unsigned short *source, *dest;
	unsigned long rlen, plen;


	if (!MiNT)
		patch_gem(v->raster->planes, v->raster->w - 1);

	if (src->fd_nplanes < 8)
	{
		if (src->fd_nplanes == 1)
		{
			if (src->fd_addr != dst->fd_addr)
				memcpy(dst->fd_addr, src->fd_addr, clc_rlen(src) );
			if (!src->fd_stand)
				dst->fd_stand = 1;
			else
				dst->fd_stand = 0;
			return;
		}
#if 0
		if (src->fd_nplanes != dst->fd_nplanes)
		{
			if (src->fd_stand == 1)
			{
				struct xpnd x;
				expand_from xf;

				xf = exp_from[src->fd_nplanes];
				if (!xf)
					return;

				dst->fd_w = src->fd_w;
				dst->fd_h = src->fd_h;
				dst->fd_wdwidth = src->fd_wdwidth;
				dst->fd_stand = 0;
				dst->fd_nplanes = v->driver->r.planes;
				rlen = clc_rlen(dst);

				if (src->fd_addr == dst->fd_addr)
				{
					dest = (unsigned short *)Malloc(rlen + 4);
					if (!dest)
						return;
				}
				else
					dest = dst->fd_addr;

				x.exp_to = exp_to[v->driver->r.planes];
				x.pixelvals = v->raster->pixelvalues;
				x.dplanes = v->driver->r.planes;
				x.splanes = src->fd_nplanes;
				x.width = src->fd_w;
				x.height = src->fd_h;
				x.wwidth = src->fd_wdwidth;
				x.src = src->fd_addr;
				x.dst = (unsigned char *)dest;

				(*xf)(src, &x);

				if (src->fd_addr == dst->fd_addr)
				{
					memcpy(src->fd_addr, dest, rlen);
					Mfree(dest);
				}
			}
		}
		else 
		{
			rlen = clc_rlen(src);
			plen = clc_plen(src);

			if (src->fd_addr == dst->fd_addr)
			{
				dest = (unsigned short *)Malloc(rlen + 4);
				if (!dest)
					return;
			}
			else
				dest = dst->fd_addr;

			dst->fd_w = src->fd_w;
			dst->fd_h = src->fd_h;
			dst->fd_wdwidth = src->fd_wdwidth;
			dst->fd_nplanes = src->fd_nplanes;

			if (!src->fd_stand)		/* DEV_SPEC to VDI */
			{
				conv_dev2vdi(src->fd_addr, dest, plen, plen, src->fd_nplanes);
				dst->fd_stand = 1;
			}
			else				/* VDI to DEV_SPEC */
			{
				conv_vdi2dev(src->fd_addr, dest, plen, plen, src->fd_nplanes);
				dst->fd_stand = 0;
			}

			if (src->fd_addr == dst->fd_addr)
			{
				memcpy(src->fd_addr, dest, rlen);
				Mfree(dest);
			}
		}
#endif
	}
	else /* source planes > 8 */
	{

		rlen = clc_rlen(src);
		plen = clc_plen(src);

		if (src->fd_addr == dst->fd_addr)
		{
			dest = (unsigned short *)Malloc(rlen + 4);
			if (!dest)
				return;
		}
		else
			dest = dst->fd_addr;

		source = (unsigned short *)src->fd_addr;

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
	return;
}

unsigned long
clc_plen( MFDB *r)
{
	return ((unsigned long) (((unsigned long)r->fd_wdwidth << 1) * (unsigned long)r->fd_h));
}

unsigned long
clc_rlen( MFDB *r)
{
	return ((unsigned long)(((unsigned long)r->fd_wdwidth << 1) * (unsigned long)r->fd_h * (unsigned long)r->fd_nplanes));
}


void
conv_vdi2dev( unsigned short *src, unsigned short *dst, unsigned long splen, unsigned long dplen, short planes)
{
	short i, j;
	long len;
	unsigned long pixel;

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
			*dst++ = src[0];
			*dst++ = src[splen];
			*dst++ = src[splen << 1];
			*dst++ = src[(splen << 1) + splen];
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
				*dst++ = (unsigned short)pixel;
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
				*dst++ = (unsigned char)((unsigned long)pixel >> 16);
				*dst++ = (unsigned char)((unsigned long)pixel >> 8);
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
				*dst++ = (unsigned long)pixel;
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
conv_vdi2dev_1b( unsigned short *src, unsigned short *dst, unsigned long splen)
{
	memcpy( dst, src, splen);
	return;
}
void
conv_vdi2dev_2b( unsigned short *src, unsigned short *dst, unsigned long splen)
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
conv_vdi2dev_4b( unsigned short *src, unsigned short *dst, unsigned long splen)
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
conv_vdi2dev_8b( unsigned short *src, unsigned short *dst, unsigned long splen)
{
	short i, j;
	long len;
	unsigned short pixel;
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
conv_vdi2dev_16b( unsigned short *src, unsigned short *dst, unsigned long splen)
{
	short i, j;
	long len;
	unsigned short pixel;

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
			*dst++ = (unsigned short)pixel;
		}
		len--;
		src++;
	}
	return;
}
void
conv_vdi2dev_24b( unsigned short *src, unsigned short *dst, unsigned long splen)
{
	short i, j;
	long len;
	unsigned long pixel;
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
			*d++ = (unsigned char)((unsigned long)pixel >> 16);
			*d++ = (unsigned char)((unsigned long)pixel >> 8);
			*d++ = (unsigned char)pixel;
		}
		len--;
		src++;
	}
	return;
}
void
conv_vdi2dev_32b( unsigned short *src, unsigned short *dst, unsigned long splen)
{
	short i, j;
	long len;
	unsigned long pixel;
	unsigned long *d;

	splen >>= 1;
	len = splen;

	d = (unsigned long *)dst;

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
conv_dev2vdi( unsigned short *src, unsigned short *dst, unsigned long splen, unsigned long dplen, short planes)
{

	short i, j;
	long len;
	unsigned long pixel;

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
			dst[0] = *src++;
			dst[splen] = *src++;
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
				pixel = (unsigned short)*src++;
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
				pixel = (unsigned long)((pixel << 8) | (unsigned char)*s++);
				pixel = (unsigned long)((pixel << 8) | (unsigned char)*s++);
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
		unsigned long *s;

		s = (unsigned long *)src;

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
conv_dev2vdi_1b( unsigned short *src, unsigned short *dst, unsigned long splen)
{

	memcpy( dst, src, splen << 1);
	return;
}
void
conv_dev2vdi_2b( unsigned short *src, unsigned short *dst, unsigned long splen)
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
conv_dev2vdi_4b( unsigned short *src, unsigned short *dst, unsigned long splen)
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
conv_dev2vdi_8b( unsigned short *src, unsigned short *dst, unsigned long splen)
{
	short i, j;
	long len;
	unsigned short pixel;
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
conv_dev2vdi_16b( unsigned short *src, unsigned short *dst, unsigned long splen)
{
	short i, j;
	long len;
	unsigned short pixel;

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
conv_dev2vdi_24b( unsigned short *src, unsigned short *dst, unsigned long splen)
{
	short i, j;
	long len;
	unsigned long pixel;
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
			pixel = (unsigned long)((pixel << 8) | (unsigned char)*s++);
			pixel = (unsigned long)((pixel << 8) | (unsigned char)*s++);
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
conv_dev2vdi_32b( unsigned short *src, unsigned short *dst, unsigned long splen)
{
	short i, j;
	long len;
	unsigned long pixel;
	unsigned long *s;

	splen >>= 1;
	len = splen;

	s = (unsigned long *)src;

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
