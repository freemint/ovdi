#include "mouse.h"
#include "ovdi_mouse.h"
#include "ovdi_defs.h"

#include "16b_generic.h"

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
get_pixel_16b(unsigned char *sb, short bpl, short x, short y)
{
	union { long lng; unsigned short *s; } a;
	a.lng = (long)sb + ((long)y * bpl) + ((long)x << 1);	
	return (*a.s & 0xffffUL);
}

void
put_pixel_16b(unsigned char *sb, short bpl, short x, short y, unsigned long pixel)
{
	union { long lng; unsigned short *s; } a;
	a.lng = (long)sb + ((long)y * bpl) + ((long)x << 1);
	*a.s = pixel;
}
#if 0
/* mono-raster blits */
pixel_blit dpf_16b[] = 
{
	S_ONLY,
	S_ONLY,

	S_ONLY,
	0,

	S_XOR_D,
	S_XOR_D,

	0,
	S_ONLY, /*S_XOR_D,*/

	0,0,0,0,	/* reserved */
	0,0,0,0		/* reserved */
};
#endif
pixel_blit rt_ops_16b[] = 
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
	*addr = 0xffff;
	return;
}
static void
S_AND_D(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	
	*addr = (unsigned short)data & *addr;
	return;
}
static void
S_AND_NOTD(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	
	*addr = (unsigned short)data & ~*addr;
	return;
}
static void
S_ONLY(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr = (unsigned short)data;
	return;
}
static void
NOTS_AND_D(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr &= (unsigned short)~data;
	return;
}
static void
D_ONLY(unsigned char *_addr, long data)
{
	return;
}
static void
S_XOR_D(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr ^= (unsigned short)data;
	return;
}
static void
S_OR_D(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr |= (unsigned short)data;
	return;
}
static void
NOT_SORD(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr = ~((unsigned short)data | *addr);
	return;
}
static void
NOT_SXORD(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr = ~((unsigned short)data ^ *addr);
	return;
}
static void
NOT_D(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr = ~*addr;
	return;
}
static void
S_OR_NOTD(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr = (unsigned short)data | ~*addr;
	return;
}
static void
NOT_S(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;

	*addr = (unsigned short)~data;
	return;
}
static void
NOTS_OR_D(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr = (unsigned short)~data | *addr;
	return;
}
static void
NOT_SANDD(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr = ~((unsigned short)data & *addr);
	return;
}
static void
ALL_BLACK(unsigned char *_addr, long data)
{
	unsigned short *addr = (unsigned short *)_addr;
	*addr = 0;
	return;
}


	
/* *************** RASTER OPERATIONS **************** */
static void rb_ALL_WHITE	(ROP_PB *);
static void rb_S_AND_D		(ROP_PB *);
static void rb_S_AND_NOTD	(ROP_PB *);
static void rb_S_ONLY		(ROP_PB *);
static void rb_NOTS_AND_D	(ROP_PB *);
static void rb_D_ONLY		(ROP_PB *);
static void rb_S_XOR_D		(ROP_PB *);
static void rb_S_OR_D		(ROP_PB *);
static void rb_NOT_SORD		(ROP_PB *);
static void rb_NOT_SXORD	(ROP_PB *);
static void rb_NOT_D		(ROP_PB *);
static void rb_S_OR_NOTD	(ROP_PB *);
static void rb_NOT_S		(ROP_PB *);
static void rb_NOTS_OR_D	(ROP_PB *);
static void rb_NOT_SANDD	(ROP_PB *);
static void rb_ALL_BLACK	(ROP_PB *);

raster_blit rops_16b[] =
{
	rb_ALL_WHITE,
	rb_S_AND_D,
	rb_S_AND_NOTD,
	rb_S_ONLY,
	rb_NOTS_AND_D,
	rb_D_ONLY,
	rb_S_XOR_D,
	rb_S_OR_D,
	rb_NOT_SORD,
	rb_NOT_SXORD,
	rb_NOT_D,
	rb_S_OR_NOTD,
	rb_NOT_S,
	rb_NOTS_OR_D,
	rb_NOT_SANDD,
	rb_ALL_BLACK
};

static void
rb_ALL_WHITE(ROP_PB *rpb)
{
	register int i, width, height, dbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da;
	register unsigned long d = 0xffffffffUL;

	width  = rpb->dx2 - rpb->dx1 + 1;
	height = rpb->dy2 - rpb->dy1 + 1;
	
	da.lng = (long)rpb->d_addr + ((long)rpb->dy2 * rpb->d_bypl) + (((long)rpb->dx2 + 2) << 1);
	dbpl = rpb->d_bypl - (width << 1);
	for (; height > 0; height--)
	{
		for (i = width >> 4; i > 0; i--)
		{
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
		}
		for (i = width & 0xf; i > 0; i--)
			*--da.s = (unsigned short)d;
		da.lng -= dbpl;
	}
}

