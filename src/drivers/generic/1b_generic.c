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

static void
rb_S_ONLY_new(ROP_PB *rpb)
{
	unsigned short begmask, endmask, p0;
	int shift, swords, dwords, sbpl, dbpl, height;
	unsigned short *s, *d;

	sbpl = (rpb->s_bypl >> 1);
	dbpl = (rpb->d_bypl >> 1);
	height = rpb->sy2 - rpb->sy1 + 1;
	s = (unsigned short *)rpb->s_addr + (long)(rpb->sx1 >> 4) + (long)((long)rpb->sy1 * sbpl);
	d = (unsigned short *)rpb->d_addr + (long)(rpb->dx1 >> 4) + (long)((long)rpb->dy1 * dbpl);
	swords	= (((rpb->sx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;
	dwords	= (((rpb->dx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;

	if (s < d)
	{
		s = (unsigned short *)rpb->s_addr + (long)(rpb->sx2 >> 4) + (long)((long)rpb->sy2 * sbpl);
		d = (unsigned short *)rpb->d_addr + (long)(rpb->dx2 >> 4) + (long)((long)rpb->dy2 * dbpl);
		sbpl -= swords + 1;
		dbpl -= dwords + 1;
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
							*d-- = *s--;
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							for (swords = dwords + 1; swords > 0; swords--)
								*d-- = *s--;
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
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							for (swords = dwords; swords > 0; swords--)
								*d-- = *s--;
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
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							for (swords = dwords; swords > 0; swords--)
								*d-- = *s--;
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
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d-- = (*d & ~begmask) | (*s-- & begmask);
							for (swords = dwords - 1; swords > 0; swords--)
								*d-- = *s--;
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
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | ((p0 << shift) & begmask);
						for (dwords = swords; dwords > 1; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = (p0 >> rshift) | (tmp << shift);
							p0 = tmp;
						}
						*d-- = (*d & ~endmask) | ( ((p0 >> rshift) | (*s-- << shift)) & endmask);
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
						*d-- = (*d & ~endmask) | ((p0 >> rshift) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | ((p0 << shift) & begmask);
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = (p0 >> rshift) | (tmp << shift);
							p0 = tmp;
						}
						*d-- = (*d & ~endmask) | ((p0 >> rshift) & endmask);
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
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | ( ((p0 >> shift) | (*s << rshift)) & begmask);
						p0 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = (p0 >> shift) | (tmp << rshift);
							p0 = tmp;
						}
						*d-- = (*d & ~endmask) | ((p0 >> shift) & endmask);
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
						p0 = *s--;
						*d-- = (*d & ~begmask) | (((p0 >> shift) | (*s-- << rshift)) & begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | (((p0 >> shift) | (*s << rshift)) & begmask);
						p0 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = (p0 >> shift) | (tmp << rshift);
							p0 = tmp;
						}
						*d-- = (*d & ~endmask) | (((p0 >> shift) | (*s-- << rshift)) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
		}			
	}
	else // endif (s < d)
	{
		sbpl -= swords + 1;
		dbpl -= dwords + 1;
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
							*d++ = *s++;
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (;height > 0; height--)
					{
						for (i = swords; i > 0; i--)
							*d++ = *s++;
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
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							for (i = swords; i > 0; i--)
								*d++ = *s++;
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
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (*s++ & begmask);
							for (i = swords - 1; i > 0; i--)
								*d++ = *s++;
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
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | ((p0 >> shift) & begmask);
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = (p0 << rshift) | (tmp >> shift);
							p0 = tmp;
						}
						*d++ = (*d & ~endmask) | (((p0 << rshift) | (*s++ >> shift)) & endmask);
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
						*d++ = (*d & ~endmask) | ((p0 << rshift) & endmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | ((p0 >> shift) & begmask);	
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = (p0 << rshift) | (tmp >> shift);
							p0 = tmp;
						}
						*d++ = (*d & ~endmask) | ((p0 << rshift) & endmask);
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
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | (((p0 << shift) | (*s >> rshift)) & begmask);
						p0 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = (p0 << shift) | (tmp >> rshift);
							p0 = tmp;
						}
						*d++ = (*d & ~endmask) | ((p0 << shift) & endmask);
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
						*d++ = (*d & ~begmask) | (((p0 << shift) | (*s++ >> rshift)) & begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | (((p0 << shift) | (*s >> rshift)) & begmask);
						p0 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							*d++ = (p0 << shift) | (*s >> rshift);
							p0 = *s++;
						}
						*d++ = (*d & ~endmask) | (((p0 << shift) | (*s++ >> rshift)) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
		}
	}
}

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

#if 0
void
draw_solid_rect_1b(RASTER *r, COLINF *c, short *corners, short wrmode, short color)
{
	unsigned short *dst, *d;
	short i, bypl, dx, dy, sb, hb, tb, g;
	unsigned short pixel, sm, em;

	bypl = r->bypl >> 1;
	pixel = color & 1 ? 0xffff : 0x0; 
	dx = corners[2] - corners[0] + 1;
	dy = corners[3] - corners[1] + 1;

	sb	= corners[0] & 0xf;
	sm	= 0xffff >> sb;
	em	= ~(0xffff >> ((corners[2] + 1) & 0xf));
	hb	= (16 - sb) & 0xf;
	dx	-= hb;

	if (dx <= 0)
	{
		hb = dx + hb;
		g = tb = 0;
		if (dx != 0)
			sm &= em;
	}
	else if (dx > 15)
	{
		tb = (corners[2] + 1) & 0xf;
		g = (dx - tb) >> 4;
	}
	else
	{
		tb = dx;
		g = 0;
	}

	dst = (unsigned short *)r->base + (long)(corners[0]>>4) + (long)((long)corners[1] * bypl);

	SYNC_RASTER(r);

	switch (wrmode)
	{
		case 0: /* MD_REPLACE */
		case 1: /* MD_TRANS */
		{
			for (; dy > 0; dy--)
			{
				d = dst;

				if (hb)
					*d++ = (*d & ~sm) | (pixel & sm);
				if (g)
				{
					for ( i = g; i > 0; i--)
						*d++ = pixel;
				}
				if (tb)
					*d++ = (*d & ~em) | (pixel & em);
				dst += bypl;
			}
			break;
		}
		case 2: /* MD_EOR */
		{
			for (; dy > 0; dy--)
			{
				d = dst;

				if (hb)
					*d++ = (*d & ~sm) | ((*d ^ pixel) & sm);
				if (g)
				{
					for ( i = g; i > 0; i--)
						*d++ ^= pixel;
				}
				if (tb)
					*d++ = (*d & ~em) | ((*d ^ pixel) & em);

				dst += bypl;
			}
			break;
		}
		case 3:
		{
			pixel = ~pixel;
			for (; dy > 0; dy--)
			{
				d = dst;

				if (hb)
					*d++ = (*d & ~sm) | ((*d ^ pixel) & sm);
				if (g)
				{
					for ( i = g; i > 0; i--)
						*d++ ^= pixel;
				}
				if (tb)
					*d++ = (*d & ~em) | ((*d ^ pixel) & em);

				dst += bypl;
			}
			break;
		}
	}
}
#endif
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
static void rb_NOT_S		(ROP_PB *);
//static void rb_NOTS_OR_D	(ROP_PB *);
//static void rb_NOT_SANDD	(ROP_PB *);
//static void rb_ALL_BLACK	(ROP_PB *);

raster_blit rops_1b[] =
{
	0,	/* rb_ALL_WHITE,*/
	0,	/* rb_S_AND_D,*/
	0,	/* rb_S_AND_NOTD,*/
	rb_S_ONLY_new,
	0,	/* rb_NOTS_AND_D,*/
	0,	/* rb_D_ONLY,*/
	rb_S_XOR_D,
	rb_S_OR_D,
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
/**/
static void
rb_S_ONLY(ROP_PB *rpb)
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

	//if (src < dst)
	if (src >= dst)
		rb_S_ONLY_new(rpb);
	else
	{
		src = (unsigned short *)rpb->s_addr + (long)(rpb->sx2 >> 4) + (long)((long)rpb->sy2 * sbpl);
		dst = (unsigned short *)rpb->d_addr + (long)(rpb->dx2 >> 4) + (long)((long)rpb->dy2 * dbpl);

		s_shift	= 15 - (rpb->sx2 & 0xf);
		d_shift	= 15 - (rpb->dx2 & 0xf);

		if (!s_shift && !d_shift)
		{
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
#if 0
	else
	{
		unsigned short data1;

		s_shift	= rpb->sx1 & 0xf;
		d_shift	= rpb->dx1 & 0xf;

		if (!s_shift && !d_shift)
		{
		/* source & destination starts on whole word */

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
#endif
}

static void
rb_S_XOR_D(ROP_PB *rpb)
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
		/* source & destination starts on whole word */

			endmask = 0xffff >> (rpb->sx1 & 0xf);

			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				for (i = width >> 4; i > 0; i--)
					*d-- ^= *s--;
				if (endmask != 0xffff)
					*d-- = ((*d ^ *s--) & endmask) | (*d & ~endmask);
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
					*d-- = ((*d ^ *s--) & begmask) | (*d & ~begmask);
				for (i = groups; i > 0; i--)
					*d-- ^= *s--;
				if (ebits)
					*d-- = ((*d ^ *s--) & endmask) | (*d & ~endmask);

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
					if ((s_shift + hbits) > 16)
						data |= *s << (16 - shift);
					*d-- = ((*d ^ data) & begmask) | (*d & ~ begmask);

				}

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data = *s-- >> shift;
						data |= *s << (16 - shift);
						*d-- ^= data;
					}
				}

				if (ebits)
				{
					data = *s-- >> shift;
					if ((s_shift + ebits) > 16)
						data |= *s << (16 - shift);
					*d-- = ((*d ^ data) & endmask) | (*d & ~endmask);
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
					*d-- = ((*d ^ (data << shift)) & begmask) | (*d & ~begmask);
				}
				else
					data = 0;

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data >>= 16 - shift;
						data1 = *s--;
						data |= data1 << shift;
						*d-- ^= data;
						data = data1;
					}
				}

				if (ebits)
				{
					data >>= 16 - shift;
					if (spans)
						data |= *s-- << shift;
					*d-- = ((*d ^ data) & endmask) | (*d & ~endmask);
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

			endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));

			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				for (i = width >> 4; i > 0; i--)
					*d++ ^= *s++;
				if (endmask != 0xffff)
					*d++ = ((*d ^ *s++) & endmask) | (*d & ~endmask);
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
					*d++ = ((*d ^ *s++) & begmask) | (*d & ~begmask);
				for (i = groups; i > 0; i--)
					*d++ ^= *s++;
				if (ebits)
					*d++ = ((*d ^ *s++) & endmask) | (*d & ~endmask);

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
					*d++ = ((*d ^ (data >> shift)) & begmask) | (*d & ~begmask);
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
						*d++ ^= data;
						data = data1;
					}
				}

				if (ebits)
				{
					data <<= 16 - shift;
					if (spans)
						data |= *s++ >> shift;
					*d++ = ((*d ^ data) & endmask) | (*d & ~endmask);
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

			for (; height > 0; height--)
			{
				s = src;
				d = dst;

				if (hbits)
				{
					data = *s++ << shift;

					if ((s_shift + hbits) > 16)
						data |= *s >> (16 - shift);

					*d++ = ((*d ^ data) & begmask) | (*d & ~begmask);
				}
				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data = *s++ << shift;
						data |= *s >> (16 - shift);
						*d++ ^= data;
					}
				}
				if (ebits)
				{
					data = *s++ << shift;

					if ((s_shift + ebits) > 16)
						data |= *s >> (16 - shift);

					*d++ = ((*d ^ data) & endmask) | (*d & ~endmask);
				}

				src += sbpl;
				dst += dbpl;
			}		
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
		/* source & destination starts on whole word */

			endmask = 0xffff >> (rpb->sx1 & 0xf);

			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				for (i = width >> 4; i > 0; i--)
					*d-- |= *s--;
				if (endmask != 0xffff)
					*d-- = ((*d | *s--) & endmask) | (*d & ~endmask);
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
					*d-- = ((*d | *s--) & begmask) | (*d & ~begmask);
				for (i = groups; i > 0; i--)
					*d-- |= *s--;
				if (ebits)
					*d-- = ((*d | *s--) & endmask) | (*d & ~endmask);

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
					if ((s_shift + hbits) > 16)
						data |= *s << (16 - shift);
					*d-- = ((*d | data) & begmask) | (*d & ~ begmask);

				}

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data = *s-- >> shift;
						data |= *s << (16 - shift);
						*d-- |= data;
					}
				}

				if (ebits)
				{
					data = *s-- >> shift;
					if ((s_shift + ebits) > 16)
						data |= *s << (16 - shift);
					*d-- = ((*d | data) & endmask) | (*d & ~endmask);
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
					*d-- = ((*d | (data << shift)) & begmask) | (*d & ~begmask);
				}
				else
					data = 0;

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data >>= 16 - shift;
						data1 = *s--;
						data |= data1 << shift;
						*d-- |= data;
						data = data1;
					}
				}

				if (ebits)
				{
					data >>= 16 - shift;
					if (spans)
						data |= *s-- << shift;
					*d-- = ((*d | data) & endmask) | (*d & ~endmask);
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

			endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));

			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				for (i = width >> 4; i > 0; i--)
					*d++ |= *s++;
				if (endmask != 0xffff)
					*d++ = ((*d | *s++) & endmask) | (*d & ~endmask);
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
					*d++ = ((*d | *s++) & begmask) | (*d & ~begmask);
				for (i = groups; i > 0; i--)
					*d++ |= *s++;
				if (ebits)
					*d++ = ((*d | *s++) & endmask) | (*d & ~endmask);

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
					*d++ = ((*d | (data >> shift)) & begmask) | (*d & ~begmask);
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
						*d++ |= data;
						data = data1;
					}
				}

				if (ebits)
				{
					data <<= 16 - shift;
					if (spans)
						data |= *s++ >> shift;
					*d++ = ((*d | data) & endmask) | (*d & ~endmask);
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

			for (; height > 0; height--)
			{
				s = src;
				d = dst;

				if (hbits)
				{
					data = *s++ << shift;

					if ((s_shift + hbits) > 16)
						data |= *s >> (16 - shift); //spans;

					*d++ = ((*d | data) & begmask) | (*d & ~begmask);
				}
				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data = *s++ << shift;
						data |= *s >> (16 - shift);
						*d++ |= data;
					}
				}
				if (ebits)
				{
					data = *s++ << shift;

					if ((s_shift + ebits) > 16)
						data |= *s >> (16 - shift);

					*d++ = ((*d | data) & endmask) | (*d & ~endmask);
				}

				src += sbpl;
				dst += dbpl;
			}		
		}
	}
}

