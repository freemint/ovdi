#include "mouse.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "display.h"

#include "4b_generic.h"

/* ******************************************************** */
/* *************** DIFFERENT WRMODES **************** */
/* ******************************************************** */
static void _cdecl ALL_WHITE	(unsigned char *addr, long data);
static void _cdecl S_AND_D	(unsigned char *addr, long data);
static void _cdecl S_AND_NOTD	(unsigned char *addr, long data);
static void _cdecl S_ONLY	(unsigned char *addr, long data);
static void _cdecl NOTS_AND_D	(unsigned char *addr, long data);
static void _cdecl D_ONLY	(unsigned char *addr, long data);
static void _cdecl S_XOR_D	(unsigned char *addr, long data);
static void _cdecl S_OR_D	(unsigned char *addr, long data);
static void _cdecl NOT_SORD	(unsigned char *addr, long data);
static void _cdecl NOT_SXORD	(unsigned char *addr, long data);
static void _cdecl NOT_D	(unsigned char *addr, long data);
static void _cdecl S_OR_NOTD	(unsigned char *addr, long data);
static void _cdecl NOT_S	(unsigned char *addr, long data);
static void _cdecl NOTS_OR_D	(unsigned char *addr, long data);
static void _cdecl NOT_SANDD	(unsigned char *addr, long data);
static void _cdecl ALL_BLACK	(unsigned char *addr, long data);

static unsigned short pm[] =
{ 0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100,
  0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001 };

unsigned long _cdecl
get_pixel_4b(unsigned char *_sb, short bpl, short x, short y)
{
	unsigned short *sb;
	unsigned short pixel = 0;
	int mask, shift;

	shift	= x & 0xf;
	mask	= pm[shift];
	shift	= 15 - shift;
	sb	= (unsigned short *)(_sb + (long)(8 + ((x >> 4) << 3) + (long)y * bpl));


	pixel	|= (*--sb >> shift) & 1;
	pixel	<<= 1;
	pixel	|= (*--sb >> shift) & 1;
	pixel	<<= 1;
	pixel	|= (*--sb >> shift) & 1;
	pixel	<<= 1;
	pixel	|= (*--sb >> shift) & 1;

	return (unsigned long)pixel;
}

void _cdecl
put_pixel_4b(unsigned char *_sb, short bpl, short x, short y, unsigned long pixel)
{
	unsigned short *sb;
	unsigned short mask, shift;

	shift	= x & 0xf;
	mask	= pm[shift];
	shift	= 15 - shift;
	sb	= (unsigned short *)(_sb + (long)(((x >> 4) << 3) + (long)y * bpl));

	*sb++ = (*sb & ~mask) | ((pixel << shift) & mask);
	pixel >>= 1;
	*sb++ = (*sb & ~mask) | ((pixel << shift) & mask);
	pixel >>= 1;
	*sb++ = (*sb & ~mask) | ((pixel << shift) & mask);
	pixel >>= 1;
	*sb   = (*sb & ~mask) | ((pixel << shift) & mask);

	return;
}

pixel_blit dpf_4b[] = 
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

pixel_blit rt_ops_4b[] = 
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

static void _cdecl
ALL_WHITE(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	unsigned short mask = ~(pm[data >> 16]);

	*addr++	&= mask;
	*addr++	&= mask;
	*addr++	&= mask;
	*addr	&= mask;
}

