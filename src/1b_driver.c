
#include "display.h"
#include "linea.h"
#include "mouse.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "1b_driver.h"

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
get_pixel_1b(unsigned char *sb, short bpl, short x, short y)
{

	return ((*(unsigned short *)(sb + (long)(((x >> 4) <<1) + ((long)y * bpl)))) & (x & 0xf));
}

void
put_pixel_1b(unsigned char *sb, short bpl, short x, short y, unsigned long pixel)
{
	unsigned short mask, shift;

	sb	+= (x >> 4) << 1;
	sb	+= (long)y * bpl;

	shift	= x & 0xf;
	mask	= 0x8000 >> shift;

	(unsigned short)pixel = ((unsigned short)pixel >> (shift + 1) | ((unsigned short)pixel << (15-(shift))) );

	*(unsigned short *)sb &= mask;
	*(unsigned short *)sb |= (unsigned short)pixel;
	
	return;
}

pixel_blit dpf_1b[] = 
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

pixel_blit rt_ops_1b[] = 
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
	short shift = data >> 16;
	unsigned short mask = ~(0x8000 >> shift);

	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	*(unsigned short *)addr &= mask;
	return;
}
static void
S_AND_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	

	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = (p & data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(unsigned short *)addr = p;

	return;
}
static void
S_AND_NOTD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = (data & ~p) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(unsigned short *)addr = p;

	return;
}
static void
S_ONLY(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short p;
	unsigned short mask = 0x8000 >> shift;
	

	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	p	&= ~mask;
	p	|= data;
	*(unsigned short *)addr = p;

	return;
}
static void
NOTS_AND_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = (~data & p) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(unsigned short *)addr = p;

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
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = (p ^ data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(unsigned short *)addr = p;

	return;
}
static void
S_OR_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	*(unsigned short *)addr |= data & mask;

	return;
}
static void
NOT_SORD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = ~(p | data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(unsigned short *)addr = p;

	return;
}
static void
NOT_SXORD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = ~(p ^ data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(unsigned short *)addr = p;

	return;
}
static void
NOT_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = ~p & mask;
	p	&= ~mask;
	p	|= pixel;
	*(unsigned short *)addr = p;

	return;
}
static void
S_OR_NOTD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = (data | ~p) & mask;
	p	&= (~mask);
	p	|= pixel;
	*(unsigned short *)addr = p;

	return;
}
static void
NOT_S(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	p	&= ~mask;
	p	|= ~data & mask;
	*(unsigned short *)addr = p;

	return;
}
static void
NOTS_OR_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = (~data | p) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(unsigned short *)addr = p;

	return;
}
static void
NOT_SANDD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short pixel, p;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	p	 = *(unsigned short *)addr;
	pixel	 = ~(p & data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(unsigned short *)addr = p;

	*addr = ~((unsigned char)data & *addr);
	return;
}
static void
ALL_BLACK(unsigned char *addr, long data)
{
	short shift = data >> 16;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	*(unsigned short *)addr |= mask;

	return;
}

/* *************** RASTER OPERATIONS **************** */
//static void rb_ALL_WHITE	(ROP_PB *);
//static void rb_S_AND_D		(ROP_PB *);
//static void rb_S_AND_NOTD	(ROP_PB *);
static void rb_S_ONLY		(ROP_PB *);
//static void rb_NOTS_AND_D	(ROP_PB *);
//static void rb_D_ONLY		(ROP_PB *);
static void rb_S_XOR_D		(ROP_PB *);
static void rb_S_OR_D		(ROP_PB *);
//static void rb_NOT_SORD		(ROP_PB *);
//static void rb_NOT_SXORD	(ROP_PB *);
//static void rb_NOT_D		(ROP_PB *);
//static void rb_S_OR_NOTD	(ROP_PB *);
//static void rb_NOT_S		(ROP_PB *);
//static void rb_NOTS_OR_D	(ROP_PB *);
//static void rb_NOT_SANDD	(ROP_PB *);
//static void rb_ALL_BLACK	(ROP_PB *);

raster_blit rops_1b[] =
{
	0,	/* rb_ALL_WHITE,*/
	0,	/* rb_S_AND_D,*/
	0,	/* rb_S_AND_NOTD,*/
	rb_S_ONLY,
	0,	/* rb_NOTS_AND_D,*/
	0,	/* rb_D_ONLY,*/
	rb_S_XOR_D,
	rb_S_OR_D,
	0,	/* rb_NOT_SORD,*/
	0,	/* rb_NOT_SXORD,*/
	0,	/* rb_NOT_D,*/
	0,	/* rb_S_OR_NOTD,*/
	0,	/* rb_NOT_S,*/
	0,	/* rb_NOTS_OR_D,*/
	0,	/* rb_NOT_SANDD,*/
	0,	/* rb_ALL_BLACK*/
};

