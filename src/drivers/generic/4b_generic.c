/*
s 15, d 3, w 1, h 18
2, shift 12, hb 1, eb 0, g 0, sp 3, bm 8, em f
s 15, d 3, w 1, h 65
2, shift 12, hb 1, eb 0, g 0, sp 3, bm 8, em f
s 15, d 3, w 1, h 36
2, shift 12, hb 1, eb 0, g 0, sp 3, bm 8, em f

s 7, d 11, w 8, h 18
3, shift 4, hb 5, eb 3, g 0, sp 0, bm f800, em 7
s 7, d 11, w 8, h 65
3, shift 4, hb 5, eb 3, g 0, sp 0, bm f800, em 7
s 7, d 11, w 8, h 36
3, shift 4, hb 5, eb 3, g 0, sp 0, bm f800, em 7

s 15, d 3, w 8, h 18
2, shift 12, hb 8, eb 0, g 0, sp 10, bm 7f8, em 7ff
s 15, d 3, w 8, h 65
2, shift 12, hb 8, eb 0, g 0, sp 10, bm 7f8, em 7ff
s 15, d 3, w 8, h 36
2, shift 12, hb 8, eb 0, g 0, sp 10, bm 7f8, em 7ff 



s 15, d 3, w 1, h 18
2, shift 12, hb 1, eb 0, g 0, sp 3, bm 8, em f

s 15, d 3, w 8, h 18
2, shift 12, hb 8, eb 0, g 0, sp 10, bm 7f8, em 7ff

*/

#include "mouse.h"
#include "mousedrv.h"
#include "ovdi_defs.h"

#include "4b_generic.h"

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

static unsigned short pm[] =
{ 0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100,
  0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001 };

