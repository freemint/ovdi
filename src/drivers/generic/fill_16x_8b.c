#include "display.h"
#include "memory.h"
#include "ovdi_defs.h"
#include "expand.h"

#include "8b_generic.h"

extern short logit;

static unsigned char fillbuff[16 * 16];
static unsigned char maskbuff[16 * 16];
#if 0
static unsigned long nib2long [] = 
{
	0x00000000,
	0x000000ff,
	0x0000ff00,
	0x0000ffff,

	0x00ff0000,
	0x00ff00ff,
	0x00ffff00,
	0x00ffffff,

	0xff000000,
	0xff0000ff,
	0xff00ff00,
	0xff00ffff,

	0xffff0000,
	0xffff00ff,
	0xffffff00,
	0xffffffff
};
#endif
void
fill_16x_8b(RASTER *r, COLINF *c, short *corners, short interior, PatAttr *ptrn)
{
	int i, words, bypl, height, y, wrmode, sm, em;
	unsigned long lp0, lp1, lp2, lp3;
	unsigned long *s, *d, *m;
	unsigned long fill[4];
	unsigned long mask[4];

	/*
	 * check if pattern is expanded and do expand it if it isnt
	*/
	wrmode = ptrn->wrmode;
	if (ptrn->expanded != 8 && interior > FIS_SOLID)
	{
		short col[2];
		//unsigned short data;
		//unsigned long m0, m1, m2, m3; //p0, p1;

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

			(long)ptrn->exp_data = (long)&fillbuff;
			(long)ptrn->mask = (long)&maskbuff;
			ptrn->expanded = 0;
		}
		else
			ptrn->expanded = r->planes;
			
		
#if 1
		if (interior == FIS_HOLLOW)
		{
			col[1] = ptrn->bgcol[wrmode] & 0xff; //0;
			col[0] = col[1];
		}
		else
		{
			col[1] = ptrn->color[wrmode] & 0xff;
			col[0] = ptrn->bgcol[wrmode] & 0xff;
		}
		expand( ptrn->width, ptrn->height,
			ptrn->planes, PF_ATARI, ptrn->data,
			8, PF_PACKED, ptrn->exp_data, (short *)&col, ptrn->mask);
#else

		s = (unsigned long *)ptrn->data;
		d = (unsigned long *)ptrn->exp_data;
		m = (unsigned long *)ptrn->mask;
		height = ptrn->height;

		if (interior == FIS_HOLLOW)
		{
			col[1] = ptrn->bgcol[wrmode] & 0xff; //0;
			col[0] = col[1];
		}
		else
		{
			col[1] = ptrn->color[wrmode] & 0xff;
			col[0] = ptrn->bgcol[wrmode] & 0xff;
		}

		/*
		 * do the expansion
		*/
		for (; height > 0; height--)
		{
			data = *(unsigned short *)((short *)s)++;
			lp0 = lp1 = lp2 = lp3 = 0;
			m0 = m1 = m2 = m3 = 0;
			for (y = 0; y < 4; y++)
			{
				lp0 <<= 8, lp1 <<=8, lp2 <<= 8, lp3 <<= 8;
				m0 <<= 8, m1 <<= 8, m2 <<= 8, m3 <<= 8;
				if (data & (1<<15))	lp0 |= col[1], m0 |= 0xff;
				else			lp0 |= col[0];
				if (data & (1<<11))	lp1 |= col[1], m1 |= 0xff;
				else			lp1 |= col[0];
				if (data & (1<<7))	lp2 |= col[1], m2 |= 0xff;
				else			lp2 |= col[0];
				if (data & (1<<3))	lp3 |= col[1], m3 |= 0xff;
				else			lp3 |= col[0];
				data <<= 1;
			}
			*d++ = lp0, *d++ = lp1, *d++ = lp2, *d++ = lp3;
			*m++ = m0, *m++ = m1, *m++ = m2, *m++ = m3;
		}
#endif
	}
	bypl = r->bypl;
	words = (((corners[0] & 0xf) + (corners[2] - corners[0]) + 16) >> 4);
	height = corners[3] - corners[1] + 1;

	sm = corners[0] & 0xf;
	em = (corners[2] + 1) & 0xf;

	(char *)d = (char *)r->base + (long)((corners[0] >> 4) << 4) + ((long)corners[1] * bypl);

	bypl -= ((words << 4) - ((16 - em) & 0xf));

	if (sm) words--;
	if (em) words--;

	/*
	 * If interior is hollow or solid, we can do things faster,
	 * without expanding pattern.
	*/
	if (interior <= FIS_SOLID)
	{
		if (interior == FIS_HOLLOW) {
			fill[0] = fill[1] = fill[2] = fill[3] = 0;
			mask[0] = mask[1] = mask[2] = mask[3] = 0;
			wrmode = 0;
		} else {
			short fc = ptrn->color[wrmode];
			fill[0] = fill[1] = fill[2] = fill[3] = ((long)fc << 24) | ((long)fc << 16) | ((long)fc << 8) | fc;
			mask[0] = mask[1] = mask[2] = mask[3] = 0xffffffffL; }
		s = (unsigned long *)&fill;
		m = (unsigned long *)&mask;
		goto singleline;
	}
