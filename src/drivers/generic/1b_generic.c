
#include "mouse.h"
#include "mousedrv.h"
#include "ovdi_defs.h"

#include "1b_generic.h"

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
get_pixel_1b(unsigned char *_sb, short bpl, short x, short y)
{
	unsigned short *sb;

	sb = (unsigned short *)((long)_sb + (long)((x >> 4) << 1) + (y * bpl));
	return (*sb & (x & 0xf));
#if 0
	return ((*(unsigned short *)(sb + (long)(((x >> 4) <<1) + ((long)y * bpl)))) & (x & 0xf));
#endif
}

void
put_pixel_1b(unsigned char *sb, short bpl, short x, short y, unsigned long _pixel)
{
	union { void *sb; unsigned short *ptr; long adr;} s;
	unsigned short mask, shift, pixel = (unsigned short)_pixel;

	s.sb = sb;

	s.adr	+= (long)((x >> 4) << 1) + (long)y * bpl;
	shift	 = x & 0xf;
	mask	 = 0x8000 >> shift;
	pixel	 = (pixel >> (shift + 1)) | (pixel << (15 - shift));
	*s.ptr	&= mask;
	*s.ptr	|= pixel;
#if 0	 
	sb	+= (x >> 4) << 1;
	sb	+= (long)y * bpl;

	shift	= x & 0xf;
	mask	= 0x8000 >> shift;

	pixel = (pixel >> (shift + 1) | (pixel << (15-(shift))) );

	*sb &= mask;
	*sb |= pixel;

	return;
#endif
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
	union { void *v; unsigned short *s;} a;
	union { long  l; short s[2]; } d;
	short shift;
	unsigned short mask;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = ~(0x8000 >> shift);
	
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	*a.s &= mask;
#if 0
	short shift = data >> 16;
	unsigned short mask = ~(0x8000 >> shift);

	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	*(unsigned short *)addr &= mask;
	return;
#endif
}
static void
S_AND_D(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; unsigned short s[2]; } d;
	short shift;
	unsigned short pixel, p, mask;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));

	p = *a.s;
	pixel = (p & d.s[1]) & mask;
	p &= ~mask;
	p |= pixel;
	*a.s = p;
#if 0	
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
#endif
}
static void
S_AND_NOTD(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short pixel, p, mask;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	p = *a.s;
	pixel = (d.s[1] & ~p) & mask;
	p &= ~mask;
	p |= pixel;
	*a.s = p;
#if 0	
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
#endif
}
static void
S_ONLY(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short p, mask;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	p = *a.s;
	p &= ~mask;
	p |= d.s[1];
	*a.s = p;
#if 0
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
#endif
}
static void
NOTS_AND_D(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short pixel, p, mask;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	p = *a.s;
	pixel = (~d.s[1] & p) & mask;
	p &= ~mask;
	p |= pixel;
	*a.s = p;
#if 0	
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
#endif
}
static void
D_ONLY(unsigned char *addr, long data)
{
	return;
}
static void
S_XOR_D(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short pixel, p, mask;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	p = *a.s;
	pixel = (p ^ data) & mask;
	p &= ~mask;
	p |= pixel;
	*a.s = p;
#if 0	
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
#endif
}
static void
S_OR_D(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short mask;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	*a.s |= (d.s[1] & mask);
#if 0
	short shift = data >> 16;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	*(unsigned short *)addr |= data & mask;

	return;
#endif
}
static void
NOT_SORD(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short mask, pixel, p;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	p = *a.s;
	pixel = ~(p & d.s[1]) & mask;
	p &= ~mask;
	p |= pixel;
	*a.s = p;
#if 0
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
#endif
}
static void
NOT_SXORD(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short mask, pixel, p;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	p = *a.s;
	pixel = ~(p ^ d.s[1]) & mask;
	p &= ~mask;
	p |= pixel;
	*a.s = p;
#if 0	
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
#endif
}
static void
NOT_D(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short mask, pixel, p;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	p = *a.s;
	pixel = ~p & mask;
	p &= ~mask;
	p |= pixel;
	*a.s = p;
#if 0
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
#endif
}
static void
S_OR_NOTD(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short mask, pixel, p;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	p = *a.s;
	pixel = (d.s[1] | ~p) & mask;
	p &= (~mask);
	p |= pixel;
	*a.s = p;
#if 0
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
#endif
}
static void
NOT_S(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short mask, p;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));

	p = *a.s;
	p &= ~mask;
	p |= ~d.s[1] & mask;
	*a.s = p;