unsigned long
get_pixel_4b(unsigned char *_sb, short bpl, short x, short y)
{
	unsigned short *sb;
	unsigned short pixel = 0;
	short mask, shift;

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

void
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

static void
ALL_WHITE(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	unsigned short mask = ~(pm[data >> 16]);

	*addr++	&= mask;
	*addr++	&= mask;
	*addr++	&= mask;
	*addr	&= mask;
}

static void
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
static void
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
static void
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
static void
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
static void
D_ONLY(unsigned char *addr, long data)
{
	return;
}
static void
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
static void
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
static void
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
static void
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
static void
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
static void
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
static void
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
static void
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
static void
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
static void
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
//static void rb_ALL_WHITE	(ROP_PB *);
//static void rb_S_AND_D		(ROP_PB *);
//static void rb_S_AND_NOTD	(ROP_PB *);
//static void rb_S_ONLY		(ROP_PB *);
static void rb_S_ONLY_new	(ROP_PB *);
//static void rb_NOTS_AND_D	(ROP_PB *);
//static void rb_D_ONLY		(ROP_PB *);
static void rb_S_XOR_D		(ROP_PB *);
//static void rb_S_OR_D		(ROP_PB *);
static void rb_S_OR_D_new	(ROP_PB *);
//static void rb_NOT_SORD		(ROP_PB *);
//static void rb_NOT_SXORD	(ROP_PB *);
//static void rb_NOT_D		(ROP_PB *);
//static void rb_S_OR_NOTD	(ROP_PB *);
static void rb_NOT_S		(ROP_PB *);
//static void rb_NOTS_OR_D	(ROP_PB *);
//static void rb_NOT_SANDD	(ROP_PB *);
//static void rb_ALL_BLACK	(ROP_PB *);

raster_blit rops_4b[] =
{
	0,	/* rb_ALL_WHITE,*/
	0,	/* rb_S_AND_D,*/
	0,	/* rb_S_AND_NOTD,*/
	rb_S_ONLY_new,
	0,	/* rb_NOTS_AND_D,*/
	0,	/* rb_D_ONLY,*/
	rb_S_XOR_D,
	rb_S_OR_D_new,
	0,	/* rb_NOT_SORD,*/
	0,	/* rb_NOT_SXORD,*/
	0,	/* rb_NOT_D,*/
	0,	/* rb_S_OR_NOTD,*/
	rb_NOT_S,
	0,	/* rb_NOTS_OR_D,*/
	0,	/* rb_NOT_SANDD,*/
	0,	/* rb_ALL_BLACK*/
};

/*
s 15, d 3, w 1, h 18
2, shift 12, hb 1, eb 0, g 0, sp 3, bm 8, em f

s 15, d 3, w 8, h 18
2, shift 12, hb 8, eb 0, g 0, sp 10, bm 7f8, em 7ff

*/
static void
rb_S_ONLY_new(ROP_PB *rpb)
{
	unsigned short begmask, endmask, p0, p1, p2, p3;
	int shift, swords, dwords, sbpl, dbpl, height;
	unsigned short *s, *d;

	sbpl = (rpb->s_bypl >> 1);
	dbpl = (rpb->d_bypl >> 1);
	height = rpb->sy2 - rpb->sy1 + 1;
	s = (unsigned short *)rpb->s_addr + (long)((rpb->sx1 >> 4) << 2) + (long)((long)rpb->sy1 * sbpl);
	d = (unsigned short *)rpb->d_addr + (long)((rpb->dx1 >> 4) << 2) + (long)((long)rpb->dy1 * dbpl);
	swords	= (((rpb->sx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;
	dwords	= (((rpb->dx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;

	if (s < d)
	{
		s = (unsigned short *)rpb->s_addr + 3 + (long)((rpb->sx2 >> 4) << 2) + (long)((long)rpb->sy2 * sbpl);
		d = (unsigned short *)rpb->d_addr + 3 + (long)((rpb->dx2 >> 4) << 2) + (long)((long)rpb->dy2 * dbpl);
		sbpl -= (swords + 1) << 2;
		dbpl -= (dwords + 1) << 2;
		begmask = 0xffff << (15 - (rpb->dx2 & 0xf));
		endmask = 0xffff >> (rpb->dx1 & 0xf);
		shift	= (rpb->sx2 & 0xf) - (rpb->dx2 & 0xf);
		if (!shift)
		{
			if (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = *s--; *d-- = *s--; *d-- = *s--; *d-- = *s--;
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							for (swords = dwords + 1; swords > 0; swords--)
								*d-- = *s--,*d-- = *s--,*d-- = *s--,*d-- = *s--;
							d -= dbpl, s -= sbpl;
						}
					}
				}
				else // endif (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = (*d & ~ endmask) | (*s-- & endmask);
							*d-- = (*d & ~ endmask) | (*s-- & endmask);
							*d-- = (*d & ~ endmask) | (*s-- & endmask);
							*d-- = (*d & ~ endmask) | (*s-- & endmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							for (swords = dwords; swords > 0; swords--)
								*d-- = *s--,*d-- = *s--,*d-- = *s--,*d-- = *s--;
							*d-- = (*d & ~endmask) | (*s-- & endmask);
							*d-- = (*d & ~endmask) | (*s-- & endmask);
							*d-- = (*d & ~endmask) | (*s-- & endmask);
							*d-- = (*d & ~endmask) | (*s-- & endmask);
							d -= dbpl, s -= sbpl;
						}
					}
				}
			}
			else // endif (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							for (swords = dwords; swords > 0; swords--)
								*d-- = *s--,*d-- = *s--,*d-- = *s--,*d-- = *s--;
							d -= dbpl, s -= sbpl;
						}
					}
				}
				else // (endmask == 0xffff)
				{
					if (!swords)
					{
						begmask &= endmask;
						for (; height > 0; height--)
						{
							*d-- = (*d & ~ endmask) | (*s-- & endmask);
							*d-- = (*d & ~ endmask) | (*s-- & endmask);
							*d-- = (*d & ~ endmask) | (*s-- & endmask);
							*d-- = (*d & ~ endmask) | (*s-- & endmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							for (swords = dwords - 1; swords > 0; swords--)
								*d-- = *s--,*d-- = *s--,*d-- = *s--,*d-- = *s--;
							*d-- = (*d & ~endmask) | (*s-- & endmask);
							*d-- = (*d & ~endmask) | (*s-- & endmask);
							*d-- = (*d & ~endmask) | (*s-- & endmask);
							*d-- = (*d & ~endmask) | (*s-- & endmask);
							d -= dbpl, s -= sbpl;
						}
					}
				}
			}
		}
		else if (shift > 0)
		{
			int rshift = 16 - shift;

			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d-- = (*d & ~begmask) | ((*s-- << shift) & begmask);
						*d-- = (*d & ~begmask) | ((*s-- << shift) & begmask);
						*d-- = (*d & ~begmask) | ((*s-- << shift) & begmask);
						*d-- = (*d & ~begmask) | ((*s-- << shift) & begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | ((p0 << shift) & begmask);
						p1 = *s--;
						*d-- = (*d & ~begmask) | ((p1 << shift) & begmask);
						p2 = *s--;
						*d-- = (*d & ~begmask) | ((p2 << shift) & begmask);
						p3 = *s--;
						*d-- = (*d & ~begmask) | ((p3 << shift) & begmask);
						for (dwords = swords; dwords > 1; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = (p0 >> rshift) | (tmp << shift);
							p0 = tmp;
							tmp = *s--;
							*d-- = (p1 >> rshift) | (tmp << shift);
							p1 = tmp;
							tmp = *s--;
							*d-- = (p2 >> rshift) | (tmp << shift);
							p2 = tmp;
							tmp = *s--;
							*d-- = (p3 >> rshift) | (tmp << shift);
							p3 = tmp;
						}
						*d-- = (*d & ~endmask) | ( ((p0 >> rshift) | (*s-- << shift)) & endmask);
						*d-- = (*d & ~endmask) | ( ((p1 >> rshift) | (*s-- << shift)) & endmask);
						*d-- = (*d & ~endmask) | ( ((p2 >> rshift) | (*s-- << shift)) & endmask);
						*d-- = (*d & ~endmask) | ( ((p3 >> rshift) | (*s-- << shift)) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
			else /* more dest than src words */
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | ((p0 << shift) & begmask);
						p1 = *s--;
						*d-- = (*d & ~begmask) | ((p1 << shift) & begmask);
						p2 = *s--;
						*d-- = (*d & ~begmask) | ((p2 << shift) & begmask);
						p3 = *s--;
						*d-- = (*d & ~begmask) | ((p3 << shift) & begmask);
						
						*d-- = (*d & ~endmask) | ((p0 >> rshift) & endmask);
						*d-- = (*d & ~endmask) | ((p1 >> rshift) & endmask);
						*d-- = (*d & ~endmask) | ((p2 >> rshift) & endmask);
						*d-- = (*d & ~endmask) | ((p3 >> rshift) & endmask);
#if 0
						p0 = *s--;
						*d-- = (*d & ~begmask) | ((p0 << shift) & begmask);
						*d-- = (*d & ~endmask) | ((p0 >> rshift) & endmask);
						d -= dbpl, s -= sbpl;
#endif
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | ((p0 << shift) & begmask);
						p1 = *s--;
						*d-- = (*d & ~begmask) | ((p1 << shift) & begmask);
						p2 = *s--;
						*d-- = (*d & ~begmask) | ((p2 << shift) & begmask);
						p3 = *s--;
						*d-- = (*d & ~begmask) | ((p3 << shift) & begmask);
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = (p0 >> rshift) | (tmp << shift);
							p0 = tmp;
							tmp = *s--;
							*d-- = (p1 >> rshift) | (tmp << shift);
							p1 = tmp;
							tmp = *s--;
							*d-- = (p2 >> rshift) | (tmp << shift);
							p2 = tmp;
							tmp = *s--;
							*d-- = (p3 >> rshift) | (tmp << shift);
							p3 = tmp;
						}
						*d-- = (*d & ~endmask) | ((p0 >> rshift) & endmask);
						*d-- = (*d & ~endmask) | ((p1 >> rshift) & endmask);
						*d-- = (*d & ~endmask) | ((p2 >> rshift) & endmask);
						*d-- = (*d & ~endmask) | ((p3 >> rshift) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
		}
		else /* shift <= 0 */
		{
			int rshift;

			shift = -shift;
			rshift = 16 - shift;

			if (dwords == swords)
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d-- = (*d & ~begmask) | (*s-- & begmask);
						*d-- = (*d & ~begmask) | (*s-- & begmask);
						*d-- = (*d & ~begmask) | (*s-- & begmask);
						*d-- = (*d & ~begmask) | (*s-- & begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--, p1 = *s--, p2 = *s--, p3 = *s--;
						*d-- = (*d & ~begmask) | ( ((p0 >> shift) | (*s << rshift)) & begmask);
						p0 = *s--;
						*d-- = (*d & ~begmask) | ( ((p1 >> shift) | (*s << rshift)) & begmask);
						p1 = *s--;
						*d-- = (*d & ~begmask) | ( ((p2 >> shift) | (*s << rshift)) & begmask);
						p2 = *s--;
						*d-- = (*d & ~begmask) | ( ((p3 >> shift) | (*s << rshift)) & begmask);
						p3 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = (p0 >> shift) | (tmp << rshift);
							p0 = tmp;
							tmp = *s--;
							*d-- = (p1 >> shift) | (tmp << rshift);
							p1 = tmp;
							tmp = *s--;
							*d-- = (p2 >> shift) | (tmp << rshift);
							p2 = tmp;
							tmp = *s--;
							*d-- = (p3 >> shift) | (tmp << rshift);
							p3 = tmp;
						}
						*d-- = (*d & ~endmask) | ((p0 >> shift) & endmask);
						*d-- = (*d & ~endmask) | ((p1 >> shift) & endmask);
						*d-- = (*d & ~endmask) | ((p2 >> shift) & endmask);
						*d-- = (*d & ~endmask) | ((p3 >> shift) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
			else /* more src words */
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s--, p1 = *s--, p2 = *s--, p3 = *s--;
						*d-- = (*d & ~begmask) | (((p0 >> shift) | (*s-- << rshift)) & begmask);
						*d-- = (*d & ~begmask) | (((p0 >> shift) | (*s-- << rshift)) & begmask);
						*d-- = (*d & ~begmask) | (((p0 >> shift) | (*s-- << rshift)) & begmask);
						*d-- = (*d & ~begmask) | (((p0 >> shift) | (*s-- << rshift)) & begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--, p1 = *s--, p2 = *s--, p3 = *s--;
						*d-- = (*d & ~begmask) | (((p0 >> shift) | (*s << rshift)) & begmask);
						p0 = *s--;
						*d-- = (*d & ~begmask) | (((p1 >> shift) | (*s << rshift)) & begmask);
						p1 = *s--;
						*d-- = (*d & ~begmask) | (((p2 >> shift) | (*s << rshift)) & begmask);
						p2 = *s--;
						*d-- = (*d & ~begmask) | (((p3 >> shift) | (*s << rshift)) & begmask);
						p3 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = (p0 >> shift) | (tmp << rshift);
							p0 = tmp;
							tmp = *s--;
							*d-- = (p1 >> shift) | (tmp << rshift);
							p1 = tmp;
							tmp = *s--;
							*d-- = (p2 >> shift) | (tmp << rshift);
							p2 = tmp;
							tmp = *s--;
							*d-- = (p3 >> shift) | (tmp << rshift);
							p3 = tmp;
						}
						*d-- = (*d & ~endmask) | (((p0 >> shift) | (*s-- << rshift)) & endmask);
						*d-- = (*d & ~endmask) | (((p1 >> shift) | (*s-- << rshift)) & endmask);
						*d-- = (*d & ~endmask) | (((p2 >> shift) | (*s-- << rshift)) & endmask);
						*d-- = (*d & ~endmask) | (((p3 >> shift) | (*s-- << rshift)) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
		}			
	}
	else // endif (s < d)
	{
		sbpl -= (swords + 1) << 2;
		dbpl -= (dwords + 1) << 2;
		begmask = 0xffff >> (rpb->dx1 & 0xf);
		endmask = 0xffff << (15 - (rpb->dx2 & 0xf));
		shift	= (rpb->dx1 & 0xf) - (rpb->sx1 & 0xf);

		if (!shift)
		{
			int i;

			if (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					for (;height > 0; height--)
					{
						for (i = swords + 1; i > 0; i--)
							*d++ = *s++,*d++ = *s++,*d++ = *s++,*d++ = *s++;
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (;height > 0; height--)
					{
						for (i = swords; i > 0; i--)
							*d++ = *s++,*d++ = *s++,*d++ = *s++,*d++ = *s++;
						*d++ = (*d & ~endmask) | (*s++ & endmask);
						*d++ = (*d & ~endmask) | (*s++ & endmask);
						*d++ = (*d & ~endmask) | (*s++ & endmask);
						*d++ = (*d & ~endmask) | (*s++ & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							for (i = swords; i > 0; i--)
								*d++ = *s++,*d++ = *s++,*d++ = *s++,*d++ = *s++;
							d += dbpl, s += sbpl;
						}
					}
				}
				else
				{
					if (!swords)
					{
						begmask &= endmask;
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							for (i = swords - 1; i > 0; i--)
								*d++ = *s++,*d++ = *s++,*d++ = *s++,*d++ = *s++;
							*d++ = (*d & ~endmask) | (*s++ & endmask);
							*d++ = (*d & ~endmask) | (*s++ & endmask);
							*d++ = (*d & ~endmask) | (*s++ & endmask);
							*d++ = (*d & ~endmask) | (*s++ & endmask);
							d += dbpl, s += sbpl;
						}
					}
				}
			}
		}
		else if (shift > 0)
		{
			int rshift = 16 - shift;

			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d++ = (*d & ~begmask) | ((*s++ >> shift) & begmask);
						*d++ = (*d & ~begmask) | ((*s++ >> shift) & begmask);
						*d++ = (*d & ~begmask) | ((*s++ >> shift) & begmask);
						*d++ = (*d & ~begmask) | ((*s++ >> shift) & begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | ((p0 >> shift) & begmask);
						p1 = *s++;
						*d++ = (*d & ~begmask) | ((p1 >> shift) & begmask);
						p2 = *s++;
						*d++ = (*d & ~begmask) | ((p2 >> shift) & begmask);
						p3 = *s++;
						*d++ = (*d & ~begmask) | ((p3 >> shift) & begmask);
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = (p0 << rshift) | (tmp >> shift);
							p0 = tmp;
							tmp = *s++;
							*d++ = (p1 << rshift) | (tmp >> shift);
							p1 = tmp;
							tmp = *s++;
							*d++ = (p2 << rshift) | (tmp >> shift);
							p2 = tmp;
							tmp = *s++;
							*d++ = (p3 << rshift) | (tmp >> shift);
							p3 = tmp;
						}
						*d++ = (*d & ~endmask) | (((p0 << rshift) | (*s++ >> shift)) & endmask);
						*d++ = (*d & ~endmask) | (((p1 << rshift) | (*s++ >> shift)) & endmask);
						*d++ = (*d & ~endmask) | (((p2 << rshift) | (*s++ >> shift)) & endmask);
						*d++ = (*d & ~endmask) | (((p3 << rshift) | (*s++ >> shift)) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{	/* should be more destination than source */
				if (!swords)
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | ((p0 >> shift) & begmask);
						p1 = *s++;
						*d++ = (*d & ~begmask) | ((p1 >> shift) & begmask);
						p2 = *s++;
						*d++ = (*d & ~begmask) | ((p2 >> shift) & begmask);
						p3 = *s++;
						*d++ = (*d & ~begmask) | ((p3 >> shift) & begmask);
						*d++ = (*d & ~endmask) | ((p0 << rshift) & endmask);
						*d++ = (*d & ~endmask) | ((p1 << rshift) & endmask);
						*d++ = (*d & ~endmask) | ((p2 << rshift) & endmask);
						*d++ = (*d & ~endmask) | ((p3 << rshift) & endmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | ((p0 >> shift) & begmask);	
						p1 = *s++;
						*d++ = (*d & ~begmask) | ((p1 >> shift) & begmask);	
						p2 = *s++;
						*d++ = (*d & ~begmask) | ((p2 >> shift) & begmask);	
						p3 = *s++;
						*d++ = (*d & ~begmask) | ((p3 >> shift) & begmask);	
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = (p0 << rshift) | (tmp >> shift);
							p0 = tmp;
							tmp = *s++;
							*d++ = (p1 << rshift) | (tmp >> shift);
							p1 = tmp;
							tmp = *s++;
							*d++ = (p2 << rshift) | (tmp >> shift);
							p2 = tmp;
							tmp = *s++;
							*d++ = (p3 << rshift) | (tmp >> shift);
							p3 = tmp;
						}
						*d++ = (*d & ~endmask) | ((p0 << rshift) & endmask);
						*d++ = (*d & ~endmask) | ((p1 << rshift) & endmask);
						*d++ = (*d & ~endmask) | ((p2 << rshift) & endmask);
						*d++ = (*d & ~endmask) | ((p3 << rshift) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
		}
		else /* shift <= 0 */
		{
			int rshift;

			shift = -shift;
			rshift = 16 - shift;
			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d++ = (*d & ~begmask) | ((*s++ << shift) & begmask);
						*d++ = (*d & ~begmask) | ((*s++ << shift) & begmask);
						*d++ = (*d & ~begmask) | ((*s++ << shift) & begmask);
						*d++ = (*d & ~begmask) | ((*s++ << shift) & begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						*d++ = (*d & ~begmask) | (((p0 << shift) | (*s >> rshift)) & begmask);
						p0 = *s++;
						*d++ = (*d & ~begmask) | (((p1 << shift) | (*s >> rshift)) & begmask);
						p1 = *s++;
						*d++ = (*d & ~begmask) | (((p2 << shift) | (*s >> rshift)) & begmask);
						p2 = *s++;
						*d++ = (*d & ~begmask) | (((p3 << shift) | (*s >> rshift)) & begmask);
						p3 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = (p0 << shift) | (tmp >> rshift);
							p0 = tmp;
							tmp = *s++;
							*d++ = (p1 << shift) | (tmp >> rshift);
							p1 = tmp;
							tmp = *s++;
							*d++ = (p2 << shift) | (tmp >> rshift);
							p2 = tmp;
							tmp = *s++;
							*d++ = (p3 << shift) | (tmp >> rshift);
							p3 = tmp;
						}
						*d++ = (*d & ~endmask) | ((p0 << shift) & endmask);
						*d++ = (*d & ~endmask) | ((p1 << shift) & endmask);
						*d++ = (*d & ~endmask) | ((p2 << shift) & endmask);
						*d++ = (*d & ~endmask) | ((p3 << shift) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s++;
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						*d++ = (*d & ~begmask) | (((p0 << shift) | (*s++ >> rshift)) & begmask);
						*d++ = (*d & ~begmask) | (((p1 << shift) | (*s++ >> rshift)) & begmask);
						*d++ = (*d & ~begmask) | (((p2 << shift) | (*s++ >> rshift)) & begmask);
						*d++ = (*d & ~begmask) | (((p3 << shift) | (*s++ >> rshift)) & begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						*d++ = (*d & ~begmask) | (((p0 << shift) | (*s >> rshift)) & begmask);
						p0 = *s++;
						*d++ = (*d & ~begmask) | (((p1 << shift) | (*s >> rshift)) & begmask);
						p1 = *s++;
						*d++ = (*d & ~begmask) | (((p2 << shift) | (*s >> rshift)) & begmask);
						p2 = *s++;
						*d++ = (*d & ~begmask) | (((p3 << shift) | (*s >> rshift)) & begmask);
						p3 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = (p0 << shift) | (tmp >> rshift);
							p0 = tmp;
							tmp = *s++;
							*d++ = (p1 << shift) | (tmp >> rshift);
							p1 = tmp;
							tmp = *s++;
							*d++ = (p2 << shift) | (tmp >> rshift);
							p2 = tmp;
							tmp = *s++;
							*d++ = (p3 << shift) | (tmp >> rshift);
							p3 = tmp;
						}
						*d++ = (*d & ~endmask) | (((p0 << shift) | (*s++ >> rshift)) & endmask);
						*d++ = (*d & ~endmask) | (((p1 << shift) | (*s++ >> rshift)) & endmask);
						*d++ = (*d & ~endmask) | (((p2 << shift) | (*s++ >> rshift)) & endmask);
						*d++ = (*d & ~endmask) | (((p3 << shift) | (*s++ >> rshift)) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
		}
	}
}
static void
rb_S_OR_D_new(ROP_PB *rpb)
{
	unsigned short begmask, endmask, p0, p1, p2, p3;
	int shift, swords, dwords, sbpl, dbpl, height;
	unsigned short *s, *d;

	sbpl = (rpb->s_bypl >> 1);
	dbpl = (rpb->d_bypl >> 1);
	height = rpb->sy2 - rpb->sy1 + 1;
	s = (unsigned short *)rpb->s_addr + (long)((rpb->sx1 >> 4) << 2) + (long)((long)rpb->sy1 * sbpl);
	d = (unsigned short *)rpb->d_addr + (long)((rpb->dx1 >> 4) << 2) + (long)((long)rpb->dy1 * dbpl);
	swords	= (((rpb->sx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;
	dwords	= (((rpb->dx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;

	if (s < d)
	{
		s = (unsigned short *)rpb->s_addr + 3 + (long)((rpb->sx2 >> 4) << 2) + (long)((long)rpb->sy2 * sbpl);
		d = (unsigned short *)rpb->d_addr + 3 + (long)((rpb->dx2 >> 4) << 2) + (long)((long)rpb->dy2 * dbpl);
		sbpl -= (swords + 1) << 2;
		dbpl -= (dwords + 1) << 2;
		begmask = 0xffff << (15 - (rpb->dx2 & 0xf));
		endmask = 0xffff >> (rpb->dx1 & 0xf);
		shift	= (rpb->sx2 & 0xf) - (rpb->dx2 & 0xf);
		if (!shift)
		{
			if (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- |= *s--;
							*d-- |= *s--;
							*d-- |= *s--;
							*d-- |= *s--;
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							for (swords = dwords + 1; swords > 0; swords--)
								*d-- |= *s--,*d-- |= *s--,*d-- |= *s--,*d-- |= *s--;
							d -= dbpl, s -= sbpl;
						}
					}
				}
				else // endif (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							for (swords = dwords; swords > 0; swords--)
								*d-- |= *s--,*d-- |= *s--,*d-- |= *s--,*d-- |= *s--;
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							d -= dbpl, s -= sbpl;
						}
					}
				}
			}
			else // endif (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							for (swords = dwords; swords > 0; swords--)
								*d-- |= *s--,*d-- |= *s--,*d-- |= *s--,*d-- |= *s--;
							d -= dbpl, s -= sbpl;
						}
					}
				}
				else // (endmask == 0xffff)
				{
					if (!swords)
					{
						begmask &= endmask;
						for (; height > 0; height--)
						{
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- | *d) & begmask) | (*d & ~begmask);
							for (swords = dwords - 1; swords > 0; swords--)
								*d-- |= *s--,*d-- |= *s--,*d-- |= *s--,*d-- |= *s--;
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- | *d) & endmask) | (*d & ~endmask);
							d -= dbpl, s -= sbpl;
						}
					}
				}
			}
		}
		else if (shift > 0)
		{
			int rshift = 16 - shift;

			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d-- = (((*s-- << shift) | *d) & begmask) | (*d & ~begmask);
						*d-- = (((*s-- << shift) | *d) & begmask) | (*d & ~begmask);
						*d-- = (((*s-- << shift) | *d) & begmask) | (*d & ~begmask);
						*d-- = (((*s-- << shift) | *d) & begmask) | (*d & ~begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (((p0 << shift) | *d) & begmask)|(*d & ~begmask);
						p1 = *s--;
						*d-- = (((p1 << shift) | *d) & begmask)|(*d & ~begmask);
						p2 = *s--;
						*d-- = (((p2 << shift) | *d) & begmask)|(*d & ~begmask);
						p3 = *s--;
						*d-- = (((p3 << shift) | *d) & begmask)|(*d & ~begmask);
						for (dwords = swords; dwords > 1; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- |= (p0 >> rshift) | (tmp << shift);
							p0 = tmp;
							tmp = *s--;
							*d-- |= (p1 >> rshift) | (tmp << shift);
							p1 = tmp;
							tmp = *s--;
							*d-- |= (p2 >> rshift) | (tmp << shift);
							p2 = tmp;
							tmp = *s--;
							*d-- |= (p3 >> rshift) | (tmp << shift);
							p3 = tmp;
						}
						*d-- = ( (((p0 >> rshift)|(*s-- << shift)) | *d) & endmask) | (*d & ~endmask);
						*d-- = ( (((p0 >> rshift)|(*s-- << shift)) | *d) & endmask) | (*d & ~endmask);
						*d-- = ( (((p0 >> rshift)|(*s-- << shift)) | *d) & endmask) | (*d & ~endmask);
						*d-- = ( (((p0 >> rshift)|(*s-- << shift)) | *d) & endmask) | (*d & ~endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
			else /* more dest than src words */
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (((p0 << shift) | *d) & begmask)|(*d & ~begmask);
						p1 = *s--;
						*d-- = (((p1 << shift) | *d) & begmask)|(*d & ~begmask);
						p2 = *s--;
						*d-- = (((p2 << shift) | *d) & begmask)|(*d & ~begmask);
						p3 = *s--;
						*d-- = (((p3 << shift) | *d) & begmask)|(*d & ~begmask);
						
						*d-- = (((p0 >> rshift) | *d) & endmask)|(*d & ~endmask);
						*d-- = (((p1 >> rshift) | *d) & endmask)|(*d & ~endmask);
						*d-- = (((p2 >> rshift) | *d) & endmask)|(*d & ~endmask);
						*d-- = (((p3 >> rshift) | *d) & endmask)|(*d & ~endmask);
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (((p0 << shift) | *d) & begmask)|(*d & ~begmask);
						p1 = *s--;
						*d-- = (((p1 << shift) | *d) & begmask)|(*d & ~begmask);
						p2 = *s--;
						*d-- = (((p2 << shift) | *d) & begmask)|(*d & ~begmask);
						p3 = *s--;
						*d-- = (((p3 << shift) | *d) & begmask)|(*d & ~begmask);
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- |= (p0 >> rshift) | (tmp << shift);
							p0 = tmp;
							tmp = *s--;
							*d-- |= (p1 >> rshift) | (tmp << shift);
							p1 = tmp;
							tmp = *s--;
							*d-- |= (p2 >> rshift) | (tmp << shift);
							p2 = tmp;
							tmp = *s--;
							*d-- |= (p3 >> rshift) | (tmp << shift);
							p3 = tmp;
						}
						*d-- = (((p0 >> rshift) | *d) & endmask)|(*d & ~endmask);
						*d-- = (((p1 >> rshift) | *d) & endmask)|(*d & ~endmask);
						*d-- = (((p2 >> rshift) | *d) & endmask)|(*d & ~endmask);
						*d-- = (((p3 >> rshift) | *d) & endmask)|(*d & ~endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
		}
		else /* shift <= 0 */
		{
			int rshift;

			shift = -shift;
			rshift = 16 - shift;

			if (dwords == swords)
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d-- = ((*s-- & begmask) | *d) | (*d & ~begmask);
						*d-- = ((*s-- & begmask) | *d) | (*d & ~begmask);
						*d-- = ((*s-- & begmask) | *d) | (*d & ~begmask);
						*d-- = ((*s-- & begmask) | *d) | (*d & ~begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--, p1 = *s--, p2 = *s--, p3 = *s--;
						*d-- =  ( (((p0 >> shift)|(*s << rshift)) | *d) & begmask)|(*d & ~begmask);
						p0 = *s--;
						*d-- =  ( (((p1 >> shift)|(*s << rshift)) | *d) & begmask)|(*d & ~begmask);
						p1 = *s--;
						*d-- =  ( (((p2 >> shift)|(*s << rshift)) | *d) & begmask)|(*d & ~begmask);
						p2 = *s--;
						*d-- =  ( (((p3 >> shift)|(*s << rshift)) | *d) & begmask)|(*d & ~begmask);
						p3 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- |= (p0 >> shift) | (tmp << rshift);
							p0 = tmp;
							tmp = *s--;
							*d-- |= (p1 >> shift) | (tmp << rshift);
							p1 = tmp;
							tmp = *s--;
							*d-- |= (p2 >> shift) | (tmp << rshift);
							p2 = tmp;
							tmp = *s--;
							*d-- |= (p3 >> shift) | (tmp << rshift);
							p3 = tmp;
						}
						*d-- =  (((p0 >> shift) | *d) & endmask) | (*d & ~endmask);
						*d-- =  (((p1 >> shift) | *d) & endmask) | (*d & ~endmask);
						*d-- =  (((p2 >> shift) | *d) & endmask) | (*d & ~endmask);
						*d-- =  (((p3 >> shift) | *d) & endmask) | (*d & ~endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
			else /* more src words */
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s--, p1 = *s--, p2 = *s--, p3 = *s--;
						*d-- =  ((((p0 >> shift) | (*s-- << rshift)) | *d) & begmask) | (*d & ~begmask);
						*d-- =  ((((p1 >> shift) | (*s-- << rshift)) | *d) & begmask) | (*d & ~begmask);
						*d-- =  ((((p2 >> shift) | (*s-- << rshift)) | *d) & begmask) | (*d & ~begmask);
						*d-- =  ((((p3 >> shift) | (*s-- << rshift)) | *d) & begmask) | (*d & ~begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--, p1 = *s--, p2 = *s--, p3 = *s--;
						*d-- = ((((p0 >> shift) | (*s << rshift)) | *d) & begmask) | (*d & ~begmask);
						p0 = *s--;
						*d-- = ((((p1 >> shift) | (*s << rshift)) | *d) & begmask) | (*d & ~begmask);
						p1 = *s--;
						*d-- = ((((p2 >> shift) | (*s << rshift)) | *d) & begmask) | (*d & ~begmask);
						p2 = *s--;
						*d-- = ((((p3 >> shift) | (*s << rshift)) | *d) & begmask) | (*d & ~begmask);
						p3 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- |= (p0 >> shift) | (tmp << rshift);
							p0 = tmp;
							tmp = *s--;
							*d-- |= (p1 >> shift) | (tmp << rshift);
							p1 = tmp;
							tmp = *s--;
							*d-- |= (p2 >> shift) | (tmp << rshift);
							p2 = tmp;
							tmp = *s--;
							*d-- |= (p3 >> shift) | (tmp << rshift);
							p3 = tmp;
						}
						*d-- = ((((p0 >> shift) | (*s-- << rshift)) | *d) & endmask) | (*d & ~endmask);
						*d-- = ((((p1 >> shift) | (*s-- << rshift)) | *d) & endmask) | (*d & ~endmask);
						*d-- = ((((p2 >> shift) | (*s-- << rshift)) | *d) & endmask) | (*d & ~endmask);
						*d-- = ((((p3 >> shift) | (*s-- << rshift)) | *d) & endmask) | (*d & ~endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
		}			
	}
	else // endif (s < d)
	{
		sbpl -= (swords + 1) << 2;
		dbpl -= (dwords + 1) << 2;
		begmask = 0xffff >> (rpb->dx1 & 0xf);
		endmask = 0xffff << (15 - (rpb->dx2 & 0xf));
		shift	= (rpb->dx1 & 0xf) - (rpb->sx1 & 0xf);

		if (!shift)
		{
			int i;

			if (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					for (;height > 0; height--)
					{
						for (i = swords + 1; i > 0; i--)
							*d++ |= *s++,*d++ |= *s++,*d++ |= *s++,*d++ |= *s++;
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (;height > 0; height--)
					{
						for (i = swords; i > 0; i--)
							*d++ |= *s++,*d++ |= *s++,*d++ |= *s++,*d++ |= *s++;
						*d++ = ((*s++ & endmask) | *d) | (*d & ~endmask);
						*d++ = ((*s++ & endmask) | *d) | (*d & ~endmask);
						*d++ = ((*s++ & endmask) | *d) | (*d & ~endmask);
						*d++ = ((*s++ & endmask) | *d) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							for (i = swords; i > 0; i--)
								*d++ |= *s++,*d++ |= *s++,*d++ |= *s++,*d++ |= *s++;
							d += dbpl, s += sbpl;
						}
					}
				}
				else
				{
					if (!swords)
					{
						begmask &= endmask;
						for (; height > 0; height--)
						{
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) | *d) | (*d & ~begmask);
							for (i = swords - 1; i > 0; i--)
								*d++ |= *s++,*d++ |= *s++,*d++ |= *s++,*d++ |= *s++;
							*d++ = ((*s++ & endmask) | *d) | (*d & ~endmask);
							*d++ = ((*s++ & endmask) | *d) | (*d & ~endmask);
							*d++ = ((*s++ & endmask) | *d) | (*d & ~endmask);
							*d++ = ((*s++ & endmask) | *d) | (*d & ~endmask);
							d += dbpl, s += sbpl;
						}
					}
				}
			}
		}
		else if (shift > 0)
		{
			int rshift = 16 - shift;

			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d++ = (((*s++ >> shift) | *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ >> shift) | *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ >> shift) | *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ >> shift) | *d) & begmask) | (*d & ~begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (((p0 >> shift) | *d) & begmask) | (*d & ~begmask);
						p1 = *s++;
						*d++ = (((p1 >> shift) | *d) & begmask) | (*d & ~begmask);
						p2 = *s++;
						*d++ = (((p2 >> shift) | *d) & begmask) | (*d & ~begmask);
						p3 = *s++;
						*d++ = (((p3 >> shift) | *d) & begmask) | (*d & ~begmask);
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ |= (p0 << rshift) | (tmp >> shift);
							p0 = tmp;
							tmp = *s++;
							*d++ |= (p1 << rshift) | (tmp >> shift);
							p1 = tmp;
							tmp = *s++;
							*d++ |= (p2 << rshift) | (tmp >> shift);
							p2 = tmp;
							tmp = *s++;
							*d++ |= (p3 << rshift) | (tmp >> shift);
							p3 = tmp;
						}
						*d++ = ((((p0 << rshift) | (*s++ >> shift)) | *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p1 << rshift) | (*s++ >> shift)) | *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p2 << rshift) | (*s++ >> shift)) | *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p3 << rshift) | (*s++ >> shift)) | *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{	/* should be more destination than source */
				if (!swords)
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (((p0 >> shift) | *d) & begmask) | (*d & ~begmask);
						p1 = *s++;
						*d++ = (((p1 >> shift) | *d) & begmask) | (*d & ~begmask);
						p2 = *s++;
						*d++ = (((p2 >> shift) | *d) & begmask) | (*d & ~begmask);
						p3 = *s++;
						*d++ = (((p3 >> shift) | *d) & begmask) | (*d & ~begmask);
						*d++ = (((p0 << rshift) | *d) & endmask) | (*d & ~endmask);
						*d++ = (((p1 << rshift) | *d) & endmask) | (*d & ~endmask);
						*d++ = (((p2 << rshift) | *d) & endmask) | (*d & ~endmask);
						*d++ = (((p3 << rshift) | *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (((p0 >> shift) | *d) & begmask) | (*d & ~begmask);	
						p1 = *s++;
						*d++ = (((p1 >> shift) | *d) & begmask) | (*d & ~begmask);	
						p2 = *s++;
						*d++ = (((p2 >> shift) | *d) & begmask) | (*d & ~begmask);	
						p3 = *s++;
						*d++ = (((p3 >> shift) | *d) & begmask) | (*d & ~begmask);	
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ |= (p0 << rshift) | (tmp >> shift);
							p0 = tmp;
							tmp = *s++;
							*d++ |= (p1 << rshift) | (tmp >> shift);
							p1 = tmp;
							tmp = *s++;
							*d++ |= (p2 << rshift) | (tmp >> shift);
							p2 = tmp;
							tmp = *s++;
							*d++ |= (p3 << rshift) | (tmp >> shift);
							p3 = tmp;
						}
						*d++ = (((p0 << rshift) | *d) & endmask) | (*d & ~endmask);
						*d++ = (((p1 << rshift) | *d) & endmask) | (*d & ~endmask);
						*d++ = (((p2 << rshift) | *d) & endmask) | (*d & ~endmask);
						*d++ = (((p3 << rshift) | *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
		}
		else /* shift <= 0 */
		{
			int rshift;

			shift = -shift;
			rshift = 16 - shift;
			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d++ = (((*s++ << shift) | *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ << shift) | *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ << shift) | *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ << shift) | *d) & begmask) | (*d & ~begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						*d++ = ((((p0 << shift) | (*s >> rshift)) | *d) & begmask) | (*d & ~begmask);
						p0 = *s++;
						*d++ = ((((p1 << shift) | (*s >> rshift)) | *d) & begmask) | (*d & ~begmask);
						p1 = *s++;
						*d++ = ((((p2 << shift) | (*s >> rshift)) | *d) & begmask) | (*d & ~begmask);
						p2 = *s++;
						*d++ = ((((p3 << shift) | (*s >> rshift)) | *d) & begmask) | (*d & ~begmask);
						p3 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ |= (p0 << shift) | (tmp >> rshift);
							p0 = tmp;
							tmp = *s++;
							*d++ |= (p1 << shift) | (tmp >> rshift);
							p1 = tmp;
							tmp = *s++;
							*d++ |= (p2 << shift) | (tmp >> rshift);
							p2 = tmp;
							tmp = *s++;
							*d++ |= (p3 << shift) | (tmp >> rshift);
							p3 = tmp;
						}
						*d++ = (((p0 << shift) | *d) & endmask) | (*d & ~endmask);
						*d++ = (((p1 << shift) | *d) & endmask) | (*d & ~endmask);
						*d++ = (((p2 << shift) | *d) & endmask) | (*d & ~endmask);
						*d++ = (((p3 << shift) | *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s++;
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						*d++ = ((((p0 << shift) | (*s++ >> rshift)) | *d) & begmask) | (*d & ~begmask);
						*d++ = ((((p1 << shift) | (*s++ >> rshift)) | *d) & begmask) | (*d & ~begmask);
						*d++ = ((((p2 << shift) | (*s++ >> rshift)) | *d) & begmask) | (*d & ~begmask);
						*d++ = ((((p3 << shift) | (*s++ >> rshift)) | *d) & begmask) | (*d & ~begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						*d++ = ((((p0 << shift) | (*s >> rshift)) | *d) & begmask) | (*d & ~begmask);
						p0 = *s++;
						*d++ = ((((p1 << shift) | (*s >> rshift)) | *d) & begmask) | (*d & ~begmask);
						p1 = *s++;
						*d++ = ((((p2 << shift) | (*s >> rshift)) | *d) & begmask) | (*d & ~begmask);
						p2 = *s++;
						*d++ = ((((p3 << shift) | (*s >> rshift)) | *d) & begmask) | (*d & ~begmask);
						p3 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ |= (p0 << shift) | (tmp >> rshift);
							p0 = tmp;
							tmp = *s++;
							*d++ |= (p1 << shift) | (tmp >> rshift);
							p1 = tmp;
							tmp = *s++;
							*d++ |= (p2 << shift) | (tmp >> rshift);
							p2 = tmp;
							tmp = *s++;
							*d++ |= (p3 << shift) | (tmp >> rshift);
							p3 = tmp;
						}
						*d++ = ((((p0 << shift) | (*s++ >> rshift)) | *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p1 << shift) | (*s++ >> rshift)) | *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p2 << shift) | (*s++ >> rshift)) | *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p3 << shift) | (*s++ >> rshift)) | *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
		}
	}
}
static void
rb_S_XOR_D(ROP_PB *rpb)
{
	unsigned short begmask, endmask, p0, p1, p2, p3;
	int shift, swords, dwords, sbpl, dbpl, height;
	unsigned short *s, *d;

	sbpl = (rpb->s_bypl >> 1);
	dbpl = (rpb->d_bypl >> 1);
	height = rpb->sy2 - rpb->sy1 + 1;
	s = (unsigned short *)rpb->s_addr + (long)((rpb->sx1 >> 4) << 2) + (long)((long)rpb->sy1 * sbpl);
	d = (unsigned short *)rpb->d_addr + (long)((rpb->dx1 >> 4) << 2) + (long)((long)rpb->dy1 * dbpl);
	swords	= (((rpb->sx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;
	dwords	= (((rpb->dx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;

	if (s < d)
	{
		s = (unsigned short *)rpb->s_addr + 3 + (long)((rpb->sx2 >> 4) << 2) + (long)((long)rpb->sy2 * sbpl);
		d = (unsigned short *)rpb->d_addr + 3 + (long)((rpb->dx2 >> 4) << 2) + (long)((long)rpb->dy2 * dbpl);
		sbpl -= (swords + 1) << 2;
		dbpl -= (dwords + 1) << 2;
		begmask = 0xffff << (15 - (rpb->dx2 & 0xf));
		endmask = 0xffff >> (rpb->dx1 & 0xf);
		shift	= (rpb->sx2 & 0xf) - (rpb->dx2 & 0xf);
		if (!shift)
		{
			if (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- ^= *s--;
							*d-- ^= *s--;
							*d-- ^= *s--;
							*d-- ^= *s--;
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							for (swords = dwords + 1; swords > 0; swords--)
								*d-- ^= *s--,*d-- ^= *s--,*d-- ^= *s--,*d-- ^= *s--;
							d -= dbpl, s -= sbpl;
						}
					}
				}
				else // endif (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							for (swords = dwords; swords > 0; swords--)
								*d-- ^= *s--,*d-- ^= *s--,*d-- ^= *s--,*d-- ^= *s--;
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							d -= dbpl, s -= sbpl;
						}
					}
				}
			}
			else // endif (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							for (swords = dwords; swords > 0; swords--)
								*d-- ^= *s--,*d-- ^= *s--,*d-- ^= *s--,*d-- ^= *s--;
							d -= dbpl, s -= sbpl;
						}
					}
				}
				else // (endmask == 0xffff)
				{
					if (!swords)
					{
						begmask &= endmask;
						for (; height > 0; height--)
						{
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							*d-- = ((*s-- ^ *d) & begmask) | (*d & ~begmask);
							for (swords = dwords - 1; swords > 0; swords--)
								*d-- ^= *s--,*d-- ^= *s--,*d-- ^= *s--,*d-- ^= *s--;
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							*d-- = ((*s-- ^ *d) & endmask) | (*d & ~endmask);
							d -= dbpl, s -= sbpl;
						}
					}
				}
			}
		}
		else if (shift > 0)
		{
			int rshift = 16 - shift;

			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d-- = (((*s-- << shift) ^ *d) & begmask) | (*d & ~begmask);
						*d-- = (((*s-- << shift) ^ *d) & begmask) | (*d & ~begmask);
						*d-- = (((*s-- << shift) ^ *d) & begmask) | (*d & ~begmask);
						*d-- = (((*s-- << shift) ^ *d) & begmask) | (*d & ~begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (((p0 << shift) ^ *d) & begmask)|(*d & ~begmask);
						p1 = *s--;
						*d-- = (((p1 << shift) ^ *d) & begmask)|(*d & ~begmask);
						p2 = *s--;
						*d-- = (((p2 << shift) ^ *d) & begmask)|(*d & ~begmask);
						p3 = *s--;
						*d-- = (((p3 << shift) ^ *d) & begmask)|(*d & ~begmask);
						for (dwords = swords; dwords > 1; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- ^= (p0 >> rshift) | (tmp << shift);
							p0 = tmp;
							tmp = *s--;
							*d-- ^= (p1 >> rshift) | (tmp << shift);
							p1 = tmp;
							tmp = *s--;
							*d-- ^= (p2 >> rshift) | (tmp << shift);
							p2 = tmp;
							tmp = *s--;
							*d-- ^= (p3 >> rshift) | (tmp << shift);
							p3 = tmp;
						}
						*d-- = ( (((p0 >> rshift)|(*s-- << shift)) ^ *d) & endmask) | (*d & ~endmask);
						*d-- = ( (((p0 >> rshift)|(*s-- << shift)) ^ *d) & endmask) | (*d & ~endmask);
						*d-- = ( (((p0 >> rshift)|(*s-- << shift)) ^ *d) & endmask) | (*d & ~endmask);
						*d-- = ( (((p0 >> rshift)|(*s-- << shift)) ^ *d) & endmask) | (*d & ~endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
			else /* more dest than src words */
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (((p0 << shift) ^ *d) & begmask)|(*d & ~begmask);
						p1 = *s--;
						*d-- = (((p1 << shift) ^ *d) & begmask)|(*d & ~begmask);
						p2 = *s--;
						*d-- = (((p2 << shift) ^ *d) & begmask)|(*d & ~begmask);
						p3 = *s--;
						*d-- = (((p3 << shift) ^ *d) & begmask)|(*d & ~begmask);
						
						*d-- = (((p0 >> rshift) ^ *d) & endmask)|(*d & ~endmask);
						*d-- = (((p1 >> rshift) ^ *d) & endmask)|(*d & ~endmask);
						*d-- = (((p2 >> rshift) ^ *d) & endmask)|(*d & ~endmask);
						*d-- = (((p3 >> rshift) ^ *d) & endmask)|(*d & ~endmask);
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (((p0 << shift) ^ *d) & begmask)|(*d & ~begmask);
						p1 = *s--;
						*d-- = (((p1 << shift) ^ *d) & begmask)|(*d & ~begmask);
						p2 = *s--;
						*d-- = (((p2 << shift) ^ *d) & begmask)|(*d & ~begmask);
						p3 = *s--;
						*d-- = (((p3 << shift) ^ *d) & begmask)|(*d & ~begmask);
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- ^= (p0 >> rshift) | (tmp << shift);
							p0 = tmp;
							tmp = *s--;
							*d-- ^= (p1 >> rshift) | (tmp << shift);
							p1 = tmp;
							tmp = *s--;
							*d-- ^= (p2 >> rshift) | (tmp << shift);
							p2 = tmp;
							tmp = *s--;
							*d-- ^= (p3 >> rshift) | (tmp << shift);
							p3 = tmp;
						}
						*d-- = (((p0 >> rshift) ^ *d) & endmask)|(*d & ~endmask);
						*d-- = (((p1 >> rshift) ^ *d) & endmask)|(*d & ~endmask);
						*d-- = (((p2 >> rshift) ^ *d) & endmask)|(*d & ~endmask);
						*d-- = (((p3 >> rshift) ^ *d) & endmask)|(*d & ~endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
		}
		else /* shift <= 0 */
		{
			int rshift;

			shift = -shift;
			rshift = 16 - shift;

			if (dwords == swords)
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d-- = ((*s-- & begmask) ^ *d) | (*d & ~begmask);
						*d-- = ((*s-- & begmask) ^ *d) | (*d & ~begmask);
						*d-- = ((*s-- & begmask) ^ *d) | (*d & ~begmask);
						*d-- = ((*s-- & begmask) ^ *d) | (*d & ~begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--, p1 = *s--, p2 = *s--, p3 = *s--;
						*d-- =  ( (((p0 >> shift)|(*s << rshift)) ^ *d) & begmask)|(*d & ~begmask);
						p0 = *s--;
						*d-- =  ( (((p1 >> shift)|(*s << rshift)) ^ *d) & begmask)|(*d & ~begmask);
						p1 = *s--;
						*d-- =  ( (((p2 >> shift)|(*s << rshift)) ^ *d) & begmask)|(*d & ~begmask);
						p2 = *s--;
						*d-- =  ( (((p3 >> shift)|(*s << rshift)) ^ *d) & begmask)|(*d & ~begmask);
						p3 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- ^= (p0 >> shift) | (tmp << rshift);
							p0 = tmp;
							tmp = *s--;
							*d-- ^= (p1 >> shift) | (tmp << rshift);
							p1 = tmp;
							tmp = *s--;
							*d-- ^= (p2 >> shift) | (tmp << rshift);
							p2 = tmp;
							tmp = *s--;
							*d-- ^= (p3 >> shift) | (tmp << rshift);
							p3 = tmp;
						}
						*d-- =  (((p0 >> shift) ^ *d) & endmask) | (*d & ~endmask);
						*d-- =  (((p1 >> shift) ^ *d) & endmask) | (*d & ~endmask);
						*d-- =  (((p2 >> shift) ^ *d) & endmask) | (*d & ~endmask);
						*d-- =  (((p3 >> shift) ^ *d) & endmask) | (*d & ~endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
			else /* more src words */
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s--, p1 = *s--, p2 = *s--, p3 = *s--;
						*d-- =  ((((p0 >> shift) | (*s-- << rshift)) ^ *d) & begmask) | (*d & ~begmask);
						*d-- =  ((((p1 >> shift) | (*s-- << rshift)) ^ *d) & begmask) | (*d & ~begmask);
						*d-- =  ((((p2 >> shift) | (*s-- << rshift)) ^ *d) & begmask) | (*d & ~begmask);
						*d-- =  ((((p3 >> shift) | (*s-- << rshift)) ^ *d) & begmask) | (*d & ~begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--, p1 = *s--, p2 = *s--, p3 = *s--;
						*d-- = ((((p0 >> shift) | (*s << rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p0 = *s--;
						*d-- = ((((p1 >> shift) | (*s << rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p1 = *s--;
						*d-- = ((((p2 >> shift) | (*s << rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p2 = *s--;
						*d-- = ((((p3 >> shift) | (*s << rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p3 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- ^= (p0 >> shift) | (tmp << rshift);
							p0 = tmp;
							tmp = *s--;
							*d-- ^= (p1 >> shift) | (tmp << rshift);
							p1 = tmp;
							tmp = *s--;
							*d-- ^= (p2 >> shift) | (tmp << rshift);
							p2 = tmp;
							tmp = *s--;
							*d-- ^= (p3 >> shift) | (tmp << rshift);
							p3 = tmp;
						}
						*d-- = ((((p0 >> shift) | (*s-- << rshift)) ^ *d) & endmask) | (*d & ~endmask);
						*d-- = ((((p1 >> shift) | (*s-- << rshift)) ^ *d) & endmask) | (*d & ~endmask);
						*d-- = ((((p2 >> shift) | (*s-- << rshift)) ^ *d) & endmask) | (*d & ~endmask);
						*d-- = ((((p3 >> shift) | (*s-- << rshift)) ^ *d) & endmask) | (*d & ~endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
		}			
	}
	else // endif (s < d)
	{
		sbpl -= (swords + 1) << 2;
		dbpl -= (dwords + 1) << 2;
		begmask = 0xffff >> (rpb->dx1 & 0xf);
		endmask = 0xffff << (15 - (rpb->dx2 & 0xf));
		shift	= (rpb->dx1 & 0xf) - (rpb->sx1 & 0xf);

		if (!shift)
		{
			int i;

			if (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					for (;height > 0; height--)
					{
						for (i = swords + 1; i > 0; i--)
							*d++ ^= *s++,*d++ ^= *s++,*d++ ^= *s++,*d++ ^= *s++;
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (;height > 0; height--)
					{
						for (i = swords; i > 0; i--)
							*d++ ^= *s++,*d++ ^= *s++,*d++ ^= *s++,*d++ ^= *s++;
						*d++ = ((*s++ & endmask) ^ *d) | (*d & ~endmask);
						*d++ = ((*s++ & endmask) ^ *d) | (*d & ~endmask);
						*d++ = ((*s++ & endmask) ^ *d) | (*d & ~endmask);
						*d++ = ((*s++ & endmask) ^ *d) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							for (i = swords; i > 0; i--)
								*d++ ^= *s++,*d++ ^= *s++,*d++ ^= *s++,*d++ ^= *s++;
							d += dbpl, s += sbpl;
						}
					}
				}
				else
				{
					if (!swords)
					{
						begmask &= endmask;
						for (; height > 0; height--)
						{
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							*d++ = ((*s++ & begmask) ^ *d) | (*d & ~begmask);
							for (i = swords - 1; i > 0; i--)
								*d++ ^= *s++,*d++ ^= *s++,*d++ ^= *s++,*d++ ^= *s++;
							*d++ = ((*s++ & endmask) ^ *d) | (*d & ~endmask);
							*d++ = ((*s++ & endmask) ^ *d) | (*d & ~endmask);
							*d++ = ((*s++ & endmask) ^ *d) | (*d & ~endmask);
							*d++ = ((*s++ & endmask) ^ *d) | (*d & ~endmask);
							d += dbpl, s += sbpl;
						}
					}
				}
			}
		}
		else if (shift > 0)
		{
			int rshift = 16 - shift;

			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d++ = (((*s++ >> shift) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ >> shift) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ >> shift) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ >> shift) ^ *d) & begmask) | (*d & ~begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (((p0 >> shift) ^ *d) & begmask) | (*d & ~begmask);
						p1 = *s++;
						*d++ = (((p1 >> shift) ^ *d) & begmask) | (*d & ~begmask);
						p2 = *s++;
						*d++ = (((p2 >> shift) ^ *d) & begmask) | (*d & ~begmask);
						p3 = *s++;
						*d++ = (((p3 >> shift) ^ *d) & begmask) | (*d & ~begmask);
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ ^= (p0 << rshift) | (tmp >> shift);
							p0 = tmp;
							tmp = *s++;
							*d++ ^= (p1 << rshift) | (tmp >> shift);
							p1 = tmp;
							tmp = *s++;
							*d++ ^= (p2 << rshift) | (tmp >> shift);
							p2 = tmp;
							tmp = *s++;
							*d++ ^= (p3 << rshift) | (tmp >> shift);
							p3 = tmp;
						}
						*d++ = ((((p0 << rshift) | (*s++ >> shift)) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p1 << rshift) | (*s++ >> shift)) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p2 << rshift) | (*s++ >> shift)) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p3 << rshift) | (*s++ >> shift)) ^ *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{	/* should be more destination than source */
				if (!swords)
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (((p0 >> shift) ^ *d) & begmask) | (*d & ~begmask);
						p1 = *s++;
						*d++ = (((p1 >> shift) ^ *d) & begmask) | (*d & ~begmask);
						p2 = *s++;
						*d++ = (((p2 >> shift) ^ *d) & begmask) | (*d & ~begmask);
						p3 = *s++;
						*d++ = (((p3 >> shift) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = (((p0 << rshift) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = (((p1 << rshift) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = (((p2 << rshift) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = (((p3 << rshift) ^ *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (((p0 >> shift) ^ *d) & begmask) | (*d & ~begmask);	
						p1 = *s++;
						*d++ = (((p1 >> shift) ^ *d) & begmask) | (*d & ~begmask);	
						p2 = *s++;
						*d++ = (((p2 >> shift) ^ *d) & begmask) | (*d & ~begmask);	
						p3 = *s++;
						*d++ = (((p3 >> shift) ^ *d) & begmask) | (*d & ~begmask);	
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ ^= (p0 << rshift) | (tmp >> shift);
							p0 = tmp;
							tmp = *s++;
							*d++ ^= (p1 << rshift) | (tmp >> shift);
							p1 = tmp;
							tmp = *s++;
							*d++ ^= (p2 << rshift) | (tmp >> shift);
							p2 = tmp;
							tmp = *s++;
							*d++ ^= (p3 << rshift) | (tmp >> shift);
							p3 = tmp;
						}
						*d++ = (((p0 << rshift) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = (((p1 << rshift) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = (((p2 << rshift) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = (((p3 << rshift) ^ *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
		}
		else /* shift <= 0 */
		{
			int rshift;

			shift = -shift;
			rshift = 16 - shift;
			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d++ = (((*s++ << shift) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ << shift) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ << shift) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = (((*s++ << shift) ^ *d) & begmask) | (*d & ~begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						*d++ = ((((p0 << shift) | (*s >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p0 = *s++;
						*d++ = ((((p1 << shift) | (*s >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p1 = *s++;
						*d++ = ((((p2 << shift) | (*s >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p2 = *s++;
						*d++ = ((((p3 << shift) | (*s >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p3 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ ^= (p0 << shift) | (tmp >> rshift);
							p0 = tmp;
							tmp = *s++;
							*d++ ^= (p1 << shift) | (tmp >> rshift);
							p1 = tmp;
							tmp = *s++;
							*d++ ^= (p2 << shift) | (tmp >> rshift);
							p2 = tmp;
							tmp = *s++;
							*d++ ^= (p3 << shift) | (tmp >> rshift);
							p3 = tmp;
						}
						*d++ = (((p0 << shift) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = (((p1 << shift) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = (((p2 << shift) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = (((p3 << shift) ^ *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s++;
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						*d++ = ((((p0 << shift) | (*s++ >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = ((((p1 << shift) | (*s++ >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = ((((p2 << shift) | (*s++ >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						*d++ = ((((p3 << shift) | (*s++ >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						p1 = *s++;
						p2 = *s++;
						p3 = *s++;
						*d++ = ((((p0 << shift) | (*s >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p0 = *s++;
						*d++ = ((((p1 << shift) | (*s >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p1 = *s++;
						*d++ = ((((p2 << shift) | (*s >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p2 = *s++;
						*d++ = ((((p3 << shift) | (*s >> rshift)) ^ *d) & begmask) | (*d & ~begmask);
						p3 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ ^= (p0 << shift) | (tmp >> rshift);
							p0 = tmp;
							tmp = *s++;
							*d++ ^= (p1 << shift) | (tmp >> rshift);
							p1 = tmp;
							tmp = *s++;
							*d++ ^= (p2 << shift) | (tmp >> rshift);
							p2 = tmp;
							tmp = *s++;
							*d++ ^= (p3 << shift) | (tmp >> rshift);
							p3 = tmp;
						}
						*d++ = ((((p0 << shift) | (*s++ >> rshift)) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p1 << shift) | (*s++ >> rshift)) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p2 << shift) | (*s++ >> rshift)) ^ *d) & endmask) | (*d & ~endmask);
						*d++ = ((((p3 << shift) | (*s++ >> rshift)) ^ *d) & endmask) | (*d & ~endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
		}
	}
}
static void
rb_NOT_S(ROP_PB *rpb)
{
	short i, width, height, s_shift, d_shift, shift, hbits, ebits, groups, spans, sbpl, dbpl;
	unsigned short begmask, endmask, data, p0, p1, p2, p3;
	register unsigned short *src, *dst, *s, *d;
	
	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sbpl	= rpb->s_bypl >> 1;
	dbpl	= rpb->d_bypl >> 1;

	src = (unsigned short *)rpb->s_addr + (long)((rpb->sx1 >> 4) << 2) + (long)((long)rpb->sy1 * sbpl);
	dst = (unsigned short *)rpb->d_addr + (long)((rpb->dx1 >> 4) << 2) + (long)((long)rpb->dy1 * dbpl);

	if (src < dst)
	{
		src = (unsigned short *)rpb->s_addr + 3 + (long)((rpb->sx2 >> 4) << 2) + (long)((long)rpb->sy2 * sbpl);
		dst = (unsigned short *)rpb->d_addr + 3 + (long)((rpb->dx2 >> 4) << 2) + (long)((long)rpb->dy2 * dbpl);

		s_shift	= 15 - (rpb->sx2 & 0xf);
		d_shift	= 15 - (rpb->dx2 & 0xf);

		if (!s_shift && !d_shift)
		{
		/* source & destination starts on whole word */

			endmask = 0xffff >> (rpb->sx1 & 0xf);

			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				for (i = width >> 4; i > 0; i--)
				{
					*d-- = ~*s--;
					*d-- = ~*s--;
					*d-- = ~*s--;
					*d-- = ~*s--;
				}
				if (endmask != 0xffff)
				{
					*d-- = (~*s-- & endmask) | (*d & ~endmask);
					*d-- = (~*s-- & endmask) | (*d & ~endmask);
					*d-- = (~*s-- & endmask) | (*d & ~endmask);
					*d-- = (~*s-- & endmask) | (*d & ~endmask);
				}
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
				{
					*d-- = (~*s-- & begmask) | (*d & ~begmask);
					*d-- = (~*s-- & begmask) | (*d & ~begmask);
					*d-- = (~*s-- & begmask) | (*d & ~begmask);
					*d-- = (~*s-- & begmask) | (*d & ~begmask);
				}
				for (i = groups; i > 0; i--)
				{
					*d-- = ~*s--;
					*d-- = ~*s--;
					*d-- = ~*s--;
					*d-- = ~*s--;
				}
				if (ebits)
				{
					*d-- = (~*s-- & endmask) | (*d & ~endmask);
					*d-- = (~*s-- & endmask) | (*d & ~endmask);
					*d-- = (~*s-- & endmask) | (*d & ~endmask);
					*d-- = (~*s-- & endmask) | (*d & ~endmask);
				}

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
					p0 = ~*s-- >> shift;
					p1 = ~*s-- >> shift;
					p2 = ~*s-- >> shift;
					p3 = ~*s-- >> shift;
					if ((s_shift + hbits) > 16)
					{
						p0 |= ~*s     << (16 - shift);
						p1 |= ~*(s-1) << (16 - shift);
						p2 |= ~*(s-2) << (16 - shift);
						p3 |= ~*(s-3) << (16 - shift);
					}
					*d-- = (p0 & begmask) | (*d & ~ begmask);
					*d-- = (p1 & begmask) | (*d & ~ begmask);
					*d-- = (p2 & begmask) | (*d & ~ begmask);
					*d-- = (p3 & begmask) | (*d & ~ begmask);

				}

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						p0    = ~*s-- >> shift;
						p1    = ~*s-- >> shift;
						p2    = ~*s-- >> shift;
						p3    = ~*s-- >> shift;

						p0   |= ~*s-- << (16 - shift);
						p1   |= ~*s-- << (16 - shift);
						p2   |= ~*s-- << (16 - shift);
						p3   |= ~*s-- << (16 - shift);

						s += 4;

						*d--  = p0;
						*d--  = p1;
						*d--  = p2;
						*d--  = p3;
					}
				}

				if (ebits)
				{
					p0 = ~*s-- >> shift;
					p1 = ~*s-- >> shift;
					p2 = ~*s-- >> shift;
					p3 = ~*s-- >> shift;
					if (s_shift + ebits > 16)
					{
						p0 |= ~*s-- << (16 - shift);
						p1 |= ~*s-- << (16 - shift);
						p2 |= ~*s-- << (16 - shift);
						p3 |= ~*s-- << (16 - shift);
					}
					*d-- = (p0 & endmask) | (*d & ~endmask);
					*d-- = (p1 & endmask) | (*d & ~endmask);
					*d-- = (p2 & endmask) | (*d & ~endmask);
					*d-- = (p3 & endmask) | (*d & ~endmask);
				}
				src -= sbpl;
				dst -= dbpl;
			}		
		}
		else
		{
			//unsigned short data1;

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
					p0 = ~*s--;
					p1 = ~*s--;
					p2 = ~*s--;
					p3 = ~*s--;
					*d-- = ((p0 << shift) & begmask) | (*d & ~begmask);
					*d-- = ((p1 << shift) & begmask) | (*d & ~begmask);
					*d-- = ((p2 << shift) & begmask) | (*d & ~begmask);
					*d-- = ((p3 << shift) & begmask) | (*d & ~begmask);
				}
				else
					p0 = p1 = p2 = p3 = 0;

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						p0   >>= 16 - shift;
						data   = ~*s--;
						p0    |= data << shift;
						*d--   = p0;
						p0     = data;

						p1   >>= 16 - shift;
						data   = ~*s--;
						p1    |= data << shift;
						*d--   = p1;
						p1     = data;

						p2   >>= 16 - shift;
						data   = ~*s--;
						p2    |= data << shift;
						*d--   = p2;
						p2     = data;

						p3   >>= 16 - shift;
						data   = ~*s--;
						p3    |= data << shift;
						*d--   = p3;
						p3     = data;
					}
				}

				if (ebits)
				{
					p0 >>= 16 - shift;
					p1 >>= 16 - shift;
					p2 >>= 16 - shift;
					p3 >>= 16 - shift;
					if (spans)
					{
						p0 |= ~*s-- << shift;
						p1 |= ~*s-- << shift;
						p2 |= ~*s-- << shift;
						p3 |= ~*s-- << shift;
					}
					*d-- = (p0 & endmask) | (*d & ~endmask);
					*d-- = (p1 & endmask) | (*d & ~endmask);
					*d-- = (p2 & endmask) | (*d & ~endmask);
					*d-- = (p3 & endmask) | (*d & ~endmask);
				}
				src -= sbpl;
				dst -= dbpl;
			}
		}
	}
	else
	{
		//unsigned short data1;

		s_shift	= rpb->sx1 & 0xf;
		d_shift	= rpb->dx1 & 0xf;

		if (!s_shift && !d_shift)
		{
		/* source & destination starts on whole word */

			endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));

			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				for (i = width >> 4; i > 0; i--)
				{
					*d++ = ~*s++;
					*d++ = ~*s++;
					*d++ = ~*s++;
					*d++ = ~*s++;
				}
				if (endmask != 0xffff)
				{
					*d++ = (~*s++ & endmask) | (*d & ~endmask);
					*d++ = (~*s++ & endmask) | (*d & ~endmask);
					*d++ = (~*s++ & endmask) | (*d & ~endmask);
					*d++ = (~*s++ & endmask) | (*d & ~endmask);
				}
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
				{
					*d++ = (~*s++ & begmask) | (*d & ~begmask);
					*d++ = (~*s++ & begmask) | (*d & ~begmask);
					*d++ = (~*s++ & begmask) | (*d & ~begmask);
					*d++ = (~*s++ & begmask) | (*d & ~begmask);
				}
				for (i = groups; i > 0; i--)
				{
					*d++ = ~*s++;
					*d++ = ~*s++;
					*d++ = ~*s++;
					*d++ = ~*s++;
				}
				if (ebits)
				{
					*d++ = (~*s++ & endmask) | (*d & ~endmask);
					*d++ = (~*s++ & endmask) | (*d & ~endmask);
					*d++ = (~*s++ & endmask) | (*d & ~endmask);
					*d++ = (~*s++ & endmask) | (*d & ~endmask);
				}
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
					p0 = ~*s++;
					p1 = ~*s++;
					p2 = ~*s++;
					p3 = ~*s++;
					*d++ = ((p0 >> shift) & begmask) | (*d & ~begmask);
					*d++ = ((p1 >> shift) & begmask) | (*d & ~begmask);
					*d++ = ((p2 >> shift) & begmask) | (*d & ~begmask);
					*d++ = ((p3 >> shift) & begmask) | (*d & ~begmask);
				}
				else
					p0 = p1 = p2 = p3 = 0;

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						p0   <<= 16 - shift;
						data   = ~*s++;
						p0    |= data >> shift;
						*d++   = p0;
						p0     = data;

						p1   <<= 16 - shift;
						data   = ~*s++;
						p1    |= data >> shift;
						*d++   = p1;
						p1     = data;

						p2   <<= 16 - shift;
						data   = ~*s++;
						p2    |= data >> shift;
						*d++   = p2;
						p2     = data;

						p3   <<= 16 - shift;
						data   = ~*s++;
						p3    |= data >> shift;
						*d++   = p3;
						p3     = data;
					}
				}

				if (ebits)
				{
					p0 <<= 16 - shift;
					p1 <<= 16 - shift;
					p2 <<= 16 - shift;
					p3 <<= 16 - shift;
					if (spans)
					{
						p0 |= ~*s++ >> shift;
						p1 |= ~*s++ >> shift;
						p2 |= ~*s++ >> shift;
						p3 |= ~*s++ >> shift;
					}
					*d++ = (p0 & endmask) | (*d & ~endmask);
					*d++ = (p1 & endmask) | (*d & ~endmask);
					*d++ = (p2 & endmask) | (*d & ~endmask);
					*d++ = (p3 & endmask) | (*d & ~endmask);
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
					p0 = ~*s++ << shift;
					p1 = ~*s++ << shift;
					p2 = ~*s++ << shift;
					p3 = ~*s++ << shift;

					if ((s_shift + hbits) > 16)
					{
						p0 |= ~*s >> (16 - shift);
						p1 |= ~*(s+1) >> (16 - shift);
						p2 |= ~*(s+2) >> (16 - shift);
						p3 |= ~*(s+3) >> (16 - shift);
					}
					*d++ = (p0 & begmask) | (*d & ~begmask);
					*d++ = (p1 & begmask) | (*d & ~begmask);
					*d++ = (p2 & begmask) | (*d & ~begmask);
					*d++ = (p3 & begmask) | (*d & ~begmask);
				}
				if (groups)
				{
					for (i = groups; i > 0; i--)
					{

						p0    = ~*s++ << shift;
						p1    = ~*s++ << shift;
						p2    = ~*s++ << shift;
						p3    = ~*s++ << shift;

						p0   |= ~*s++ >> (16 - shift);
						p1   |= ~*s++ >> (16 - shift);
						p2   |= ~*s++ >> (16 - shift);
						p3   |= ~*s++ >> (16 - shift);

						s  -= 4;

						*d++  = p0;
						*d++  = p1;
						*d++  = p2;
						*d++  = p3;
					}
				}
				if (ebits)
				{
					p0 = ~*s++ << shift;
					p1 = ~*s++ << shift;
					p2 = ~*s++ << shift;
					p3 = ~*s++ << shift;

					if ((s_shift + ebits) > 16)
					{
						p0 |= ~*s++ >> (16 - shift);
						p1 |= ~*s++ >> (16 - shift);
						p2 |= ~*s++ >> (16 - shift);
						p3 |= ~*s++ >> (16 - shift);
					}
					*d++ = (p0 & endmask) | (*d & ~endmask);
					*d++ = (p1 & endmask) | (*d & ~endmask);
					*d++ = (p2 & endmask) | (*d & ~endmask);
					*d++ = (p3 & endmask) | (*d & ~endmask);
				}
				src += sbpl;
				dst += dbpl;
			}		
		}
	}
}

void
draw_mousecurs_4b(register XMFORM *mf, register short x, register short y)
{

	register short width, height, xoff, yoff, bypl;
	short	dbpl, w;
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

	xoff	&= 0xf;
	bypl	>>= 1;
	w	= ((x & 0xf) + width + 15) >> 4;
	dbpl	= bypl - (w << 2);

	{
		register unsigned short *src, *dst;
		register short i, j;


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
		register short i, hbits, ebits, groups, sbpl, spans;
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

void
restore_msave_4b(XMSAVE *ms)
{
	register short width, w, height, nl;
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
