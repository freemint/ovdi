
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

O_u32
get_pixel_1b(unsigned char *sb, O_Int bpl, O_Pos x, O_Pos y)
{

	return ((*(O_u16 *)(sb + (long)(((x >> 4) <<1) + ((long)y * bpl)))) & (x & 0xf));
}

void
put_pixel_1b(unsigned char *sb, O_Int bpl, O_Pos x, O_Pos y, O_u32 pixel)
{
	O_u16 mask, shift;

	sb	+= (x >> 4) << 1;
	sb	+= (long)y * bpl;

	shift	= x & 0xf;
	mask	= 0x8000 >> shift;

	(O_u16)pixel = ((O_u16)pixel >> (shift + 1) | ((O_u16)pixel << (15-(shift))) );

	*(O_u16 *)sb &= mask;
	*(O_u16 *)sb |= (O_u16)pixel;
	
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
	O_u16 mask = ~(0x8000 >> shift);

	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	*(O_u16 *)addr &= mask;
	return;
}
static void
S_AND_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	

	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = (p & data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(O_u16 *)addr = p;

	return;
}
static void
S_AND_NOTD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = (data & ~p) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(O_u16 *)addr = p;

	return;
}
static void
S_ONLY(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 p;
	O_u16 mask = 0x8000 >> shift;
	

	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	p	&= ~mask;
	p	|= data;
	*(O_u16 *)addr = p;

	return;
}
static void
NOTS_AND_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = (~data & p) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(O_u16 *)addr = p;

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
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = (p ^ data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(O_u16 *)addr = p;

	return;
}
static void
S_OR_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	*(O_u16 *)addr |= data & mask;

	return;
}
static void
NOT_SORD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = ~(p | data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(O_u16 *)addr = p;

	return;
}
static void
NOT_SXORD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = ~(p ^ data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(O_u16 *)addr = p;

	return;
}
static void
NOT_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = ~p & mask;
	p	&= ~mask;
	p	|= pixel;
	*(O_u16 *)addr = p;

	return;
}
static void
S_OR_NOTD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = (data | ~p) & mask;
	p	&= (~mask);
	p	|= pixel;
	*(O_u16 *)addr = p;

	return;
}
static void
NOT_S(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	p	&= ~mask;
	p	|= ~data & mask;
	*(O_u16 *)addr = p;

	return;
}
static void
NOTS_OR_D(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = (~data | p) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(O_u16 *)addr = p;

	return;
}
static void
NOT_SANDD(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 pixel, p;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	p	 = *(O_u16 *)addr;
	pixel	 = ~(p & data) & mask;
	p	&= ~mask;
	p	|= pixel;
	*(O_u16 *)addr = p;

	*addr = ~((unsigned char)data & *addr);
	return;
}
static void
ALL_BLACK(unsigned char *addr, long data)
{
	short shift = data >> 16;
	O_u16 mask = 0x8000 >> shift;
	
	(O_u16)data &= 1;
	(O_u16)data = (O_u16)data >> (shift + 1) | (O_u16)data << (15-(shift));

	*(O_u16 *)addr |= mask;

	return;
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
		register O_u16 *src, *dst;
		register int i, j;


		dst		= (O_u16 *)ms->save;
		src		= (O_u16 *)mf->scr_base + (long)(((long)y * bypl) + (x >> 4));

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
		register O_u32 fgc, bgc;
		register O_u16 data, shift, begmask, endmask, mask;
		register O_u16 *s;
		register O_u16 *d;


		d = (O_u16 *)mf->scr_base + (long)(((long)y * bypl) + (x >> 4));
		s = (O_u16 *)mf->data + (long)(yoff * mf->mfbypl) + (long)((xoff >> 4) << 1);

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
				register O_u16 md, mm;

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
	register O_u16 *src, *dst;

	if (ms->valid)
	{
		src	= (O_u16 *)ms->save;
		dst	= (O_u16 *)ms->src;
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