static void
rb_S_ONLY(ROP_PB *rpb)
{
	short i, width, height, s_shift, d_shift, shift, hbits, ebits, groups, spans, sbpl, dbpl;
	unsigned short begmask, endmask, data;
	register unsigned short *src, *dst, *s, *d;
	
	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sbpl	= rpb->s_bypl;
	dbpl	= rpb->d_bypl;

	(long)src = (long)rpb->s_addr + (long)(rpb->sx1 >> 4) + (long)(rpb->sy1 * sbpl);
	(long)dst = (long)rpb->d_addr + (long)(rpb->dx1 >> 4) + (long)(rpb->dy1 * dbpl);

	s_shift	= rpb->sx1 & 0xf;
	d_shift	= rpb->dx1 & 0xf;

	sbpl >>= 1; 
	dbpl >>= 1;

	if (!s_shift && !d_shift)
	{
	/* source & destination starts on whole word */

		endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));

		for (; height > 0; height--)
		{
			s = src;
			d = dst;
			for (i = width >> 4; i > 0; i--)
				*d++ = *s++;
			if (endmask != 0xffff)
				*d++ = (*s++ & endmask) | (*d & ~endmask);
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

		spans = (s_shift + width) - 16;

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
					data |= (*s >> shift);
					*d++ = data;
					data = *s++;
				}
			}

			if (ebits)
			{
				data <<= 16 - shift;
				if (spans)
					data |= *s++ >> (ebits - spans);
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
		begmask = 0xffff >> s_shift;
		endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));
		hbits	= (16 - s_shift) & 0xf;
		width	-= hbits;

		if (width <= 0)
		{
			hbits = width + hbits;
			groups = ebits = 0;
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

		spans = (d_shift + width) - 16;

		if (spans < 0)
			spans = 0;

		for (; height > 0; height--)
		{
			s = src;
			d = dst;

			if (hbits)
			{
				data = (*s++ << shift) & begmask;
			}
			else
				data = 0;
			if (groups)
			{
				data |= *s >> (d_shift + hbits);
				*d++ = data | (*d & ~begmask);

				for (i = groups; i > 0; i--)
				{
					*d++ = (*s++ << shift) | (*s >> (16 - shift));
					data = *s++ << shift;
				}
			}

			if (ebits)
			{
				
				data |= *s >> (d_shift + hbits);
				if (spans)
				{
					*d++ = data;
					data = *s++;
				}
				else
					s++;
				*d++ = (data & endmask) | (*d & ~endmask);
			}
			else
				*d++ = data | (*d & ~begmask);

			src += sbpl;
			dst += dbpl;
		}		
	}
}

static void
rb_S_XOR_D(ROP_PB *rpb)
{
	short i, width, height, s_shift, d_shift, shift, hbits, ebits, groups, spans, sbpl, dbpl;
	unsigned short begmask, endmask, data;
	register unsigned short *src, *dst, *s, *d;
	
	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sbpl	= rpb->s_bypl;
	dbpl	= rpb->d_bypl;

	(long)src = (long)rpb->s_addr + (long)(rpb->sx1 >> 4) + (long)(rpb->sy1 * sbpl);
	(long)dst = (long)rpb->d_addr + (long)(rpb->dx1 >> 4) + (long)(rpb->dy1 * dbpl);

	s_shift	= rpb->sx1 & 0xf;
	d_shift	= rpb->dx1 & 0xf;

	sbpl >>= 1; 
	dbpl >>= 1;

	if (!s_shift && !d_shift)
	{
	/* source & destination starts on whole word */

		endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));

		for (; height > 0; height--)
		{
			s = src;
			d = dst;
			for (i = width >> 4; i > 0; i--)
				*d++ ^= *s++;
			if (endmask != 0xffff)
				*d++ = ((*s++ ^ *d) & endmask) | (*d & ~endmask);

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
				*d++ = ((*s++ ^ *d) & begmask) | (*d & ~begmask);
			for (i = groups; i > 0; i--)
				*d++ ^= *s++;
			if (ebits)
				*d++ = ((*s++ ^ *d) & endmask) | (*d & ~endmask);

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

		spans = (s_shift + width) - 16;

		if (spans < 0)
			spans = 0;

		for (; height > 0; height--)
		{
			s = src;
			d = dst;
			if (hbits)
			{
				data = *s++;
				*d++ = (((data >> shift) ^ *d) & begmask) | (*d & ~begmask);
			}
			else
				data = 0;
			if (groups)
			{
				for (i = groups; i > 0; i--)
				{
					data <<= 16 - shift;
					data |= (*s >> shift);
					*d++ ^= data;
					data = *s++;
				}
			}

			if (ebits)
			{
				data <<= 16 - shift;
				if (spans)
					data |= *s++ >> (ebits - spans);
				*d++ = ((data ^ *d) & endmask) | (*d & ~endmask);
			}
			src += sbpl;
			dst += dbpl;
		}		
	}
	else
	{
	/* shift source left */
		shift	= -shift;
		begmask = 0xffff >> s_shift;
		endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));
		hbits	= (16 - s_shift) & 0xf;
		width	-= hbits;

		if (width <= 0)
		{
			hbits = width + hbits;
			groups = ebits = 0;
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

		spans = (d_shift + width) - 16;

		if (spans < 0)
			spans = 0;

		for (; height > 0; height--)
		{
			s = src;
			d = dst;

			if (hbits)
			{
				data = (*s++ << shift) & begmask;
			}
			else
				data = 0;
			if (groups)
			{
				data |= *s >> (d_shift + hbits);
				*d++ ^= ((data ^ *d) & begmask) | (*d & ~begmask);

				for (i = groups; i > 0; i--)
				{
					*d++ ^= (*s++ << shift) | (*s >> (16 - shift));
					data = *s++ << shift;
				}
			}

			if (ebits)
			{
				data |= *s >> (d_shift + hbits);
				if (spans)
				{
					*d++ ^= data;
					data = *s++;
				}
				else
					s++;
				*d++ = ((data ^ *d) & endmask) | (*d & ~endmask);
			}
			else
				*d++ = data;

			src += sbpl;
			dst += dbpl;
		}		
	}
}

