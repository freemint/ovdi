/* This file contains the built in 8 bit per planes support */

#include "display.h"
#include "mouse.h"
#include "mousedrv.h"
#include "ovdi_defs.h"

#include "8b_generic.h"

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

unsigned long _cdecl
get_pixel_8b(unsigned char *sb, short bpl, short x, short y)
{
	return ((*(unsigned char *)(sb + (long)(x + ((long)y * bpl)))) & 0xff);
}

void _cdecl
put_pixel_8b(unsigned char *sb, short bpl, short x, short y, unsigned long pixel)
{
	*(sb + (long)(x + ((long)y * bpl))) = (unsigned char)pixel;
}

#if 0
pixel_blit dpf_8b[] = 
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
#endif

pixel_blit rt_ops_8b[] = 
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
ALL_WHITE(unsigned char *addr, long data)
{
	*addr = 0;
	return;
}
static void _cdecl
S_AND_D(unsigned char *addr, long data)
{
	*addr = (unsigned char)data & *addr;
	return;
}
static void _cdecl
S_AND_NOTD(unsigned char *addr, long data)
{
	*addr = (unsigned char)data & ~(*addr);
	return;
}
static void _cdecl
S_ONLY(unsigned char *addr, long data)
{
	*addr = (unsigned char)data;
	return;
}
static void _cdecl
NOTS_AND_D(unsigned char *addr, long data)
{
	*addr &= (unsigned char)~data;
	return;
}
static void _cdecl
D_ONLY(unsigned char *addr, long data)
{
	return;
}
static void _cdecl
S_XOR_D(unsigned char *addr, long data)
{
	*addr ^= (unsigned char)data;
	return;
}
static void _cdecl
S_OR_D(unsigned char *addr, long data)
{
	*addr |= (unsigned char)data;
	return;
}
static void _cdecl
NOT_SORD(unsigned char *addr, long data)
{
	*addr = ~((unsigned char)data | *addr);
	return;
}
static void _cdecl
NOT_SXORD(unsigned char *addr, long data)
{
	*addr = ~((unsigned char)data ^ *addr);
	return;
}
static void _cdecl
NOT_D(unsigned char *addr, long data)
{
	*addr = ~*addr;
	return;
}
static void _cdecl
S_OR_NOTD(unsigned char *addr, long data)
{
	*addr = (unsigned char)data | ~*addr;
	return;
}
static void _cdecl
NOT_S(unsigned char *addr, long data)
{
	*addr = (unsigned char)~data;
	return;
}
static void _cdecl
NOTS_OR_D(unsigned char *addr, long data)
{
	*addr = (unsigned char)~data | *addr;
	return;
}
static void _cdecl
NOT_SANDD(unsigned char *addr, long data)
{
	*addr = ~((unsigned char)data & *addr);
	return;
}
static void _cdecl
ALL_BLACK(unsigned char *addr, long data)
{
	*addr = 0xff;
	return;
}

//extern void asm_rb_S_ONLY_08(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
/* *************** RASTER OPERATIONS **************** */
static void _cdecl rb_ALL_WHITE	(ROP_PB *);
static void _cdecl rb_S_AND_D		(ROP_PB *);
static void _cdecl rb_S_AND_NOTD	(ROP_PB *);
static void _cdecl rb_S_ONLY		(ROP_PB *);
static void _cdecl rb_NOTS_AND_D	(ROP_PB *);
static void _cdecl rb_D_ONLY		(ROP_PB *);
static void _cdecl rb_S_XOR_D		(ROP_PB *);
static void _cdecl rb_S_OR_D		(ROP_PB *);
static void _cdecl rb_NOT_SORD		(ROP_PB *);
static void _cdecl rb_NOT_SXORD	(ROP_PB *);
static void _cdecl rb_NOT_D		(ROP_PB *);
static void _cdecl rb_S_OR_NOTD	(ROP_PB *);
static void _cdecl rb_NOT_S		(ROP_PB *);
static void _cdecl rb_NOTS_OR_D	(ROP_PB *);
static void _cdecl rb_NOT_SANDD	(ROP_PB *);
static void _cdecl rb_ALL_BLACK	(ROP_PB *);

raster_blit rops_8b[] =
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

