#include "../../ovdi_defs.h"
#include "acl.h"
#include "inlines.h"

void rb_S_ONLY_1b(ROP_PB *);


static void
et6k_S_ONLY_1b(ROP_PB *rpb)
{
	register volatile unsigned char *et6k;
	unsigned char *src, *dst;
	short w, h, sbpl, dbpl;
	
	et6k = et6kptr;

	w = (rpb->sx2 - rpb->sx1);
	h = (rpb->sy2 - rpb->sy1);
	sbpl = rpb->s_bypl << 2;
	dbpl = rpb->d_bypl << 2;

	src = (unsigned char *)rpb->s_addr + (long)(rpb->sx1 >> 3) + (long)((long)rpb->sy1 * sbpl);
	dst = (unsigned char *)rpb->d_addr + (long)(rpb->dx1 >> 3) + (long)((long)rpb->dy1 * dbpl);

	outb( et6k, ACL_FOREGROUND_RASTER_OPERATION, 204 );
	outb( et6k, ACL_STEPPING_INHIBIT, 0);
	outw( et6k, ACL_X_COUNT, w);
	outw( et6k, ACL_Y_COUNT, h - 1);
	outw( et6k, ACL_DESTINATION_Y_OFFSET, dbpl - 1); //rpb->d_bypl - 1);
	outw( et6k, ACL_SOURCE_Y_OFFSET, sbpl - 1); //rpb->s_bypl - 1);

	if ((long)src < (long)dst)
	{
		outb( et6k, ACL_XY_DIRECTION, 3);
		src = (unsigned char *)rpb->s_addr + (long)(rpb->sx2 >> 3) + (long)((long)rpb->sy2 * sbpl);
		dst = (unsigned char *)rpb->d_addr + (long)(rpb->dx2 >> 3) + (long)((long)rpb->dy2 * dbpl);
	}
	else
		outb( et6k, ACL_XY_DIRECTION, 0);

	outl( et6k, ACL_SOURCE_ADDRESS, src);
	outb( et6k, ACL_SOURCE_WRAP, 0x77);
	outl( et6k, ACL_DESTINATION_ADDRESS, dst);
}