static void _cdecl
S_AND_D(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | ((*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr & ((data & 1) << shift)) & mask);
}
static void _cdecl
S_AND_NOTD(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | ((~*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((~*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((~*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((~*addr & ((data & 1) << shift)) & mask);
}
static void _cdecl
S_ONLY(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | ( ((data & 1) << shift) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ( ((data & 1) << shift) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ( ((data & 1) << shift) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ( ((data & 1) << shift) & mask);
}
static void _cdecl
NOTS_AND_D(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = ~(unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | ((*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr & ((data & 1) << shift)) & mask);
}
static void _cdecl
D_ONLY(unsigned char *addr, long data)
{
	return;
}
static void _cdecl
S_XOR_D(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | ((*addr ^ ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr ^ ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr ^ ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr ^ ((data & 1) << shift)) & mask);
}
static void _cdecl
S_OR_D(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | ((*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr | ((data & 1) << shift)) & mask);
}
static void _cdecl
NOT_SORD(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | (~(*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~(*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~(*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~(*addr | ((data & 1) << shift)) & mask);
}
static void _cdecl
NOT_SXORD(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | (~(*addr ^ ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~(*addr ^ ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~(*addr ^ ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~(*addr ^ ((data & 1) << shift)) & mask);
}
static void _cdecl
NOT_D(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | (~*addr & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~*addr & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~*addr & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~*addr & mask);
}
static void _cdecl
S_OR_NOTD(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | ((~*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((~*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((~*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((~*addr | ((data & 1) << shift)) & mask);
}
static void _cdecl
NOT_S(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = ~(unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | ( ((data & 1) << shift) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ( ((data & 1) << shift) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ( ((data & 1) << shift) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ( ((data & 1) << shift) & mask);
}
static void _cdecl
NOTS_OR_D(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = ~(unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | ((*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr | ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | ((*addr | ((data & 1) << shift)) & mask);
}
static void _cdecl
NOT_SANDD(unsigned char *_addr, long _data)
{
	unsigned short *addr = (unsigned short *)_addr;
	short shift = _data >> 16;
	unsigned short data = (unsigned short)_data;
	unsigned short mask = pm[shift];

	shift = 15 - shift;

	*addr++ = (*addr & ~mask) | (~(*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~(*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~(*addr & ((data & 1) << shift)) & mask);
	data >>= 1;
	*addr++ = (*addr & ~mask) | (~(*addr & ((data & 1) << shift)) & mask);
}
static void _cdecl
ALL_BLACK(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	unsigned short mask = pm[data >> 16];

	*addr++	|= mask;
	*addr++	|= mask;
	*addr++	|= mask;
	*addr	|= mask;
}

/* *************** RASTER OPERATIONS **************** */
extern void _cdecl rb_ALL_WHITE_4b	(ROP_PB *);
extern void _cdecl rb_S_AND_D_4b	(ROP_PB *);
extern void _cdecl rb_S_AND_NOTD_4b	(ROP_PB *);
extern void _cdecl rb_S_ONLY_4b	(ROP_PB *);
extern void _cdecl rb_NOTS_AND_D_4b	(ROP_PB *);
//static void rb_D_ONLY		(ROP_PB *);
extern void _cdecl rb_S_XOR_D_4b	(ROP_PB *);
extern void _cdecl rb_S_OR_D_4b	(ROP_PB *);
extern void _cdecl rb_NOT_SORD_4b	(ROP_PB *);
extern void _cdecl rb_NOT_SXORD_4b	(ROP_PB *);
extern void _cdecl rb_NOT_D_4b		(ROP_PB *);
extern void _cdecl rb_S_OR_NOTD_4b	(ROP_PB *);
extern void _cdecl rb_NOT_S_4b		(ROP_PB *);
extern void _cdecl rb_NOTS_OR_D_4b	(ROP_PB *);
extern void _cdecl rb_NOT_SANDD_4b	(ROP_PB *);
extern void _cdecl rb_ALL_BLACK_4b	(ROP_PB *);

raster_blit rops_4b[] =
{
	rb_ALL_WHITE_4b,	/* rb_ALL_WHITE,*/
	rb_S_AND_D_4b,		/* rb_S_AND_D,*/
	rb_S_AND_NOTD_4b,	/* rb_S_AND_NOTD,*/
	rb_S_ONLY_4b,
	rb_NOTS_AND_D_4b,	/* rb_NOTS_AND_D,*/
	0,			/* rb_D_ONLY, Nothing to do, leave it NULL */ 
	rb_S_XOR_D_4b,
	rb_S_OR_D_4b,
	rb_NOT_SORD_4b,		/* rb_NOT_SORD,*/
	rb_NOT_SXORD_4b,	/* rb_NOT_SXORD,*/
	rb_NOT_D_4b,		/* rb_NOT_D,*/
	rb_S_OR_NOTD_4b,	/* rb_S_OR_NOTD,*/
	rb_NOT_S_4b,
	rb_NOTS_OR_D_4b,	/* rb_NOTS_OR_D,*/
	rb_NOT_SANDD_4b,	/* rb_NOT_SANDD,*/
	rb_ALL_BLACK_4b,	/* rb_ALL_BLACK*/
};

void _cdecl
draw_mousecurs_4b(register XMFORM *mf, register short x, register short y)
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
	dbpl	= bypl - (w << 2);

	{
		register unsigned short *src, *dst;
		register int i, j;


		dst		= (unsigned short *)ms->save;
		src		= (unsigned short *)mf->scr_base + (long)(((long)y * bypl) + ((x >> 4) << 2));

		ms->width	= w;		// Number of groups
		ms->height	= height;
		ms->bypl	= bypl;
		ms->valid	= 1;
		ms->src		= (unsigned char *)src;

		for (i = height; i > 0; i--)
		{

			for (j = w; j > 0; j--)
			{
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
			}

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


		d = (unsigned short *)mf->scr_base + (long)(((long)y * bypl) + ((x >> 4) << 2));
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
				*d++ = (((data & mask) >> shift) & begmask) | (*d & ~(mask >> shift));
				*d++ = (((data & mask) >> shift) & begmask) | (*d & ~(mask >> shift));
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
					*d++ = (*d & ~mask) | data;
					*d++ = (*d & ~mask) | data;
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
				*d++ = (*d & ~mask) | (data & mask);
				*d++ = (*d & ~mask) | (data & mask);
				*d++ = (*d & ~mask) | (data & mask);
				//*d++ = (data & endmask) | (*d & ~endmask);
			}
			s += sbpl;
			d += dbpl;
		}		
	}
	return;
}

void _cdecl
restore_msave_4b(XMSAVE *ms)
{
	register int width, w, height, nl;
	register unsigned short *src, *dst;

	if (ms->valid)
	{
		src	= (unsigned short *)ms->save;
		dst	= (unsigned short *)ms->src;
		width	= ms->width;
		nl	= ms->bypl - (width<<2);

		for (height = ms->height; height > 0; height--)
		{
			for (w = width; w > 0; w--)
			{
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
			}
			dst += nl;
		}
		ms->valid = 0;
	}
	return;
}