static void _cdecl
rb_ALL_WHITE(ROP_PB *rpb)
{
	int i;
	register long width, height, dbpl;
	union { unsigned char *c; unsigned long *l; unsigned long lng;} da;
	
	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	da.c = (char *)rpb->d_addr + ((long)rpb->dy2 * rpb->d_bypl) + rpb->dx2 + 1;

	dbpl = rpb->d_bypl - width;
	
	if (da.lng & 1)
	{
		short w;
		width -= 1;
		w = width & 0xf;
		width >>= 4;
		for (; height > 0; height--)
		{
			*--da.c = 0;
			for (i = width; i > 0; i--)
			{
				*--da.l = 0L;
				*--da.l = 0L;
				*--da.l = 0L;
				*--da.l = 0L;
			}
			for (i = w; i > 0; i--)
				*--da.c = 0;
			da.c -= dbpl;
		}
	}
	else
	{
		short w;
		w = width & 0xf;
		width >>= 4;
		for (; height > 0; height--)
		{
			for (i = width; i > 0; i--)
			{
				*--da.l = 0L;
				*--da.l = 0L;
				*--da.l = 0L;
				*--da.l = 0L;
			}
			for (i = w; w > 0; i--)
				*--da.c = 0;
			da.c -= dbpl;
		}
	}
}

static void _cdecl
rb_S_AND_D(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
				*--da.l &= *--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c &= *--sa.c;
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
				*da.l++ &= *sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ &= *sa.c++;

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_S_AND_NOTD(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				register unsigned long d;
			
				d = ~*--da.l;
				d &= *--sa.l;
				*da.l = d;
				d = ~*--da.l;
				d &= *--sa.l;
				*da.l = d;
				d = ~*--da.l;
				d &= *--sa.l;
				*da.l = d;
				d = ~*--da.l;
				d &= *--sa.l;
				*da.l = d;
			}
			for (i = width & 0xf; i > 0; i--)
			{
				register unsigned char d;
				d  = ~*--da.c;
				d &=  *--sa.c;
				*da.c = d;
			}
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				register unsigned long d;
				d = ~*da.l;
				d &= *sa.l++;
				*da.l++ = d;
				d = ~*da.l;
				d &= *sa.l++;
				*da.l++ = d;
				d = ~*da.l;
				d &= *sa.l++;
				*da.l++ = d;
				d = ~*da.l;
				d &= *sa.l++;
				*da.l++ = d;
			}
			for (i = width & 0xf; i > 0; i--)
			{
				register unsigned char d;
				d = ~*da.c;
				d &= *sa.c++;
				*da.c++ = d;
			}
			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */
/* ************************************************************************** */
static void _cdecl
rb_S_ONLY(ROP_PB *rpb)
{
	register int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
	
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
				*--da.l = *--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c = *--sa.c;
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
				*da.l++ = *sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ = *sa.c++;
			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}