static void
rb_S_AND_D(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s &= *--sa.s;
			da.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ &= *sa.s++;
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_S_AND_NOTD(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = *--sa.l & ~*da.l;
				*--da.l = *--sa.l & ~*da.l;
				*--da.l = *--sa.l & ~*da.l;
				*--da.l = *--sa.l & ~*da.l;
				*--da.l = *--sa.l & ~*da.l;
				*--da.l = *--sa.l & ~*da.l;
				*--da.l = *--sa.l & ~*da.l;
				*--da.l = *--sa.l & ~*da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s = *--sa.s & ~*da.s;
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = *sa.l++ & ~*da.l;
				*da.l++ = *sa.l++ & ~*da.l;
				*da.l++ = *sa.l++ & ~*da.l;
				*da.l++ = *sa.l++ & ~*da.l;
				*da.l++ = *sa.l++ & ~*da.l;
				*da.l++ = *sa.l++ & ~*da.l;
				*da.l++ = *sa.l++ & ~*da.l;
				*da.l++ = *sa.l++ & ~*da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ = *sa.s++ & ~*da.s;
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_S_ONLY(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s = *--sa.s;
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ = *sa.s++;
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_NOTS_AND_D(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s &= ~*--sa.s;
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ &= ~*sa.s++;
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_D_ONLY(ROP_PB *rpb)
{
	return;
}
static void
rb_S_XOR_D(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s ^= *--sa.s;
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ ^= *sa.s++;
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_S_OR_D(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s |= *--sa.s;
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ |= *sa.s++;
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_NOT_SORD(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s = ~(*--sa.s | *da.s);
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ = ~(*sa.s++ | *da.s);
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_NOT_SXORD(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s = ~(*--sa.s ^ *da.s);
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ = ~(*sa.s++ ^ *da.s);
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_NOT_D(ROP_PB *rpb)
{
	register int i, width, height, dbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da;
	register unsigned long d;

	width  = rpb->dx2 - rpb->dx1 + 1;
	height = rpb->dy2 - rpb->dy1 + 1;
	
	da.lng = (long)rpb->d_addr + ((long)rpb->dy2 * rpb->d_bypl) + (((long)rpb->dx2 + 2) << 1);
	dbpl = rpb->d_bypl - (width << 1);
	for (; height > 0; height--)
	{
		for (i = width >> 4; i > 0; i--)
		{
			d = *--da.l; *da.l = ~d;
			d = *--da.l; *da.l = ~d;
			d = *--da.l; *da.l = ~d;
			d = *--da.l; *da.l = ~d;
			d = *--da.l; *da.l = ~d;
			d = *--da.l; *da.l = ~d;
			d = *--da.l; *da.l = ~d;
			d = *--da.l; *da.l = ~d;
		}
		for (i = width & 0xf; i > 0; i--)
			d = *--da.s, *da.s = (unsigned short)d;
		da.lng -= dbpl;
	}
}
static void
rb_S_OR_NOTD(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s = *--sa.s | ~*da.s;
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ = *sa.s++ | ~*da.s;
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_NOT_S(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s = ~*--sa.s;
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ = ~*sa.s++;
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_NOTS_OR_D(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s = ~*--sa.s | *da.s;
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ = ~*sa.s++ | *da.s;
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_NOT_SANDD(ROP_PB *rpb)
{
	register int i, width, height, dbpl, sbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da, sa;

	width  = rpb->sx2 - rpb->sx1 + 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	
	sa.lng = (long)rpb->d_addr + ((long)rpb->sy1 * rpb->s_bypl) + ((long)rpb->sx1 << 1);
	da.lng = (long)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + ((long)rpb->dx1 << 1);

	dbpl = rpb->d_bypl - (width << 1);
	sbpl = rpb->s_bypl - (width << 1);
	
	if (sa.lng < da.lng)
	{
		sa.lng += (long)(height - 1) * rpb->s_bypl;
		sa.lng += (long)width << 1;
		da.lng += (long)(height - 1) * rpb->d_bypl;
		da.lng += (long)width << 1;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.s = ~(*--sa.s & *da.s);
			sa.lng -= sbpl;
			da.lng -= dbpl;
		}
	}
	else
	{
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*da.s++ = ~(*sa.s++ & *da.s);
			sa.lng += sbpl;
			da.lng += dbpl;
		}
	}
}
static void
rb_ALL_BLACK(ROP_PB *rpb)
{
	register int i, width, height, dbpl;
	register union { unsigned short *s; unsigned long *l; long lng; } da;
	register unsigned long d = 0UL;

	width  = rpb->dx2 - rpb->dx1 + 1;
	height = rpb->dy2 - rpb->dy1 + 1;
	
	da.lng = (long)rpb->d_addr + ((long)rpb->dy2 * rpb->d_bypl) + (((long)rpb->dx2 + 2) << 1);
	dbpl = rpb->d_bypl - (width << 1);
	for (; height > 0; height--)
	{
		for (i = width >> 4; i > 0; i--)
		{
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
		}
		for (i = width & 0xf; i > 0; i--)
			*--da.s = d;
		da.lng -= dbpl;
	}
}

void
draw_mousecurs_16b(register XMFORM *mf, register short x, register short y)
{

	register short width, height, xoff, yoff, bypl;
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

	{
		register unsigned short *src, *dst;
		register int i, j, nl;

		dst = (unsigned short *)ms->save;
		src = (unsigned short *)((long)mf->scr_base + (long)(((long)y * bypl) + ((long)x << 1)));

		ms->width = width;
		ms->height = height;
		ms->bypl = bypl;
		ms->valid = 1;
		ms->src = (unsigned char *)src;

		nl = (bypl - (width << 1)) >> 1;

		for (i = height; i > 0; i--)
		{

			for (j = width; j > 0; j--)
				*dst++ = *src++;

			src += nl;
		}
	}

	if (mf->planes == 1)
	{
		register int i, j, nl, mnl, w;
		register unsigned long fgc, bgc;
		register unsigned short mask, data, ovl;
		register unsigned short *ma;
		register unsigned short *dst;


		nl = (bypl - (width << 1)) >> 1;
		dst = (unsigned short *)((long)mf->scr_base + (long)(((long)y * bypl) + ((long)x << 1)));
		ma = (unsigned short *)mf->data + (long)(yoff * mf->mfbypl) + (long)((xoff >> 4) << 1);
		xoff &= 0xf;
		mnl = ((mf->width - (xoff + width)) >> 4) << 1;
		fgc = mf->fg_pix;
		bgc = mf->bg_pix;

		for (i = height; i > 0; i--)
		{
			mask = *ma++;
			data = *ma++;
			ovl = 0x8000 >> xoff;
			w = width;

			if ( (16 - xoff) < width)
			{
				for (j = 16 - xoff; j > 0; j--)
				{
					if (mask & ovl)
					{
						if (data & ovl)
							*dst = (unsigned short)fgc;
						else
							*dst = (unsigned short)bgc;
					}
					dst++;
					ovl >>= 1;
				}
				mask = *ma++;
				data = *ma++;
				ovl = 0x8000;
				w -= (16 - xoff);
			}

			while (w > 16)
			{
				for (j = 16; j > 0; j--)
				{
					if (mask & ovl)
					{
						if (data & ovl)
							*dst = (unsigned short)fgc;
						else
							*dst = (unsigned short)bgc;
					}
					dst++;
					ovl >>= 1;
				}
				mask = *ma++;
				data = *ma++;
				ovl = 0x8000;
				w -= 16;
			}

			for (j = w; j > 0; j--)
			{
				if (mask & ovl)
				{
					if (data & ovl)
						*dst = (unsigned short)fgc;
					else
						*dst = (unsigned short)bgc;
				}
				dst++;
				ovl >>= 1;
			}
			dst += nl;
			ma += mnl;
		}
	}
	else
	{
		register int i, j, dnl, snl, mnl, w;
		register unsigned short mask, ovl;
		register unsigned short *ma;
		register unsigned short *src, *dst;


		dnl = (bypl - (width << 1)) >> 1;
		snl = mf->mfbypl;
		dst = (unsigned short *)((long)mf->scr_base + (long)(((long)y * bypl) + ((long)x << 1)));
		src = (unsigned short *)((long)mf->data + (long)(((long)yoff * snl) + ((long)xoff << 1)));
		snl -= (width << 1);
		snl >>= 1;
		ma = (unsigned short *)mf->mask + (long)(yoff * ((mf->width >> 4) +1)) + (long)(xoff >> 4);
		xoff &= 0xf;
		mnl = (mf->width - (xoff + width)) >> 4;

		for (i = height; i > 0; i--)
		{
			mask = *ma++;
			ovl = 0x8000 >> xoff;
			w = width;

			if ( (16 - xoff) < width)
			{
				for (j = 16 - xoff; j > 0; j--)
				{
					if (mask & ovl)
						*dst = *src;
					dst++;
					src++;
					ovl >>= 1;
				}
				mask = *ma++;
				ovl = 0x8000;
				w -= (16 - xoff);
			}

			while (w > 16)
			{
				for (j = 16; j > 0; j--)
				{
					if (mask & ovl)
						*dst = *src;
					dst++;
					src++;
					ovl >>= 1;
				}
				mask = *ma++;
				ovl = 0x8000;
				w -= 16;
			}

			for (j = w; j > 0; j--)
			{
				if (mask & ovl)
					*dst = *src;
				dst++;
				src++;
				ovl >>= 1;
			}
			dst += dnl;
			src += snl;
			ma += mnl;
		}
	}	

	//display("x %d, y %d, width %d, height %d, xoff %d, yoff %d\n", x, y, width, height, xoff, yoff);

	return;
}

void
restore_msave_16b(XMSAVE *ms)
{
	register int width, w, height, nl;
	register unsigned short *src, *dst;

	if (ms->valid)
	{
		src = (unsigned short *)ms->save;
		dst = (unsigned short *)ms->src;
		width = ms->width;
		nl = (ms->bypl - (width << 1)) >> 1;
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