#if 0
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
#endif
}
static void
NOTS_OR_D(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short mask, pixel, p;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));
	
	p = *a.s;
	pixel = (~d.s[1] | p) & mask;
	p &= ~mask;
	p |= pixel;
	*a.s = p;
#if 0
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
#endif
}
static void
NOT_SANDD(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short mask, pixel, p;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));

	p = *a.s;
	pixel = ~(p & d.s[1]) & mask;
	p &= ~mask;
	p |= pixel;
	*a.s = p;

#if 0
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
#endif
}
static void
ALL_BLACK(unsigned char *addr, long data)
{
	union { void *v; unsigned short *s; } a;
	union { long l; short s[2]; } d;
	short shift;
	unsigned short mask, p;

	a.v = addr;
	d.l = data;

	shift = d.s[0];
	mask = 0x8000 >> shift;
	d.s[1] = 1;
	d.s[1] = (d.s[1] >> (shift + 1)) | (d.s[1] << (15 - shift));

	p = *a.s;
	p |= d.s[1];
	*a.s = p;
#if 0
	short shift = data >> 16;
	unsigned short mask = 0x8000 >> shift;
	
	(unsigned short)data &= 1;
	(unsigned short)data = (unsigned short)data >> (shift + 1) | (unsigned short)data << (15-(shift));

	*(unsigned short *)addr |= mask;

	return;
#endif
}

/* *************** RASTER OPERATIONS **************** */
extern void rb_ALL_WHITE_1b	(ROP_PB *);
extern void rb_S_AND_D_1b	(ROP_PB *);
extern void rb_S_AND_NOTD_1b	(ROP_PB *);
extern void rb_S_ONLY_1b	(ROP_PB *);
extern void rb_NOTS_AND_D_1b	(ROP_PB *);
//static void rb_D_ONLY		(ROP_PB *);
extern void rb_S_XOR_D_1b	(ROP_PB *);
extern void rb_S_OR_D_1b	(ROP_PB *);
extern void rb_NOT_SORD_1b	(ROP_PB *);
extern void rb_NOT_SXORD_1b	(ROP_PB *);
extern void rb_NOT_D_1b		(ROP_PB *);
extern void rb_S_OR_NOTD_1b	(ROP_PB *);
extern void rb_NOT_S_1b		(ROP_PB *);
extern void rb_NOTS_OR_D_1b	(ROP_PB *);
extern void rb_NOT_SANDD_1b	(ROP_PB *);
extern void rb_ALL_BLACK_1b	(ROP_PB *);

raster_blit rops_1b[] =
{
	rb_ALL_WHITE_1b,	/* rb_ALL_WHITE,*/
	rb_S_AND_D_1b,		/* rb_S_AND_D,*/
	rb_S_AND_NOTD_1b,	/* rb_S_AND_NOTD,*/
	rb_S_ONLY_1b,		 //rb_S_ONLY_new,
	rb_NOTS_AND_D_1b,	/* rb_NOTS_AND_D,*/
	0,	/* rb_D_ONLY,*/
	rb_S_XOR_D_1b,
	rb_S_OR_D_1b,
	rb_NOT_SORD_1b,		/* rb_NOT_SORD,*/
	rb_NOT_SXORD_1b,	/* rb_NOT_SXORD,*/
	rb_NOT_D_1b,		/* rb_NOT_D,*/
	rb_S_OR_NOTD_1b,	/* rb_S_OR_NOTD,*/
	rb_NOT_S_1b,
	rb_NOTS_OR_D_1b,	/* rb_NOTS_OR_D,*/
	rb_NOT_SANDD_1b,	/* rb_NOT_SANDD,*/
	rb_ALL_BLACK_1b,	/* rb_ALL_BLACK*/
};