static void _cdecl
rb_NOTS_AND_D(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
				*--da.l &= ~*--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c &= ~*--sa.c;
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
				*da.l++ &= ~*sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ &= ~*sa.c++;

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_D_ONLY(ROP_PB *rpb)
{
	return;
}
static void _cdecl
rb_S_XOR_D(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
				*--da.l ^= *--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c ^= *--sa.c;
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
				*da.l++ ^= *sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ ^= *sa.c++;

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_S_OR_D(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
				*--da.l |= *--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c |= *--sa.c;
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
				*da.l++ |= *sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ |= *sa.c++;

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_NOT_SORD(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{				
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
				*--da.l = ~(*--sa.l | *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c = ~(*--sa.c | *da.c);
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
				*da.l++ = ~(*sa.l++ | *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ = ~(*sa.c++ | *da.c);

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_NOT_SXORD(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
				*--da.l = ~(*--sa.l ^ *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c = ~(*--sa.c ^ *da.c);
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
				*da.l++ = ~(*sa.l++ ^ *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ = ~(*sa.c++ ^ *da.c);

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_NOT_D(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, dbpl;
	register union { unsigned char *c; unsigned long *l; } da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	da.c = (char *)rpb->d_addr + ((long)rpb->sy1 * rpb->d_bypl) + rpb->sx1;
	
	dbpl = rpb->d_bypl;

	da.c += width;
	da.c += (height - 1) * dbpl;
	
	dbpl -= width;
	
	for (; height > 0; height--)
	{
		for (i = width >> 4; i > 0; i--)
		{
			*--da.l = ~*da.l;
			*--da.l = ~*da.l;
			*--da.l = ~*da.l;
			*--da.l = ~*da.l;
		}
		for (i = width & 0xf; i > 0; i--)
			*--da.c = ~*da.c;
		
		da.c -= dbpl;
	}
}
static void _cdecl
rb_S_OR_NOTD(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
				*--da.l = *--sa.l | ~*da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c = *--sa.c | ~*da.c;
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
				*da.l++ = *sa.l++ | ~*da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ = *sa.c++ | ~*da.c;

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_NOT_S(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
				*--da.l = ~*--sa.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c = ~*--sa.c;
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
				*da.l++ = ~*sa.l++;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ = ~*sa.c++;

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_NOTS_OR_D(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
				*--da.l = ~*--sa.l | *da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c = ~*--sa.c | *da.c;
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
				*da.l++ = ~*sa.l++ | *da.l;
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ = ~*sa.c++ | *da.c;

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_NOT_SANDD(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, sbpl, dbpl;
	register union { unsigned char *c; unsigned long *l; } sa, da;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	sa.c = (char *)rpb->s_addr + ((long)rpb->sy1 * rpb->s_bypl) + rpb->sx1;
	da.c = (char *)rpb->d_addr + ((long)rpb->dy1 * rpb->d_bypl) + rpb->dx1;
	
	sbpl = rpb->s_bypl;
	dbpl = rpb->d_bypl;

	if (sa.c < da.c)
	{
		sa.c += width;
		sa.c += (height - 1) * sbpl;
		da.c += width;
		da.c += (height - 1) * dbpl;
		
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
				*--da.l = ~(*--sa.l & *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*--da.c = ~(*--sa.c & *da.c);
			
			sa.c -= sbpl;
			da.c -= dbpl;
		}
	}
	else
	{
		sbpl -= width;
		dbpl -= width;
		
		for (; height > 0; height--)
		{
			for (i = width >> 4; i > 0; i--)
			{
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
				*da.l++ = ~(*sa.l++ & *da.l);
			}
			for (i = width & 0xf; i > 0; i--)
				*da.c++ = ~(*sa.c++ & *da.c);

			sa.c += sbpl;
			da.c += dbpl;
		}
	}
}
static void _cdecl
rb_ALL_BLACK(ROP_PB *rpb)
{
	int i;
	register unsigned long width, height, dbpl;
	register union { unsigned char *c; unsigned long *l; } da;
	register unsigned long d = 0xffffffffL;

	width	= rpb->sx2 - rpb->sx1 + 1;
	height	= rpb->sy2 - rpb->sy1 + 1;

	da.c = (char *)rpb->d_addr + ((long)rpb->sy1 * rpb->d_bypl) + rpb->sx1;
	
	dbpl = rpb->d_bypl;

	da.c += width;
	da.c += (height - 1) * dbpl;
	
	dbpl -= width;
	
	for (; height > 0; height--)
	{
		for (i = width >> 4; i > 0; i--)
		{
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
			*--da.l = d;
		}
		for (i = width & 0xf; i > 0; i--)
			*--da.c = (unsigned char)d;
		
		da.c -= dbpl;
	}
}

void _cdecl
draw_mousecurs_8b(register XMFORM *mf, register short x, register short y)
{
	register int width, height, xoff, yoff, bypl;
	register XMSAVE *ms;

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
	{
		register unsigned char *src, *dst;
		register int i, j, nl;

		dst = (unsigned char *)ms->save;
		src = mf->scr_base + (long)(((long)y * bypl) + x);

		ms->width = width;
		ms->height = height;
		ms->bypl = bypl;
		ms->valid = 1;
		ms->src = src;

		nl = bypl - width;

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
		register unsigned char *dst;


		nl = bypl - width;
		dst = mf->scr_base + (long)(((long)y * bypl) + x);
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
							*dst = (unsigned char)fgc;
						else
							*dst = (unsigned char)bgc;
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
							*dst = (unsigned char)fgc;
						else
							*dst = (unsigned char)bgc;
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
						*dst = (unsigned char)fgc;
					else
						*dst = (unsigned char)bgc;
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
		register unsigned char *src, *dst;


		dnl = bypl - width;
		snl = mf->mfbypl;
		dst = mf->scr_base + (long)(((long)y * bypl) + x);
		src = mf->data + (long)(((long)yoff * snl) + (long)xoff);
		snl -= width;
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
}

void _cdecl
restore_msave_8b(XMSAVE *ms)
{
	register int width, w, height, nl;
	register unsigned char *src, *dst;

	if (ms->valid)
	{
		src = (unsigned char *)ms->save;
		dst = (unsigned char *)ms->src;
		width = ms->width;
		nl = ms->bypl - width;
		for (height = ms->height; height > 0; height--)
		{
			for (w = width; w > 0; w--)
				*dst++ = *src++;
			dst += nl;
		}
		ms->valid = 0;
	}
}
