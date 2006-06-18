#include "display.h"
#include "memory.h"
#include "ovdi_defs.h"
#include "expand.h"
#include "drawing.h"
#include "4b_generic.h"

/* data tables needed - in 4b_data.c */
extern long col2long4b[];
extern unsigned short fillbuff4b[];
extern unsigned short maskbuff4b[];
extern unsigned short shifts4b[];

void _cdecl
spans_16x_4b(RASTER *r, COLINF *c, short *spans, short n, PatAttr *ptrn)
{
	short y;
	int wrmode;
	struct fill16x_api f;
	unsigned long slp[2];
	unsigned short slm;
	/*
	 * check if pattern is expanded and do expand it if it isnt
	*/
	wrmode = ptrn->wrmode;
	if (ptrn->expanded != 4 && ptrn->interior > FIS_SOLID)
	{
		//short color;
		//unsigned short data, p0, p1, p2, p3;
		short col[2];

		/*
		 * If there is no pointer to expanded data buffer,
		 * we gotta use a static one. Then we also need to
		 * to make sure pattern does not exceed 16 x 16. We
		 * also do not set the expanded flag so that the
		 * exp_data and mask pointers are cleared upon returning.
		*/
		if (!ptrn->exp_data)
		{
			if (ptrn->width > 16 || ptrn->height > 16)
				return;
			(long)ptrn->exp_data = (long)&fillbuff4b;
			(long)ptrn->mask = (long)&maskbuff4b;
			ptrn->expanded = 0;
		}
		else
			ptrn->expanded = r->resfmt.planes;
#if 1
		col[1] = ptrn->color[wrmode] & 0xff;
		col[0] = ptrn->bgcol[wrmode] & 0xff;

		expand( ptrn->width, ptrn->height,
			ptrn->planes, PF_ATARI, ptrn->data,
			4, PF_ATARI, ptrn->exp_data, (short *)&col, ptrn->mask);
#else
		s = (unsigned long *)ptrn->data;
		d = (unsigned long *)ptrn->exp_data;
		m = ptrn->mask;
		height = ptrn->height;

		if (interior == FIS_HOLLOW)
		{
			col[1] = 0;
			col[0] = 0;
		}
		else
		{
			col[1] = ptrn->color[wrmode];
			col[0] = ptrn->bgcol[wrmode];
		}

		/*
		 * do the expansion
		*/
		for (; height > 0; height--)
		{
			p0 = p1 = p2 = p3 = 0;
			data = *(unsigned short *)((unsigned short *)s)++;
			*m++ = data;
			for (i = 0; i < 16; i++)
			{
				if (data & 1)
					color = col[1];
				else
					color = col[0];
				p0 <<= 1; p0 |= (color & 1), color >>= 1;
				p1 <<= 1, p1 |= (color & 1), color >>= 1;
				p2 <<= 1, p2 |= (color & 1), color >>= 1;
				p3 <<= 1, p3 |= (color & 1);
				data >>= 1;
			}
			*d++ = ((long)p0 << 16) | p1, *d++ = ((long)p2 << 16) | p3;
		}
#endif
	}

	f.dbpl = r->bypl;

	if (ptrn->interior <= FIS_SOLID)
	{
		if (ptrn->interior == FIS_HOLLOW)
			slp[0] = slp[1] = slm = wrmode = 0;
		else {
			short fc = ptrn->color[wrmode] << 1;
			slp[0] = col2long4b[fc++];
			slp[1] = col2long4b[fc];
			slm	= 0xffff;
		}
		f.s = &slp;
		f.m = &slm;
		goto singleline;
	}
	if (ptrn->height == 1)
	{
		f.s = ptrn->exp_data;
		f.m = ptrn->mask;
		goto singleline;
	}

	switch (wrmode)
	{
		case 0: f.drawspan = ds_REPLACE_4b;	break;
		case 1: f.drawspan = ds_TRANS_4b;	break;
		case 2: f.drawspan = ds_XOR_4b;		break;
		case 3: f.drawspan = ds_ERASE_4b;	break;
		default: return;
	}

	for (;n > 0; n--)
	{
		register short y1, x1, x2;
		register int sb;

		y1 = *spans++;
		x1 = *spans++;
		x2 = *spans++;

		sb	= x1 & 0xf;

		f.sm	= ((long *)shifts4b)[sb];
		f.em	= ~((long *)shifts4b)[(x2 & 0xf) + 1];

		y	=  y1 % ptrn->height;
		f.words	=  ((sb + (x2 - x1) + 16) >> 4) - 1;
		x1	>>= 4;
		x1	<<= 3;
		f.d	=  (char *)r->base + x1;
		(char *)f.d += (long)y1 * f.dbpl;
		f.m	= (short *)ptrn->mask + y;
		f.s	= (char *)ptrn->exp_data + (y << 3);

		(*f.drawspan)(&f);
	}
	goto done;
singleline:
	switch (wrmode)
	{
		case 0: f.drawspan = ds_REPLACE_4b;	break;
		case 1: f.drawspan = ds_TRANS_4b;	break;
		case 2: f.drawspan = ds_XOR_4b;		break;
		case 3: f.drawspan = ds_ERASE_4b;	break;
		default: return;
	}

	for (;n > 0; n--)
	{
		register short y1, x1, x2;
		register int sb;

		y1 = *spans++;
		x1 = *spans++;
		x2 = *spans++;

		sb	= x1 & 0xf;
		f.sm	= ((long *)shifts4b)[sb];
		f.em	= ~((long *)shifts4b)[(x2 & 0xf) + 1];

		f.words	=  (((sb) + (x2 - x1) + 16) >> 4) - 1;
		x1	>>= 4;
		x1	<<= 3;
		f.d	=  (char *)r->base + x1; //(x1 >> 4);
		(char *)f.d += (long)y1 * f.dbpl;
		(*f.drawspan)(&f);
	}
done:
	if (!ptrn->expanded)
	{
		ptrn->exp_data = 0;
		ptrn->mask = 0;
	}
}		

