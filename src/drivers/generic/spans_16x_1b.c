#include "display.h"
#include "memory.h"
#include "ovdi_defs.h"
#include "expand.h"
#include "drawing.h"
#include "1b_generic.h"

/* data tables needed - in 4b_data.c */
//extern long col2long4b[];
extern unsigned short fillbuff1b[];
extern unsigned short maskbuff1b[];
extern unsigned short shifts1b[];

void _cdecl
spans_16x_1b(RASTER *r, COLINF *c, short *spans, short n, PatAttr *ptrn)
{
	int y, wrmode;
	struct fill16x_api f;
	unsigned short slp;
	unsigned short slm;
	/*
	 * check if pattern is expanded and do expand it if it isnt
	*/
	wrmode = ptrn->wrmode;
	if (ptrn->expanded != 1 && ptrn->interior > FIS_SOLID)
	{
		int height, i;
		unsigned short data, p0;
		unsigned short *m, *s, *d;
		short fc, bc;

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
			(long)ptrn->exp_data = (long)&fillbuff1b;
			(long)ptrn->mask = (long)&maskbuff1b;
			ptrn->expanded = 0;
		}
		else
			ptrn->expanded = r->res.planes;
			
		s = (unsigned short *)ptrn->data;
		m = (unsigned short *)ptrn->mask;
		d = (unsigned short *)ptrn->exp_data;

		fc = ptrn->color[wrmode] & 1;
		bc = ptrn->bgcol[wrmode] & 1;
		height = ptrn->height;
		for (; height > 0; height--)
		{
			data = *s;
			p0 = 0;
			for (i = 0; i < 16; i++)
			{
				p0 <<= 1;
				if (data & 0x8000)
					p0 |= fc;
				else
					p0 |= bc;
				data <<= 1;
			}
			*m++ = *s++;
			*d++ = p0;
		}
	}		

	f.dbpl = r->bypl;

	if (ptrn->interior <= FIS_SOLID)
	{
		if (ptrn->interior == FIS_HOLLOW)
			slp = slm = wrmode = 0;
		else {
			slp = ptrn->color[wrmode] == 0 ? 0 : 0xffff;
			slm = 0xffff;
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
		case 0: f.drawspan = ds_REPLACE_1b;	break;
		case 1: f.drawspan = ds_TRANS_1b;	break;
		case 2: f.drawspan = ds_XOR_1b;		break;
		case 3: f.drawspan = ds_ERASE_1b;	break;
		default: return;
	}
	SYNC_RASTER(r);
	for (;n > 0; n--)
	{
		register short y1, x1, x2;
		int sb;

		y1 = *spans++;
		x1 = *spans++;
		x2 = *spans++;

		sb	= x1 & 0xf;

		f.sm	= shifts1b[sb];
		f.em	= ~shifts1b[(x2 & 0xf) + 1];

		y	=  y1 % ptrn->height;
		f.words	=  ((sb + (x2 - x1) + 16) >> 4) - 1;
		x1	>>= 4;
		x1	<<= 1;
		f.d	=  (char *)r->base + x1;
		(char *)f.d += (long)y1 * f.dbpl;
		f.m	= (short *)ptrn->mask + y;
		f.s	= (short *)ptrn->exp_data + y;

		(*f.drawspan)(&f);
	}
	goto done;
singleline:
	switch (wrmode)
	{
		case 0: f.drawspan = ds_REPLACE_1b;	break;
		case 1: f.drawspan = ds_TRANS_1b;	break;
		case 2: f.drawspan = ds_XOR_1b;		break;
		case 3: f.drawspan = ds_ERASE_1b;	break;
		default: return;
	}
	SYNC_RASTER(r);
	for (;n > 0; n--)
	{
		register short y1, x1, x2;
		int sb;

		y1 = *spans++;
		x1 = *spans++;
		x2 = *spans++;

		sb	= x1 & 0xf;
		f.sm	= shifts1b[sb];
		f.em	= ~shifts1b[(x2 & 0xf) + 1];

		f.words	=  (((sb) + (x2 - x1) + 16) >> 4) - 1;
		x1	>>= 4;
		x1	<<= 1;
		f.d	=  (char *)r->base + x1;
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
ds_REPLACE_1b(struct fill16x_api *f)
{
	int i;
	register unsigned short lsm = f->sm, lem = f->em, lp0;
	register short *d = f->d;

	lp0 = *(unsigned short *)f->s;

	if (lsm == 0xffff)
	{
		if (lem == 0xffff)
		{
			if ((i = f->words))
			{
				for (; i > -1; i--) //for (i = words + 1; i > 0; i--)
				{
					*d++ = lp0;
				}
			}
			else
			{
				*d++ = lp0;
			}
		}
		else // endif (em == 0xffff)
		{
			if (!f->words)
			{
				lp0 &= lem;
				*d++ = (*d & ~lem) | lp0;
			}
			else
			{
				for (i = f->words; i > 0; i--)
				{
					*d++ = lp0;
				}
				*d++ = (*d & ~lem) | (lp0 & lem);
			}
		}
	}
	else // (sm == 0xffff)
	{
		if (lem == 0xffff)
		{
			if (!f->words)
			{
				lp0 &= lsm;
				*d++ = (*d & ~lsm) | lp0;
			}
			else
			{
				*d++ = (*d & ~lsm) | (lp0 & lsm);
				for (i = f->words; i > 0; i--)
				{
					*d++ = lp0;
				}
			}
		}
		else // Got both start and endmasks
		{
			if (!f->words)
			{
				lsm &= lem;
				lp0 &= lsm;
				*d++ = (*d & ~lsm) | lp0;
			}
			else
			{
				*d++ = (*d & ~lsm) | (lp0 & lsm);
				for (i = f->words - 1; i > 0; i--)
				{
					*d++ = lp0;
				}
				*d++ = (*d & ~lem) | (lp0 & lem);
			}
		}
	}
}

void _cdecl
ds_TRANS_1b(struct fill16x_api *f)
{
	int i;
	register unsigned short lsm = f->sm, lem = f->em, lp0, tmp, lmask;
	register short *d = f->d;

	lmask = *(unsigned short *)f->m;
	lp0 = *(unsigned short *)f->s;

	if (lsm == 0xffff)
	{
		if (lem == 0xffff)
		{
			if (!f->words)
			{
				lp0 &= lmask;
				*d++ = (*d & ~lmask) | lp0;
			}
			else
			{
				lp0 &= lmask;
				for (i = f->words + 1; i > 0; i--)
				{
					*d++ = (*d & ~lmask) | lp0;
				}
			}
		}
		else // endif (em == 0xffff)
		{
			if (!f->words)
			{
				lmask &= lem;
				lp0 &= lmask;
				*d++ = (*d & ~lmask) | lp0;
			}
			else
			{
				lp0 &= lmask;
				tmp = lmask & lem;
				for (i = f->words; i > 0; i--)
				{
					*d++ = (*d & ~lmask ) | lp0;
				}
				*d++ = (*d & ~tmp) | (lp0 & tmp);
			}
		}
	}
	else // (sm == 0xffff)
	{
		if (lem == 0xffff)
		{
			if (!f->words)
			{
				lmask &= lsm;
				lp0 &= lmask;
				*d++ = (*d & ~lmask) | lp0;
			}
			else
			{
				lp0 &= lmask;
				tmp = lmask & lsm;
				*d++ = (*d & ~tmp) | (lp0 & tmp);
				for (i = f->words; i > 0; i--)
				{
					*d++ = (*d & ~lmask) | lp0;
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
				*d++ = (*d & ~lmask) | lp0;
			}
			else
			{
				lp0 &= lmask;
				tmp = lmask & lsm;
				lem = lmask & lem;
				*d++ = (*d & ~tmp) | (lp0 & tmp);
				for (i = f->words - 1; i > 0; i--)
				{
					*d++ = (*d & ~lmask) | lp0;
				}
				*d++ = (*d & ~lem) | (lp0 & lem);
			}
		}
	}
}
void _cdecl
ds_XOR_1b(struct fill16x_api *f)
{
	int i;
	register short lsm = f->sm, lem = f->em, lmask;
	register short *d = f->d;

	lmask = 0xffff;

	if (lsm == 0xffff)
	{
		if (lem == 0xffff)
		{
			if (!f->words)
			{
				*d++ ^= lmask;
			}
			else
			{
				for (i = f->words + 1; i > 0; i--)
				{
					*d++ ^= lmask;
				}
			}
		}
		else // endif (em == 0xffff)
		{
			if (!f->words)
			{
				*d++ = (*d & ~lem) | ((*d ^ lmask) & lem);
			}
			else
			{
				for (i = f->words; i > 0; i--)
				{
					*d++ ^= lmask;
				}
				*d++ = (*d & ~lem) | ((*d ^ lmask) & lem);
			}
		}
	}
	else // (sm == 0xffff)
	{
		if ((short)lem == 0xffff)
		{
			if (!f->words)
			{
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
			}
			else
			{
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
				for (i = f->words; i > 0; i--)
				{
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
			}
			else
			{
				*d++ = (*d & ~lsm) | ((*d ^ lmask) & lsm);
				for (i = f->words - 1; i > 0; i--)
				{
					*d++ ^= lmask;
				}
				*d++ = (*d & ~lem) | ((*d ^ lmask) & lem);
			}
		}
	}
}
void _cdecl
ds_ERASE_1b(struct fill16x_api *f)
{
	int i;
	register short lsm = f->sm, lem = f->em, lp0 = 0xffff, lmask;
	register short *d = f->d;

	lmask = *(short *)f->m;

	if (lsm == 0xffff)
	{
		if (lem == 0xffff)
		{
			if (!f->words)
			{
				*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
			}
			else
			{
				for (i = f->words + 1; i > 0; i--)
				{
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
				}
			}
		}
		else // endif (em == 0xffff)
		{
			if (!f->words)
			{
				*d++ = ((*d & lmask) & ~lem) | (((*d ^ lp0) & ~lmask) & lem);
			}
			else
			{
				for (i = f->words; i > 0; i--)
				{
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
				}
				*d++ = ((*d & lmask) & ~lem) | (((*d ^ lp0) & ~lmask) & lem);
			}
		}
	}
	else // (sm == 0xffff)
	{
		if (lem == 0xffff)
		{
			if (!f->words)
			{
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
			}
			else
			{
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
				for (i = f->words; i > 0; i--)
				{
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
			}
			else
			{
				*d++ = ((*d & lmask) & ~lsm) | (((*d ^ lp0) & ~lmask) & lsm);
				for (i = f->words - 1; i > 0; i--)
				{
					*d++ = (*d & lmask) | ((*d ^ lp0) & ~lmask);
				}
				*d++ = ((*d & lmask) & ~lem) | (((*d ^ lp0) & ~lmask) & lem);
			}
		}
	}
}
