#include "memory.h"
#include "ovdi_defs.h"

#include "4b_generic.h"

static unsigned short fillbuff[4*16];
static unsigned short maskbuff[16];

void
fill_16x_4b(RASTER *r, COLINF *c, short *corners, short interior, PatAttr *ptrn)
{
	int i, words, bypl, height, y, wrmode;
	unsigned long lp0, lp1, lsm, lem;
	unsigned short *m;
	unsigned long *s, *d;

	/*
	 * check if pattern is expanded and do expand it if it isnt
	*/
	wrmode = ptrn->wrmode;

	if (!ptrn->expanded || ptrn->expanded != 4)
	{
		short bc, fc, col;
		unsigned short data, p0, p1, p2, p3;

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
			
		
		s = (unsigned long *)ptrn->data;
		d = (unsigned long *)ptrn->exp_data;
		m = ptrn->mask;
		height = ptrn->height;

		if (interior == FIS_HOLLOW)
		{
			fc = 0;
			bc = 0;
		}
		else
		{
			fc = ptrn->color[wrmode];
			bc = ptrn->bgcol[wrmode];
		}

		/*
		 * do the expansion
		*/
		for (; height > 0; height--)
		{
			p0 = p1 = p2 = p3 = 0;
			data = *(unsigned short *)((short *)s)++;
			*m++ = data;
			for (i = 0; i < 16; i++)
			{
				if (data & 1)
					col = fc;
				else
					col = bc;
				p0 <<= 1; p0 |= (col & 1); col >>= 1;
				p1 <<= 1, p1 |= (col & 1), col >>= 1;
				p2 <<= 1, p2 |= (col & 1), col >>= 1;
				p3 <<= 1, p3 |= (col & 1);
				data >>= 1;
			}
			*d++ = ((long)p0 << 16) | p1, *d++ = ((long)p2 << 16) | p3;
		}
	}
	bypl = r->bypl >> 2;
	words = (((corners[0] & 0xf) + (corners[2] - corners[0]) + 16) >> 4) - 1;
	height = corners[3] - corners[1] + 1;
	{
		unsigned short sm, em;

		sm = 0xffff >> (corners[0] & 0xf);
		em = 0xffff << (15 - (corners[2] & 0xf));
		lsm = ((unsigned long)sm << 16) | sm;
		lem = ((unsigned long)em << 16) | em;
	}

	if (interior == FIS_HOLLOW)
		wrmode = 0;
		
	if (ptrn->height == 1)
		y = 0;
	else
		y = corners[1] % ptrn->height;

	s = (unsigned long *)ptrn->exp_data + ((long)y << 1);
	m = (unsigned short *)ptrn->mask + y;
	d = (unsigned long *)r->base + (long)((corners[0] >> 4) << 1) + ((long)corners[1] * bypl);
	bypl -= (words + 1) << 1; /* longs */

	switch (wrmode)
	{
		case 0:
		{
			if (lsm == 0xffffffff)
			{
				if (lem == 0xffffffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							*d++ = *s++;
							*d++ = *s++;
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							lp0 = *s++;
							lp1 = *s++;
							for (i = words + 1; i > 0; i--)
							{
								*d++ = lp0;
								*d++ = lp1;
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data;
							d += bypl;
						}
					}
				}
				else // endif (em == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~lem) | (*s++ & lem);
							*d++ = (*d & ~lem) | (*s++ & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{	
							lp0 = *s++;
							lp1 = *s++;
							for (i = words; i > 0; i--)
							{
								*d++ = lp0;
								*d++ = lp1;
							}
							*d++ = (*d & ~lem) | (lp0 & lem);
							*d++ = (*d & ~lem) | (lp1 & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data;
							d += bypl;
						}
					}
				}
			}
			else // (sm == 0xffff)
			{
				if ((short)lem == 0xffffffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~lsm) | (*s++ & lsm);
							*d++ = (*d & ~lsm) | (*s++ & lsm);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							lp0 = *s++;
							lp1 = *s++;
							*d++ = (*d & ~lsm) | (lp0 & lsm);
							*d++ = (*d & ~lsm) | (lp1 & lsm);
							for (i = words; i > 0; i--)
							{
								*d++ = lp0;
								*d++ = lp1;
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data;
							d += bypl;
						}
					}
				}
				else // Got both start and endmasks
				{
					if (!words)
					{
						lsm &= lem;
						for (; height > 0; height--)
						{
							*d++ = (*d & ~lsm) | (*s++ & lsm);
							*d++ = (*d & ~lsm) | (*s++ & lsm);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							lp0 = *s++;
							lp1 = *s++;
							*d++ = (*d & ~lsm) | (lp0 & lsm);
							*d++ = (*d & ~lsm) | (lp1 & lsm);
							for (i = words - 1; i > 0; i--)
							{
								*d++ = lp0;
								*d++ = lp1;
							}
							*d++ = (*d & ~lem) | (lp0 & lem);
							*d++ = (*d & ~lem) | (lp1 & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data;
							d += bypl;
						}
					}
				}
			}
			break;
		}
		case 1: /* trans */
		{
			unsigned long mask, tmp;

			if (lsm == 0xffffffff)
			{
				if (lem == 0xffffffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = (*d & ~mask) | (*s++ & mask);
							*d++ = (*d & ~mask) | (*s++ & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							lp0 = *s++ & mask;
							lp1 = *s++ & mask;
							for (i = words + 1; i > 0; i--)
							{
								*d++ = (*d & ~mask) | lp0;
								*d++ = (*d & ~mask) | lp1;
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
				else // endif (em == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							mask &= lem;
							*d++ = (*d & ~mask) | (*s++ & mask);
							*d++ = (*d & ~mask) | (*s++ & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{	
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							lp0 = *s++ & mask;
							lp1 = *s++ & mask;
							for (i = words; i > 0; i--)
							{
								*d++ = (*d & ~mask ) | lp0;
								*d++ = (*d & ~mask ) | lp1;
							}
							mask &= lem;
							*d++ = (*d & ~mask) | (lp0 & mask);
							*d++ = (*d & ~mask) | (lp1 & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
			}
			else // (sm == 0xffff)
			{
				if ((short)lem == 0xffffffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							mask &= lsm;
							*d++ = (*d & ~mask) | (*s++ & mask);
							*d++ = (*d & ~mask) | (*s++ & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							lp0 = *s++ & mask;
							lp1 = *s++ & mask;
							tmp = mask & lsm;
							*d++ = (*d & ~tmp) | (lp0 & tmp);
							*d++ = (*d & ~tmp) | (lp1 & tmp);
							for (i = words; i > 0; i--)
							{
								*d++ = (*d & ~mask) | lp0;
								*d++ = (*d & ~mask) | lp1;
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
				else // Got both start and endmasks
				{
					if (!words)
					{
						lsm &= lem;
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							mask &= lsm;
							*d++ = (*d & ~mask) | (*s++ & mask);
							*d++ = (*d & ~mask) | (*s++ & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							lp0 = *s++ & mask;
							lp1 = *s++ & mask;
							tmp = mask & lsm;
							*d++ = (*d & ~tmp) | (lp0 & tmp);
							*d++ = (*d & ~tmp) | (lp1 & tmp);
							for (i = words - 1; i > 0; i--)
							{
								*d++ = (*d & ~mask) | lp0;
								*d++ = (*d & ~mask) | lp1;
							}
							tmp = mask & lem;
							*d++ = (*d & ~tmp) | (lp0 & tmp);
							*d++ = (*d & ~tmp) | (lp1 & tmp);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
			}
			break;
		}
		case 2: /* eor */
		{
			//unsigned long mask, tmp;
			unsigned long mask = 0xffffffffUL;

			if (lsm == 0xffffffff)
			{
				if (lem == 0xffffffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							//(unsigned short)mask *m++, mask = (mask << 16) | mask;
							*d++ ^= mask;
							*d++ ^= mask;
#if 0
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
#endif
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							//(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							for (i = words + 1; i > 0; i--)
							{
								*d++ ^= mask;
								*d++ ^= mask;
							}
#if 0
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
#endif
							d += bypl;
						}
					}
				}
				else // endif (em == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							//(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = (*d & ~lem) | ((*d ^ mask) & lem);
							*d++ = (*d & ~lem) | ((*d ^ mask) & lem);
#if 0
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
#endif
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{	
							//(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							for (i = words; i > 0; i--)
							{
								*d++ ^= mask;
								*d++ ^= mask;
							}
							*d++ = (*d & ~lem) | ((*d ^ mask) & lem);
							*d++ = (*d & ~lem) | ((*d ^ mask) & lem);
#if 0
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
#endif
							d += bypl;
						}
					}
				}
			}
			else // (sm == 0xffff)
			{
				if ((short)lem == 0xffffffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							//(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
#if 0
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
#endif
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							//(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							for (i = words; i > 0; i--)
							{
								*d++ ^= mask;
								*d++ ^= mask;
							}
#if 0
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
#endif
							d += bypl;
						}
					}
				}
				else // Got both start and endmasks
				{
					if (!words)
					{
						lsm &= lem;
						for (; height > 0; height--)
						{
							//(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
#if 0
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
#endif
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							//(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							for (i = words - 1; i > 0; i--)
							{
								*d++ ^= mask;
								*d++ ^= mask;
							}
							*d++ = (*d & ~lem) | ((*d ^ mask) & lem);
							*d++ = (*d & ~lem) | ((*d ^ mask) & lem);
#if 0
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
#endif
							d += bypl;
						}
					}
				}
			}
			break;
		}
		case 3: /* erase */
		{
			unsigned long mask, tmp;
			lp0 = 0xffffffff;
			lp1 = lp0;

			if (lsm == 0xffffffff)
			{
				if (lem == 0xffffffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							//lp0 = *s++;
							//lp1 = *s++;
							for (i = words + 1; i > 0; i--)
							{
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
				else // endif (em == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = ((*d & mask) & ~lem) | (((*d ^ lp0) & ~mask) & lem);
							*d++ = ((*d & mask) & ~lem) | (((*d ^ lp0) & ~mask) & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{	
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							//lp0 = *s++;
							//lp1 = *s++;
							for (i = words; i > 0; i--)
							{
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							}
							*d++ = ((*d & mask) & ~lem) | (((*d ^ lp0) & ~mask) & lem);
							*d++ = ((*d & mask) & ~lem) | (((*d ^ lp0) & ~mask) & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
			}
			else // (sm == 0xffff)
			{
				if ((short)lem == 0xffffffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							//lp0 = *s++;
							//lp1 = *s++;
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							for (i = words; i > 0; i--)
							{
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
				else // Got both start and endmasks
				{
					if (!words)
					{
						lsm &= lem;
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							(unsigned short)mask = *m++, mask = (mask << 16) | mask;
							//lp0 = *s++;
							//lp1 = *s++;
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							for (i = words - 1; i > 0; i--)
							{
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							}
							*d++ = ((*d & mask) & ~lem) | (((*d ^ lp0) & ~mask) & lem);
							*d++ = ((*d & mask) & ~lem) | (((*d ^ lp0) & ~mask) & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned long *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
			}
			break;
		} /* case (wrmode) */
	}

	if (!ptrn->expanded)
	{
		ptrn->exp_data = 0;
		ptrn->mask = 0;
	}
}