void _cdecl
ds_REPLACE_4b(struct fill16x_api *f)
{
	int i;
	register long lsm = f->sm, lem = f->em, lp0, lp1;
	register long *d = f->d;
	long *s = f->s;

	lp0 = *s++;
	lp1 = *s;

	if (lsm == 0xffffffff)
	{
		if (lem == 0xffffffff)
		{
			if ((i = f->words))
			{
				for (; i > -1; i--) //for (i = words + 1; i > 0; i--)
				{
					*d++ = lp0;
					*d++ = lp1;
				}
			}
			else
			{
				*d++ = lp0;
				*d++ = lp1;
			}
		}
		else // endif (em == 0xffff)
		{
			if (!f->words)
			{
				lp0 &= lem;
				lp1 &= lem;
				*d++ = (*d & ~lem) | lp0;
				*d++ = (*d & ~lem) | lp1;
			}
			else
			{
				for (i = f->words; i > 0; i--)
				{
					*d++ = lp0;
					*d++ = lp1;
				}
				*d++ = (*d & ~lem) | (lp0 & lem);
				*d++ = (*d & ~lem) | (lp1 & lem);
			}
		}
	}
	else // (sm == 0xffff)
	{
		if (lem == 0xffffffff)
		{
			if (!f->words)
			{
				lp0 &= lsm;
				lp1 &= lsm;
				*d++ = (*d & ~lsm) | lp0;
				*d++ = (*d & ~lsm) | lp1;
			}
			else
			{
				*d++ = (*d & ~lsm) | (lp0 & lsm);
				*d++ = (*d & ~lsm) | (lp1 & lsm);
				for (i = f->words; i > 0; i--)
				{
					*d++ = lp0;
					*d++ = lp1;
				}
			}
		}
		else // Got both start and endmasks
		{
			if (!f->words)
			{
				lsm &= lem;
				lp0 &= lsm;
				lp1 &= lsm;
				*d++ = (*d & ~lsm) | lp0;
				*d++ = (*d & ~lsm) | lp1;
			}
			else
			{
				*d++ = (*d & ~lsm) | (lp0 & lsm);
				*d++ = (*d & ~lsm) | (lp1 & lsm);
				for (i = f->words - 1; i > 0; i--)
				{
					*d++ = lp0;
					*d++ = lp1;
				}
				*d++ = (*d & ~lem) | (lp0 & lem);
				*d++ = (*d & ~lem) | (lp1 & lem);
			}
		}
	}
}

