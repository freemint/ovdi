#include "ovdi_defs.h"

#include "1b_generic.h"
static short fillbuff[16];
static short maskbuff[16];

void
fill_16x_1b(RASTER *r, COLINF *c, short *corners, PatAttr *ptrn)
{
	int i, words, bypl, height, y, wrmode, interior;
	unsigned short lp0, lsm, lem;
	unsigned short *m;
	unsigned short *s, *d;

	/*
	 * check if pattern is expanded and do expand it if it isnt
	*/
	wrmode = ptrn->wrmode;
	interior = ptrn->interior;

	if (ptrn->expanded != 1)
	{
		unsigned short data, p0;
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
			(long)ptrn->exp_data = (long)&fillbuff;
			(long)ptrn->mask = (long)&maskbuff;
			ptrn->expanded = 0;
		}
		else
			ptrn->expanded = r->planes;
			
		s = (unsigned short *)ptrn->data;
		m = (unsigned short *)ptrn->mask;
		d = (unsigned short *)ptrn->exp_data;

		if (interior == FIS_HOLLOW)
		{
			fc = ptrn->bgcol[wrmode];
			bc = ptrn->bgcol[wrmode];
		}
		else
		{
			fc = ptrn->color[wrmode];
			bc = ptrn->bgcol[wrmode];
		}
		fc &= 1, bc &= 1;
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
	bypl = r->bypl >> 1;
	words = (((corners[0] & 0xf) + (corners[2] - corners[0]) + 16) >> 4) - 1;
	height = corners[3] - corners[1] + 1;

	lsm = 0xffff >> (corners[0] & 0xf);
	lem = 0xffff << (15 - (corners[2] & 0xf));

	if (interior == FIS_HOLLOW)
		wrmode = 0;
		
	if (ptrn->height == 1)
		y = 0;
	else
		y = corners[1] % ptrn->height;

	s = (unsigned short *)ptrn->exp_data + ((long)y);
	m = (unsigned short *)ptrn->mask + y;
	d = (unsigned short *)r->base + (long)((corners[0] >> 4)) + ((long)corners[1] * bypl);
	bypl -= (words + 1);

	SYNC_RASTER(r);

	switch (wrmode)
	{
		case 0:
		{
			if (lsm == 0xffff)
			{
				if (lem == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							*d++ = *s++;
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							lp0 = *s++;
							for (i = words + 1; i > 0; i--)
								*d++ = lp0;
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data;
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
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{	
							lp0 = *s++;
							for (i = words; i > 0; i--)
							{
								*d++ = lp0;
							}
							*d++ = (*d & ~lem) | (lp0 & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data;
							d += bypl;
						}
					}
				}
			}
			else // (sm == 0xffff)
			{
				if (lem == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~lsm) | (*s++ & lsm);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							lp0 = *s++;
							*d++ = (*d & ~lsm) | (lp0 & lsm);
							for (i = words; i > 0; i--)
							{
								*d++ = lp0;
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data;
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
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							lp0 = *s++;
							*d++ = (*d & ~lsm) | (lp0 & lsm);
							for (i = words - 1; i > 0; i--)
							{
								*d++ = lp0;
							}
							*d++ = (*d & ~lem) | (lp0 & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data;
							d += bypl;
						}
					}
				}
			}
			break;
		}
		case 1: /* trans */
		{
			unsigned short mask, tmp;

			if (lsm == 0xffff)
			{
				if (lem == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							*d++ = (*d & ~mask) | (*s++ & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							lp0 = *s++ & mask;
							for (i = words + 1; i > 0; i--)
							{
								*d++ = (*d & ~mask) | lp0;
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
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
							mask = *m++;
							mask &= lem;
							*d++ = (*d & ~mask) | (*s++ & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{	
							mask = *m++;
							lp0 = *s++ & mask;
							for (i = words; i > 0; i--)
							{
								*d++ = (*d & ~mask ) | lp0;
							}
							mask &= lem;
							*d++ = (*d & ~mask) | (lp0 & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
			}
			else // (sm == 0xffff)
			{
				if (lem == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							mask &= lsm;
							*d++ = (*d & ~mask) | (*s++ & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							lp0 = *s++ & mask;
							tmp = mask & lsm;
							*d++ = (*d & ~tmp) | (lp0 & tmp);
							for (i = words; i > 0; i--)
							{
								*d++ = (*d & ~mask) | lp0;
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
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
							mask = *m++;
							mask &= lsm;
							*d++ = (*d & ~mask) | (*s++ & mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							lp0 = *s++ & mask;
							tmp = mask & lsm;
							*d++ = (*d & ~tmp) | (lp0 & tmp);
							for (i = words - 1; i > 0; i--)
							{
								*d++ = (*d & ~mask) | lp0;
							}
							tmp = mask & lem;
							*d++ = (*d & ~tmp) | (lp0 & tmp);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
			}
			break;
		}
		case 2: /* eor */
		{
			unsigned short mask = 0xffff;

			if (lsm == 0xffff)
			{
				if (lem == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							*d++ ^= mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							for (i = words + 1; i > 0; i--)
							{
								*d++ ^= mask;
							}
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
							*d++ = (*d & ~lem) | ((*d ^ mask) & lem);
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{	
							for (i = words; i > 0; i--)
							{
								*d++ ^= mask;
							}
							*d++ = (*d & ~lem) | ((*d ^ mask) & lem);
							d += bypl;
						}
					}
				}
			}
			else // (sm == 0xffff)
			{
				if (lem == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							for (i = words; i > 0; i--)
							{
								*d++ ^= mask;
							}
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
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~lsm) | ((*d ^ mask) & lsm);
							for (i = words - 1; i > 0; i--)
							{
								*d++ ^= mask;
							}
							*d++ = (*d & ~lem) | ((*d ^ mask) & lem);
							d += bypl;
						}
					}
				}
			}
			break;
		}
		case 3: /* erase */
		{
			unsigned short mask;
			lp0 = 0xffff;

			if (lsm == 0xffff)
			{
				if (lem == 0xffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							for (i = words + 1; i > 0; i--)
							{
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
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
							mask = *m++;
							*d++ = ((*d & mask) & ~lem) | (((*d ^ lp0) & ~mask) & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{	
							mask = *m++;
							for (i = words; i > 0; i--)
							{
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							}
							*d++ = ((*d & mask) & ~lem) | (((*d ^ lp0) & ~mask) & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
				}
			}
			else // (sm == 0xffff)
			{
				if (lem == 0xffffffff)
				{
					if (!words)
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							for (i = words; i > 0; i--)
							{
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							}
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
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
							mask = *m++;
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
							d += bypl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							mask = *m++;
							*d++ = ((*d & mask) & ~lsm) | (((*d ^ lp0) & ~mask) & lsm);
							for (i = words - 1; i > 0; i--)
							{
								*d++ = (*d & mask) | ((*d ^ lp0) & ~mask);
							}
							*d++ = ((*d & mask) & ~lem) | (((*d ^ lp0) & ~mask) & lem);
							y++;
							if (y >= ptrn->height)
								y = 0, s = (unsigned short *)ptrn->exp_data, m = ptrn->mask;
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