static void
rb_S_OR_D(ROP_PB *rpb)
{
	short i, width, height, s_shift, d_shift, shift, hbits, ebits, groups, spans, sbpl, dbpl;
	unsigned short begmask, endmask, data;
	register unsigned short *src, *dst, *s, *d;
	
	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sbpl	= rpb->s_bypl;
	dbpl	= rpb->d_bypl;

	(long)src = (long)rpb->s_addr + (long)(rpb->sx1 >> 4) + (long)(rpb->sy1 * sbpl);
	(long)dst = (long)rpb->d_addr + (long)(rpb->dx1 >> 4) + (long)(rpb->dy1 * dbpl);

	s_shift	= rpb->sx1 & 0xf;
	d_shift	= rpb->dx1 & 0xf;

	sbpl >>= 1; 
	dbpl >>= 1;

	if (!s_shift && !d_shift)
	{
	/* source & destination starts on whole word */

		endmask = 0xffff >> ((rpb->sx2 + 1) & 0xf);

		for (; height > 0; height--)
		{
			s = src;
			d = dst;
			for (i = width >> 4; i > 0; i--)
				*d++ |= *s++;
			if (endmask != 0xffff)
				*d++ |= (*s++ & ~endmask);
			src += sbpl;
			dst += dbpl;
		}
	}
	else if (!(shift = d_shift - s_shift))
	{
	/* Same shift for both source and dest */
		begmask	= 0xffff >> s_shift;
		endmask = 0xffff >> ((rpb->sx2 + 1) & 0xf);
		hbits	= (16 - s_shift) & 0xf;
		width	-= hbits;

		if (width <= 0)
		{
			hbits = width + hbits;
			groups = ebits = 0;
			begmask = ~endmask;
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
				*d++ |= (*s++ & ~begmask);
			for (i = groups; i > 0; i--)
				*d++ |= *s++;
			if (ebits)
				*d++ |= (*s++ & ~endmask);

			src += sbpl;
			dst += dbpl;
		}			
	}
	else if (shift > 0)
	{
	/* shift source right */
		begmask = 0xffff >> d_shift;
		endmask = 0xffff >> ((rpb->dx2 + 1) & 0xf);
		hbits	= (16 - d_shift) & 0xf;
		width	-= hbits;

		if (width <= 0)
		{
			hbits = width + hbits;
			groups = ebits = 0;
			begmask &= ~endmask;
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

		spans = (s_shift + width) - 16;

		if (spans < 0)
			spans = 0;

		for (; height > 0; height--)
		{
			s = src;
			d = dst;
			if (hbits)
			{
				data = *s++;
				*d++ |= ((data >> shift) & ~begmask);
			}
			else
				data = 0;
			if (groups)
			{
				for (i = groups; i > 0; i--)
				{
					data <<= 16 - shift;
					data |= (*s >> shift);
					*d++ |= data;
					data = *s++;
				}
			}

			if (ebits)
			{
				data <<= 16 - shift;
				if (spans)
					data |= *s++ >> (ebits - spans);
				*d++ |= (data & ~endmask);
			}
			src += sbpl;
			dst += dbpl;
		}		
	}
	else
	{
	/* shift source left */
		shift	= -shift;
		begmask = 0xffff >> s_shift;
		endmask = 0xffff >> ((rpb->sx2 + 1) & 0xf);
		hbits	= (16 - s_shift) & 0xf;
		width	-= hbits;

		if (width <= 0)
		{
			hbits = width + hbits;
			groups = ebits = 0;
			begmask &= ~endmask;
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

		spans = (d_shift + width) - 16;

		if (spans < 0)
			spans = 0;

		for (; height > 0; height--)
		{
			s = src;
			d = dst;

			if (hbits)
			{
				data = (*s++ << shift) & begmask;
			}
			else
				data = 0;
			if (groups)
			{
				data |= *s >> (d_shift + hbits);
				*d++ |= data;

				for (i = groups; i > 0; i--)
				{
					*d++ |= (*s++ << shift) | (*s >> (16 - shift));
					data = *s++ << shift;
				}
			}

			if (ebits)
			{
				
				data |= *s >> (d_shift + hbits);
				if (spans)
				{
					*d++ |= data;
					data = *s++;
				}
				else
					s++;
				*d++ |= (data & ~endmask);
			}
			else
				*d++ |= data;

			src += sbpl;
			dst += dbpl;
		}		
	}
}