void
draw_mousecurs_1b(register XMFORM *mf, register short x, register short y)
{
	register int width, height, xoff, yoff, bypl;
	int	dbpl, w;
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
		register int x2 = x + width - 1;
		register int mx = mf->mx;

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
		register int y2 = y + height - 1;
		register int my = mf->my;

		if (y2 > my)
		{
			if ((height -= (y2 - my)) < 0)
			{
				ms->valid = 0;
				return;
			}
		}
	}

	xoff	&= 0xf;
	bypl	>>= 1;
	w	= ((x & 0xf) + width + 15) >> 4;
	dbpl	= bypl - w;

	{
		register unsigned short *src, *dst;
		register int i, j;


		dst		= (unsigned short *)ms->save;
		src		= (unsigned short *)mf->scr_base + (long)(((long)y * bypl) + (x >> 4));

		ms->width	= w;
		ms->height	= height;
		ms->bypl	= bypl;
		ms->valid	= 1;
		ms->src		= (unsigned char *)src;

		for (i = height; i > 0; i--)
		{

			for (j = w; j > 0; j--)
				*dst++ = *src++;

			src += dbpl;
		}
	}

	if (mf->planes == 1)
	{
		register int i, hbits, ebits, groups, sbpl, spans;
		register unsigned long fgc, bgc;
		register unsigned short data, shift, begmask, endmask, mask;
		register unsigned short *s;
		register unsigned short *d;


		d = (unsigned short *)mf->scr_base + (long)(((long)y * bypl) + (x >> 4));
		s = (unsigned short *)mf->data + (long)(yoff * mf->mfbypl) + (long)((xoff >> 4) << 1);

		sbpl = ((mf->width - (xoff + width)) >> 4) << 1;
		fgc = mf->fg_pix;
		bgc = mf->bg_pix;

	/* shift source right */
		if (xoff)
		{
			shift	= 16 - xoff;
			hbits	= 0;
		}
		else
		{
			shift	= x & 0xf;
			hbits	= (16 - shift) & 0xf;
		}

		begmask = 0xffff >> shift;
		endmask = ~(0xffff >> ((x + width) & 0xf));
		width	-= hbits;

		if (width <= 0)
		{
			hbits = width + hbits;
			groups = ebits = 0;
			begmask &= endmask;
		}
		else if (width > 15)
		{
			ebits = (x + width) & 0xf;
			groups = (width - ebits) >> 4;
		}
		else
		{
			groups = 0;
			ebits = width;
		}

		spans = (ebits + hbits) - 16;

		if (spans < 0)
			spans = 0;

		for (; height > 0; height--)
		{

			//data = *s++;	/* mask */
			//data &= *s++;	/* data */
			

			if (hbits)
			{
				mask = *s++;
				data = *s++;
				*d++ = (((data & mask) >> shift) & begmask) | (*d & ~(mask >> shift));
			}
			else
			{
				mask = 0;
				data = 0;
			}
				
			if (groups)
			{
				register unsigned short md, mm;

				for (i = groups; i > 0; i--)
				{
					mask <<= 16 - shift;
					data <<= 16 - shift;
					mm = *s++;
					md = *s++;
					mask |= mm >> shift;
					data |= md >> shift;
					*d++ = (*d & ~mask) | data;
					mask = mm;
					data = md;
				}
			}

			if (ebits)
			{
				data <<= 16 - shift;
				mask <<= 16 - shift;
				if (spans)
				{
					mask |= *s++ >> shift;
					data |= *s++ >> shift;
					//data |= (*s++ & *s++) >> (ebits - spans);
				}
				*d++ = (*d & ~mask) | (data & mask);
				//*d++ = (data & endmask) | (*d & ~endmask);
			}
			s += sbpl;
			d += dbpl;
		}		
	}
	return;
}

void
restore_msave_1b(XMSAVE *ms)
{
	register int width, w, height, nl;
	register unsigned short *src, *dst;

	if (ms->valid)
	{
		src	= (unsigned short *)ms->save;
		dst	= (unsigned short *)ms->src;
		width	= ms->width;
		nl	= ms->bypl - width;

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