void
rb_S_ONLY_1b(ROP_PB *rpb)
{
	short i, width, height, s_shift, d_shift, shift, hbits, ebits, groups, spans, sbpl, dbpl;
	unsigned short begmask, endmask, data;
	register unsigned short *src, *dst, *s, *d;
	
	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sbpl	= rpb->s_bypl >> 1;
	dbpl	= rpb->d_bypl >> 1;

	src = (unsigned short *)rpb->s_addr + (long)(rpb->sx1 >> 4) + (long)((long)rpb->sy1 * sbpl);
	dst = (unsigned short *)rpb->d_addr + (long)(rpb->dx1 >> 4) + (long)((long)rpb->dy1 * dbpl);

	if (src < dst)
	{
		src = (unsigned short *)rpb->s_addr + (long)(rpb->sx2 >> 4) + (long)((long)rpb->sy2 * sbpl);
		dst = (unsigned short *)rpb->d_addr + (long)(rpb->dx2 >> 4) + (long)((long)rpb->dy2 * dbpl);

		s_shift	= 15 - (rpb->sx2 & 0xf);
		d_shift	= 15 - (rpb->dx2 & 0xf);

		if (!s_shift && !d_shift)
		{
			if (rpb->s_is_scrn && rpb->d_is_scrn)
			{
				et6k_S_ONLY_1b(rpb);
				return;
			}


		/* source & destination starts on whole word */

			endmask = 0xffff >> (rpb->sx1 & 0xf);
			groups = width >> 4;
			dst++;
			src++;

			for (; height > 0; height--)
			{
				s = src;
				d = dst;

				for (i = groups >> 3; i > 0; i--)
				{
					*(long *)--((long *)d) = *(long *)--((long *)s);
					*(long *)--((long *)d) = *(long *)--((long *)s);
					*(long *)--((long *)d) = *(long *)--((long *)s);
					*(long *)--((long *)d) = *(long *)--((long *)s);
				}
				for (i = groups & 7; i > 0; i--)
					*--d = *--s;
				if (endmask != 0xffff)
					*--d = (*--s & endmask) | (*d & ~endmask);

#if 0
				for (i = width >> 4; i > 0; i--)
					*d-- = *s--;
				if (endmask != 0xffff)
					*d-- = (*s-- & endmask) | (*d & ~endmask);
#endif
				src -= sbpl;
				dst -= dbpl;
			}
		}
		else if (!(shift = s_shift - d_shift))
		{
		/* Same shift for both source and dest */
			begmask	= 0xffff << s_shift;
			endmask = 0xffff >> (rpb->sx1 & 0xf);
			hbits	= (16 - s_shift) & 0xf;
			width	-= hbits;

			if (width <= 0)
			{
				hbits = width + hbits;
				groups = ebits = 0;
				if (width)
					begmask &= endmask;
			}
			else if (width > 15)
			{
				ebits = (16 - (rpb->sx1 & 0xf)) & 0xf;
				groups = (width - ebits) >> 4;
			}
			else
			{
				groups = 0;
				ebits = width;
			}

			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				if (hbits)
					*d-- = (*s-- & begmask) | (*d & ~begmask);
				for (i = groups; i > 0; i--)
					*d-- = *s--;
				if (ebits)
					*d-- = (*s-- & endmask) | (*d & ~endmask);

				src -= sbpl;
				dst -= dbpl;
			}			
		}
		else if (shift > 0)
		{
		/* shift source right */
			begmask = 0xffff << d_shift;
			endmask = 0xffff >> (rpb->dx1 & 0xf);
			hbits	= (16 - d_shift) & 0xf;
			width	-= hbits;

			if (width <= 0)
			{
				hbits = width + hbits;
				groups = ebits = 0;
				if (width)
					begmask &= endmask;
			}
			else if (width > 15)
			{
				ebits = (16 - (rpb->dx1 & 0xf)) & 0xf;
				groups = (width - ebits) >> 4;
			}
			else
			{
				groups = 0;
				ebits = width;
			}

			for (; height > 0; height--)
			{
				s = src;
				d = dst;

				if (hbits)
				{
					data = *s-- >> shift;
					if ((s_shift + hbits) > 16) //spans > 16)
						data |= *s << (16 - shift);
					*d-- = (data & begmask) | (*d & ~ begmask);

				}

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data = *s-- >> shift;
						data |= *s << (16 - shift);
						*d-- = data;
					}
				}

				if (ebits)
				{
					data = *s-- >> shift;
					if (s_shift + ebits > 16) //spans < ebits)
						data |= *s << (16 - shift);
					*d-- = (data & endmask) | (*d & ~endmask);
				}
				src -= sbpl;
				dst -= dbpl;
			}		
		}
		else
		{
			unsigned short data1;

			/* shift source left */
			shift	= -shift;
			begmask = 0xffff << d_shift;
			endmask = 0xffff >> (rpb->dx1 & 0xf);
			hbits	= (16 - d_shift) & 0xf;
			width	-= hbits;

			if (width <= 0)
			{
				hbits = width + hbits;
				groups = ebits = 0;
				if (width)
					begmask &= endmask;
			}
			else if (width > 15)
			{
				ebits = (16 - (rpb->dx1 & 0xf)) & 0xf;
				groups = (width - ebits) >> 4;
			}
			else
			{
				groups = 0;
				ebits = width;
			}

			spans = ebits - shift;

			if (spans < 0)
				spans = 0;

			for (; height > 0; height--)
			{
				s = src;
				d = dst;

				if (hbits)
				{
					data = *s--;
					*d-- = ((data << shift) & begmask) | (*d & ~begmask);
				}
				else
					data = 0;

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data >>= 16 - shift;
						data1 = *s--;
						data |= data1 << shift; //(*s << shift);
						*d-- = data;
						data = data1;
					}
				}

				if (ebits)
				{
					data >>= 16 - shift;
					if (spans)
						data |= *s-- << shift;
					*d-- = (data & endmask) | (*d & ~endmask);
				}
				src -= sbpl;
				dst -= dbpl;
			}
		}
	}
	else
	{
		unsigned short data1;

		s_shift	= rpb->sx1 & 0xf;
		d_shift	= rpb->dx1 & 0xf;

		if (!s_shift && !d_shift)
		{
		/* source & destination starts on whole word */
			if (rpb->s_is_scrn && rpb->d_is_scrn)
			{
				et6k_S_ONLY_1b(rpb);
				return;
			}

			endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));
			groups = width >> 4;

			for (; height > 0; height--)
			{
				s = src;
				d = dst;

				for (i = groups >> 3; i > 0; i--)
				{
					*(long *)((long *)d)++ = *(long *)((long *)s)++;
					*(long *)((long *)d)++ = *(long *)((long *)s)++;
					*(long *)((long *)d)++ = *(long *)((long *)s)++;
					*(long *)((long *)d)++ = *(long *)((long *)s)++;
				}
				for (i = groups & 7; i > 0; i--)
					*d++ = *s++;
				if (endmask != 0xffff)
					*d++ = (*s++ & endmask) | (*d & ~endmask);
#if 0

				for (i = width >> 4; i > 0; i--)
					*d++ = *s++;
				if (endmask != 0xffff)
					*d++ = (*s++ & endmask) | (*d & ~endmask);
#endif
				src += sbpl;
				dst += dbpl;
			}
		}
		else if (!(shift = d_shift - s_shift))
		{
		/* Same shift for both source and dest */
			begmask	= 0xffff >> s_shift;
			endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));
			hbits	= (16 - s_shift) & 0xf;
			width	-= hbits;

			if (width <= 0)
			{
				hbits = width + hbits;
				groups = ebits = 0;
				if (width)
					begmask &= endmask;
			}
			else if (width > 15)
			{
				ebits = (rpb->sx2 + 1) & 0xf;
				groups = (width - ebits) >> 4;
			}
			else
			{
				groups = 0;
				ebits = width;
			}
			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				if (hbits)
					*d++ = (*s++ & begmask) | (*d & ~begmask);
				for (i = groups; i > 0; i--)
					*d++ = *s++;
				if (ebits)
					*d++ = (*s++ & endmask) | (*d & ~endmask);

				src += sbpl;
				dst += dbpl;
			}			
		}
		else if (shift > 0)
		{
		/* shift source right */
			begmask = 0xffff >> d_shift;
			endmask = ~(0xffff >> ((rpb->dx2 + 1) & 0xf));
			hbits	= (16 - d_shift) & 0xf;
			width	-= hbits;

			if (width <= 0)
			{
				hbits = width + hbits;
				groups = ebits = 0;
				if (width)
					begmask &= endmask;
			}
			else if (width > 15)
			{
				ebits = (rpb->dx2 + 1) & 0xf;
				groups = (width - ebits) >> 4;
			}
			else
			{
				groups = 0;
				ebits = width;
			}

			spans = ebits - shift;

			if (spans < 0)
				spans = 0;

			for (; height > 0; height--)
			{
				s = src;
				d = dst;

				if (hbits)
				{
					data = *s++;
					*d++ = ((data >> shift) & begmask) | (*d & ~begmask);
				}
				else
					data = 0;

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data <<= 16 - shift;
						data1 = *s++;
						data |= data1 >> shift;
						*d++ = data;
						data = data1;
					}
				}

				if (ebits)
				{
					data <<= 16 - shift;
					if (spans)
						data |= *s++ >> shift;
					*d++ = (data & endmask) | (*d & ~endmask);
				}
				src += sbpl;
				dst += dbpl;
			}		
		}
		else
		{
			/* shift source left */
			shift	= -shift;
			begmask = 0xffff >> d_shift;
			endmask = ~(0xffff >> ((rpb->dx2 + 1) & 0xf));
			hbits	= (16 - d_shift) & 0xf;
			width	-= hbits;

			if (width <= 0)
			{
				hbits = width + hbits;
				groups = ebits = 0;
				if (width)
					begmask &= endmask;
			}
			else if (width > 15)
			{
				ebits = (rpb->dx2 + 1) & 0xf;
				groups = (width - ebits) >> 4;
			}
			else
			{
				groups = 0;
				ebits = width;
			}

			spans = s_shift + width;

			for (; height > 0; height--)
			{
				s = src;
				d = dst;

				if (hbits)
				{
					data = *s++ << shift;

					if ((s_shift + hbits) > 16) //spans > 16)
						data |= *s >> (16 - shift); //spans;

					*d++ = (data & begmask) | (*d & ~begmask);
				}
				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data = *s++ << shift;
						data |= *s >> (16 - shift);
						*d++ = data;
					}
				}
				if (ebits)
				{
					data = *s++ << shift;

					if ((s_shift + ebits) > 16) //(spans & 0xf) < ebits)
						data |= *s >> (16 - shift);

					*d++ = (data & endmask) | (*d & ~endmask);

					//*d++ = ((*s++ << shift) & endmask) | (*d & ~endmask);
				}

				src += sbpl;
				dst += dbpl;
			}		
		}
	}
}