void _cdecl
ds_TRANS_4b(struct fill16x_api *f)
{
	int i;
	register unsigned long lsm = f->sm, lem = f->em, lp0, lp1, tmp, lmask;
	register long *d = f->d;
	long *s = f->s;

	lp0 = *(unsigned short *)f->m;
	lmask = lp0;
	lmask |= lp0 << 16;
	lp0 = *s++;
	lp1 = *s;

	if (lsm == 0xffffffff)
	{
		if (lem == 0xffffffff)
		{
			if (!f->words)
			{
				lp0 &= lmask;
				lp1 &= lmask;
				*d++ = (*d & ~lmask) | lp0;
				*d++ = (*d & ~lmask) | lp1;
			}
			else
			{
				lp0 &= lmask;
				lp1 &= lmask;
				for (i = f->words + 1; i > 0; i--)
				{
					*d++ = (*d & ~lmask) | lp0;
					*d++ = (*d & ~lmask) | lp1;
				}
			}
		}
		else // endif (em == 0xffff)
		{
			if (!f->words)
			{
				lmask &= lem;
				lp0 &= lmask;
				lp1 &= lmask;
				*d++ = (*d & ~lmask) | lp0;
				*d++ = (*d & ~lmask) | lp1;
			}
			else
			{
				lp0 &= lmask;
				lp1 &= lmask;
				tmp = lmask & lem;
				for (i = f->words; i > 0; i--)
				{
					*d++ = (*d & ~lmask ) | lp0;
					*d++ = (*d & ~lmask ) | lp1;
				}
				*d++ = (*d & ~tmp) | (lp0 & tmp);
				*d++ = (*d & ~tmp) | (lp1 & tmp);
			}
		}
	}
	else // (sm == 0xffff)
	{
		if (lem == 0xffffffff)
		{
			if (!f->words)
			{
				lmask &= lsm;
				lp0 &= lmask;
				lp1 &= lmask;
				*d++ = (*d & ~lmask) | lp0;
				*d++ = (*d & ~lmask) | lp1;
			}
			else
			{
				lp0 &= lmask;
				lp1 &= lmask;
				tmp = lmask & lsm;
				*d++ = (*d & ~tmp) | (lp0 & tmp);
				*d++ = (*d & ~tmp) | (lp1 & tmp);
				for (i = f->words; i > 0; i--)
				{
					*d++ = (*d & ~lmask) | lp0;
					*d++ = (*d & ~lmask) | lp1;
				}
			}
		}
		else // Got both start and endlmasks
		{
			if (!f->words)
			{
				lsm &= lem;
				lmask &= lsm;
				lp0 &= lmask;
				lp1 &= lmask;
				*d++ = (*d & ~lmask) | lp0;
				*d++ = (*d & ~lmask) | lp1;
			}
			else
			{
				lp0 &= lmask;
				lp1 &= lmask;
				tmp = lmask & lsm;
				lem = lmask & lem;
				*d++ = (*d & ~tmp) | (lp0 & tmp);
				*d++ = (*d & ~tmp) | (lp1 & tmp);
				for (i = f->words - 1; i > 0; i--)
				{
					*d++ = (*d & ~lmask) | lp0;
					*d++ = (*d & ~lmask) | lp1;
				}
				*d++ = (*d & ~lem) | (lp0 & lem);
				*d++ = (*d & ~lem) | (lp1 & lem);
			}
		}
	}
}
void _cdecl
ds_XOR_4b(struct fill16x_api *f)
{
	int i;
	register long lsm = f->sm, lem = f->em, lmask;
	register long *d = f->d;

	lmask = 0xffffffffUL;

	if (lsm == 0xffffffff)
	{
		if (lem == 0xffffffff)
		{
			if (!f->words)
			{
				*d++ ^= lmask;
				*d++ ^= lmask;
			}
			else
			{
				for (i = f->words + 1; i > 0; i--)
				{
					*d++ ^= lmask;
					*d++ ^= lmask;
				}
			}
		}
		else // endif (em == 0xffff)
		{
			if (!f->words)
			{
				*d++ = (*d & ~lem) | ((*d ^ lmask) & lem);
				*d++ = (*d & ~lem) | ((*d ^ lmask) & lem);
			}
			else
			{
				for (i = f->words; i > 0; i--)
				{
					*d++ ^= lmask;
					*d++ ^= lmask;
				}
				*d++ = (*d & ~lem) | ((*d ^ lmask) & lem);
				*d++ = (*d & ~lem) | ((*d ^ lmask) & lem);
			}
		}
	}
	else // (sm == 0xffff)
	{
		if ((short)lem == 0xffffffff)
		{
			if (!f->words)
			{
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
			}
			else
			{
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
				for (i = f->words; i > 0; i--)
				{
					*d++ ^= lmask;
					*d++ ^= lmask;
				}
			}
		}
		else // Got both start and endlmasks
		{
			if (!f->words)
			{
				lsm &= lem;
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
			}
			else
			{
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
				for (i = f->words - 1; i > 0; i--)
				{
					*d++ ^= lmask;
					*d++ ^= lmask;
				}
				*d++ = (*d & ~lem) | ((*d ^ lmask) & lem);
				*d++ = (*d & ~lem) | ((*d ^ lmask) & lem);
			}
		}
	}
}
void _cdecl
ds_ERASE_4b(struct fill16x_api *f)
{
	int i;
	register long lsm = f->sm, lem = f->em, lp0 = 0xffffffffL, lmask;
	register long *d = f->d;

	lmask = *(short *)f->m;
	lmask = lmask | lmask << 16;

	if (lsm == 0xffffffff)
	{
		if (lem == 0xffffffff)
		{
			if (!f->words)
			{
				*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
				*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
			}
			else
			{
				for (i = f->words + 1; i > 0; i--)
				{
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
				}
			}
		}
		else // endif (em == 0xffff)
		{
			if (!f->words)
			{
				*d++ = ((*d & lmask) & ~lem) | (((*d ^ lp0) & ~lmask) & lem);
				*d++ = ((*d & lmask) & ~lem) | (((*d ^ lp0) & ~lmask) & lem);
			}
			else
			{
				for (i = f->words; i > 0; i--)
				{
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
				}
				*d++ = ((*d & lmask) & ~lem) | (((*d ^ lp0) & ~lmask) & lem);
				*d++ = ((*d & lmask) & ~lem) | (((*d ^ lp0) & ~lmask) & lem);
			}
		}
	}
	else // (sm == 0xffff)
	{
		if (lem == 0xffffffff)
		{
			if (!f->words)
			{
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
			}
			else
			{
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
				for (i = f->words; i > 0; i--)
				{
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
				}
			}
		}
		else // Got both start and endlmasks
		{
			if (!f->words)
			{
				lsm &= lem;
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
			}
			else
			{
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
				for (i = f->words - 1; i > 0; i--)
				{
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
				}
				*d++ = ((*d & lmask) & ~lem) | (((*d ^ lp0) & ~lmask) & lem);
				*d++ = ((*d & lmask) & ~lem) | (((*d ^ lp0) & ~lmask) & lem);
			}
		}
	}
}