#if 0
	if (interior == FIS_HOLLOW)
		wrmode = 0;
#endif		
	/* 
	 * If pattern is a single-line...
	*/
	if (ptrn->height == 1)
	{
		s = (unsigned long *)ptrn->exp_data;
		m = (unsigned long *)ptrn->mask;
		goto singleline;
	}
	else
		y = corners[1] % ptrn->height;

	s = (unsigned long *)ptrn->exp_data + ((long)y << 2);
	m = (unsigned long *)ptrn->mask + ((long)y << 2);

	SYNC_RASTER(r);

	switch (wrmode)
	{
		case 0:
		{
			if (words < 0)
			{
				for (; height > 0; height--)
				{
					char *src = (char *)s;

					if (sm)
						src += sm; (char *)d += sm;
					for (i = em - sm; i > 0; i--)
							*(char *)((char *)d)++ = *src++;
					y++;
					if (y >= ptrn->height)
						y = 0, s = (unsigned long *)ptrn->exp_data;
					else
						(char *)s += 16;
					(char *)d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					if (sm)
					{
						char *src = (char *)s + sm;
						(char *)d += sm;
						for (i = 16 - sm; i > 0; i--)
							*(char *)((char *)d)++ = *src++;
					}
					if (words > 0)
					{
						unsigned long *src = s;
						lp0 = *src++, lp1 = *src++, lp2 = *src++, lp3 = *src;
						for (i = words; i > 0; i--)
							*d++ = lp0, *d++ = lp1, *d++ = lp2, *d++ = lp3;
					}
					if (em)
					{
						char *src = (char *)s;
						for (i = em; i > 0; i--)
							*(char *)((char *)d)++ = *src++;
					}
					y++;
					if (y >= ptrn->height)
						y = 0, s = (unsigned long *)ptrn->exp_data;
					else
						(char *)s += 16;
					(char *)d += bypl;
				}
			}
			break;
		}
		case 1:
		{
			if (words < 0)
			{
				for (; height > 0; height--)
				{
					char *src = (char *)s;
					char *msk = (char *)m;

					if (sm)
						src += sm, msk += sm, (char *)d += sm;
					for (i = em - sm; i > 0; i--)
					{
							if (*msk++)
								*(char *)((char *)d)++ = *src++;
							else
								((char *)d)++, src++;
					}
					y++;
					if (y >= ptrn->height)
						y = 0, s = (unsigned long *)ptrn->exp_data, m = (unsigned long *)ptrn->mask;
					else
						(char *)s += 16, (char *)m += 16;
					(char *)d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					if (sm)
					{
						char *src = (char *)s + sm;
						char *msk = (char *)m + sm;
						(char *)d += sm;
						for (i = 16 - sm; i > 0; i--)
						{
							if (*msk++)
								*(char *)((char *)d)++ = *src++;
							else
								((char *)d)++, src++;
						}
					}
					if (words > 0)
					{
						unsigned long *src = s, *msk;
						lp0 = *src++, lp1 = *src++, lp2 = *src++, lp3 = *src;
						for (i = words; i > 0; i--)
						{
							msk = m;
							*d++ = (*d & ~*msk) | (lp0 & *msk), msk++;
							*d++ = (*d & ~*msk) | (lp1 & *msk), msk++;
							*d++ = (*d & ~*msk) | (lp2 & *msk), msk++;
							*d++ = (*d & ~*msk) | (lp3 & *msk);
						}
					}
					if (em)
					{
						char *src = (char *)s, *msk = (char *)m;
						for (i = em; i > 0; i--)
						{
							if (*msk++)
								*(char *)((char *)d)++ = *src++;
							else
								((char *)d)++, src++;
						}
					}
					y++;
					if (y >= ptrn->height)
						y = 0, s = (unsigned long *)ptrn->exp_data, m = (unsigned long *)ptrn->mask;
					else
						(char *)s += 16, (char *)m += 16;
					(char *)d += bypl;
				}
			}
			break;
		}
		case 2:
		{
			lp0 = 0xffffffffL;

			if (words < 0)
			{
				for (; height > 0; height--)
				{
					if (sm)
						(char *)d += sm;
					for (i = em - sm; i > 0; i--)
							*(char *)((char *)d)++ ^= (char)lp0;
					(char *)d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					if (sm)
					{
						(char *)d += sm;
						for (i = 16 - sm; i > 0; i--)
							*(char *)((char *)d)++ ^= (char)lp0;
					}
					if (words > 0)
					{
						for (i = words; i > 0; i--)
							*d++ ^= lp0, *d++ ^= lp0, *d++ ^= lp0, *d++ ^= lp0;
					}
					if (em)
					{
						for (i = em; i > 0; i--)
							*(char *)((char *)d)++ ^= (char)lp0;
					}
					(char *)d += bypl;
				}
			}
			break;
		}
		case 3:
		{
			lp0 = 0xffffffffL;

			if (words < 0)
			{
				for (; height > 0; height--)
				{
					char *msk = (char *)m;

					if (sm)
						msk += sm, (char *)d += sm;
					for (i = em - sm; i > 0; i--)
					{
							if (!*msk++)
								*(char *)((char *)d)++ ^= (char)lp0;
							else
								((char *)d)++;
					}
					y++;
					if (y >= ptrn->height)
						y = 0, m = (unsigned long *)ptrn->mask;
					else
						(char *)m += 16;
					(char *)d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					if (sm)
					{
						char *msk = (char *)m + sm;

						(char)lp0 = (char)0xff;
						(char *)d += sm;
						for (i = 16 - sm; i > 0; i--)
						{
							if (!*msk++)
								*(char *)((char *)d)++ ^= (char)lp0;
							else
								((char *)d)++;
						}
					}
					if (words > 0)
					{
						unsigned long *msk = m;
						lp0 = *msk++, lp1 = *msk++, lp2 = *msk++, lp3 = *msk;
						for (i = words; i > 0; i--)
						{
							*d++ = (*d & lp0) | (*d ^ ~lp0);
							*d++ = (*d & lp1) | (*d ^ ~lp1);
							*d++ = (*d & lp2) | (*d ^ ~lp2);
							*d++ = (*d & lp3) | (*d ^ ~lp3);
						}
					}
					if (em)
					{
						char *msk = (char *)m;

						(char)lp0 = (char)0xff;
						for (i = em; i > 0; i--)
						{
							if (!*msk++)
								*(char *)((char *)d)++ ^= lp0;
							else
								((char *)d)++;
						}
					}
					y++;
					if (y >= ptrn->height)
						y = 0, m = (unsigned long *)ptrn->mask;
					else
						(char *)m += 16;
					(char *)d += bypl;
				}
			}
			break;
		}
	}
	goto done;