static void
rb_NOT_S(ROP_PB *rpb)
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
		/* source & destination starts on whole word */

			endmask = 0xffff >> (rpb->sx1 & 0xf);

			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				for (i = width >> 4; i > 0; i--)
					*d-- = ~*s--;
				if (endmask != 0xffff)
					*d-- = (~*s-- & endmask) | (*d & ~endmask);
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
					*d-- = (~*s-- & begmask) | (*d & ~begmask);
				for (i = groups; i > 0; i--)
					*d-- = ~*s--;
				if (ebits)
					*d-- = (~*s-- & endmask) | (*d & ~endmask);

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
					data = ~*s-- >> shift;
					if ((s_shift + hbits) > 16)
						data |= ~*s << (16 - shift);
					*d-- = (data & begmask) | (*d & ~ begmask);

				}

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data = ~*s-- >> shift;
						data |= ~*s << (16 - shift);
						*d-- = data;
					}
				}

				if (ebits)
				{
					data = ~*s-- >> shift;
					if (s_shift + ebits > 16)
						data |= ~*s << (16 - shift);
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
					data = ~*s--;
					*d-- = ((data << shift) & begmask) | (*d & ~begmask);
				}
				else
					data = 0;

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data >>= 16 - shift;
						data1 = ~*s--;
						data |= data1 << shift; //(*s << shift);
						*d-- = data;
						data = data1;
					}
				}

				if (ebits)
				{
					data >>= 16 - shift;
					if (spans)
						data |= ~*s-- << shift;
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

			endmask = ~(0xffff >> ((rpb->sx2 + 1) & 0xf));

			for (; height > 0; height--)
			{
				s = src;
				d = dst;
				for (i = width >> 4; i > 0; i--)
					*d++ = ~*s++;
				if (endmask != 0xffff)
					*d++ = (~*s++ & endmask) | (*d & ~endmask);
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
					*d++ = (~*s++ & begmask) | (*d & ~begmask);
				for (i = groups; i > 0; i--)
					*d++ = ~*s++;
				if (ebits)
					*d++ = (~*s++ & endmask) | (*d & ~endmask);

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
					data = ~*s++;
					*d++ = ((data >> shift) & begmask) | (*d & ~begmask);
				}
				else
					data = 0;

				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data <<= 16 - shift;
						data1 = ~*s++;
						data |= data1 >> shift;
						*d++ = data;
						data = data1;
					}
				}

				if (ebits)
				{
					data <<= 16 - shift;
					if (spans)
						data |= ~*s++ >> shift;
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
					data = ~*s++ << shift;

					if ((s_shift + hbits) > 16)
						data |= ~*s >> (16 - shift);

					*d++ = (data & begmask) | (*d & ~begmask);
				}
				if (groups)
				{
					for (i = groups; i > 0; i--)
					{
						data = ~*s++ << shift;
						data |= ~*s >> (16 - shift);
						*d++ = data;
					}
				}
				if (ebits)
				{
					data = ~*s++ << shift;

					if ((s_shift + ebits) > 16)
						data |= ~*s >> (16 - shift);

					*d++ = (data & endmask) | (*d & ~endmask);

				}

				src += sbpl;
				dst += dbpl;
			}		
		}
	}
}

void
draw_mousecurs_1b(register XMFORM *mf, register short x, register short y)
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
	dbpl	= bypl - w;

	{
		register unsigned short *src, *dst;
		register short i, j;


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
		register short i, hbits, ebits, groups, sbpl, spans;
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
	register short width, w, height, nl;
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
