#include "ovdi_defs.h"
#include "4b_generic.h"

void
draw_solid_rect_4b(RASTER *r, COLINF *c, short *corners, short wrmode, short color)
{
	int i, words, bypl, height;
	unsigned long lp0, lp1, lsm, lem;
	unsigned short *d;

	bypl = r->bypl >> 1;
	lp0 = (unsigned long)(color & 1 ? 0xffff0000UL : 0) | (color & 2 ? 0xffff : 0);
	lp1 = (unsigned long)(color & 4 ? 0xffff0000UL : 0) | (color & 8 ? 0xffff : 0);

	words = (((corners[0] & 0xf) + (corners[2] - corners[0]) + 16) >> 4) - 1;
	height = corners[3] - corners[1] + 1;
	{
		unsigned short sm, em;

		sm = 0xffff >> (corners[0] & 0xf);
		em = 0xffff << (15 - (corners[2] & 0xf));
		lsm = ((unsigned long)sm << 16) | sm;
		lem = ((unsigned long)em << 16) | em;
	}

	d = (unsigned short *)r->base + (long)((corners[0] >> 4) << 2) + ((long)corners[1] * bypl);
	bypl -= (words + 1) << 2;

	if (lsm == 0xffffffff)
	{
		if (lem == 0xffffffff)
		{
			if (!words)
			{
				for (; height > 0; height--)
				{
					*(long *)((long *)d)++ = lp0;
					*(long *)((long *)d)++ = lp1;
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					for (i = words + 1; i > 0; i--)
					{
						*(long *)((long *)d)++ = lp0;
						*(long *)((long *)d)++ = lp1;
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
					*(long *)((long *)d)++ = (*(long *)d & ~lem) | (lp0 & lem);
					*(long *)((long *)d)++ = (*(long *)d & ~lem) | (lp1 & lem);
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{	
					for (i = words; i > 0; i--)
					{
						*(long *)((long *)d)++ = lp0;
						*(long *)((long *)d)++ = lp1;
					}
					*(long *)((long *)d)++ = (*(long *)d & ~lem) | (lp0 & lem);
					*(long *)((long *)d)++ = (*(long *)d & ~lem) | (lp1 & lem);
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
					*(long *)((long *)d)++ = (*(long *)d & ~lsm) | (lp0 & lsm);
					*(long *)((long *)d)++ = (*(long *)d & ~lsm) | (lp1 & lsm);
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					*(long *)((long *)d)++ = (*(long *)d & ~lsm) | (lp0 & lsm);
					*(long *)((long *)d)++ = (*(long *)d & ~lsm) | (lp1 & lsm);
					for (i = words; i > 0; i--)
					{
						*(long *)((long *)d)++ = lp0;
						*(long *)((long *)d)++ = lp1;
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
					*(long *)((long *)d)++ = (*(long *)d & ~lsm) | (lp0 & lsm);
					*(long *)((long *)d)++ = (*(long *)d & ~lsm) | (lp1 & lsm);
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					*(long *)((long *)d)++ = (*(long *)d & ~lsm) | (lp0 & lsm);
					*(long *)((long *)d)++ = (*(long *)d & ~lsm) | (lp1 & lsm);
					for (i = words - 1; i > 0; i--)
					{
						*(long *)((long *)d)++ = lp0;
						*(long *)((long *)d)++ = lp1;
					}
					*(long *)((long *)d)++ = (*(long *)d & ~lem) | (lp0 & lem);
					*(long *)((long *)d)++ = (*(long *)d & ~lem) | (lp1 & lem);
					d += bypl;
				}
			}
		}
	}
}