singleline:
	//s = (unsigned long *)ptrn->exp_data;
	//m = (unsigned long *)ptrn->mask;

	SYNC_RASTER(r);

	switch (wrmode)
	{
		case 0:
		{
			if (words < 0)
			{
				for (; height > 0; height--)
				{
					char *src = (char *)s;

					if (sm)
						src += sm; (char *)d += sm;
					for (i = em - sm; i > 0; i--)
							*(char *)((char *)d)++ = *src++;
					(char *)d += bypl;
				}
			}
			else
			{
				unsigned long *src = s;
				lp0 = *src++, lp1 = *src++, lp2 = *src++, lp3 = *src;
				for (; height > 0; height--)
				{
					if (sm)
					{
						(char *)src = (char *)s + sm;
						(char *)d += sm;
						for (i = 16 - sm; i > 0; i--)
							*(char *)((char *)d)++ = *(char *)((char *)src)++;
					}
					if (words > 0)
					{
						for (i = words; i > 0; i--)
							*d++ = lp0, *d++ = lp1, *d++ = lp2, *d++ = lp3;
					}
					if (em)
					{
						(char *)src = (char *)s;
						for (i = em; i > 0; i--)
							*(char *)((char *)d)++ = *(char *)((char *)src)++;
					}
					(char *)d += bypl;
				}
			}
			break;
		}
		case 1:
		{
			if (words < 0)
			{
				for (; height > 0; height--)
				{
					char *src = (char *)s;
					char *msk = (char *)m;

					if (sm)
						src += sm, msk += sm, (char *)d += sm;
					for (i = em - sm; i > 0; i--)
					{
							if (*msk++)
								*(char *)((char *)d)++ = *src++;
							else
								((char *)d)++, src++;
					}
					(char *)d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					if (sm)
					{
						char *src = (char *)s + sm;
						char *msk = (char *)m + sm;
						(char *)d += sm;
						for (i = 16 - sm; i > 0; i--)
						{
							if (*msk++)
								*(char *)((char *)d)++ = *src++;
							else
								((char *)d)++, src++;
						}
					}
					if (words > 0)
					{
						unsigned long *src = s, *msk;
						lp0 = *src++, lp1 = *src++, lp2 = *src++, lp3 = *src;
						for (i = words; i > 0; i--)
						{
							msk = m;
							*d++ = (*d & ~*msk) | (lp0 & *msk), msk++;
							*d++ = (*d & ~*msk) | (lp1 & *msk), msk++;
							*d++ = (*d & ~*msk) | (lp2 & *msk), msk++;
							*d++ = (*d & ~*msk) | (lp3 & *msk);
						}
					}
					if (em)
					{
						char *src = (char *)s, *msk = (char *)m;
						for (i = em; i > 0; i--)
						{
							if (*msk++)
								*(char *)((char *)d)++ = *src++;
							else
								((char *)d)++, src++;
						}
					}
					(char *)d += bypl;
				}
			}
			break;
		}
		case 2:
		{
			lp0 = 0xffffffffL;

			if (words < 0)
			{
				for (; height > 0; height--)
				{
					if (sm)
						(char *)d += sm;
					for (i = em - sm; i > 0; i--)
							*(char *)((char *)d)++ ^= (char)lp0;
					(char *)d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					if (sm)
					{
						(char *)d += sm;
						for (i = 16 - sm; i > 0; i--)
							*(char *)((char *)d)++ ^= (char)lp0;
					}
					if (words > 0)
					{
						for (i = words; i > 0; i--)
							*d++ ^= lp0, *d++ ^= lp0, *d++ ^= lp0, *d++ ^= lp0;
					}
					if (em)
					{
						for (i = em; i > 0; i--)
							*(char *)((char *)d)++ ^= (char)lp0;
					}
					(char *)d += bypl;
				}
			}
			break;
		}
		case 3:
		{
			lp0 = 0xffffffffL;

			if (words < 0)
			{
				for (; height > 0; height--)
				{
					char *msk = (char *)m;

					if (sm)
						msk += sm, (char *)d += sm;
					for (i = em - sm; i > 0; i--)
					{
							if (!*msk++)
								*(char *)((char *)d)++ ^= (char)lp0;
							else
								((char *)d)++;
					}
					(char *)d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					if (sm)
					{
						char *msk = (char *)m + sm;

						(char)lp0 = (char)0xff;
						(char *)d += sm;
						for (i = 16 - sm; i > 0; i--)
						{
							if (!*msk++)
								*(char *)((char *)d)++ ^= (char)lp0;
							else
								((char *)d)++;
						}
					}
					if (words > 0)
					{
						unsigned long *msk = m;
						lp0 = *msk++, lp1 = *msk++, lp2 = *msk++, lp3 = *msk;
						for (i = words; i > 0; i--)
						{
							*d++ = (*d & lp0) | (*d ^ ~lp0);
							*d++ = (*d & lp1) | (*d ^ ~lp1);
							*d++ = (*d & lp2) | (*d ^ ~lp2);
							*d++ = (*d & lp3) | (*d ^ ~lp3);
						}
					}
					if (em)
					{
						char *msk = (char *)m;

						(char)lp0 = (char)0xff;
						for (i = em; i > 0; i--)
						{
							if (!*msk++)
								*(char *)((char *)d)++ ^= lp0;
							else
								((char *)d)++;
						}
					}
					(char *)d += bypl;
				}
			}
			break;
		}
	}

done:	if (!ptrn->expanded)
	{
		ptrn->exp_data = 0;
		ptrn->mask = 0;
	}
}
