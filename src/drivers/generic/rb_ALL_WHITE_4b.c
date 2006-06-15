#include "ovdi_defs.h"


void  _cdecl rb_ALL_WHITE_4b(ROP_PB *rpb);

extern short shifts4b[];

void _cdecl
rb_ALL_WHITE_4b(ROP_PB *rpb)
{
	unsigned long lsm, lem, lp0;
	int i, words, bypl, height, sb;
	unsigned long *d;

	bypl = (rpb->d_bypl >> 2);
	height = rpb->sy2 - rpb->sy1 + 1;
	sb = rpb->dx1 & 0xf;
	lsm = ((long *)shifts4b)[sb];
	lem = ~((long *)shifts4b)[(rpb->dx2 & 0xf) + 1];
	words = ((sb + (rpb->sx2 - rpb->sx1) + 16) >> 4) - 1;
	d = (unsigned long *)rpb->d_addr + ((rpb->dx1 >> 4) << 1) + (long)((long)rpb->dy1 * bypl);
	bypl -= (words + 1) << 1;
	lp0 = 0;

	if (lsm == 0xffffffff)
	{
		if (lem == 0xffffffff)
		{
			if (!words)
			{
				for (; height > 0; height--)
				{
					*d++ = lp0;
					*d++ = lp0;
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					for (i = words + 1; i > 0; i--)
					{
						*d++ = lp0;
						*d++ = lp0;
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
					*d++ &= ~lem; //= (*d & ~lem) | (*s++ & lem);
					*d++ &= ~lem; //= (*d & ~lem) | (*s++ & lem);
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{	
					for (i = words; i > 0; i--)
					{
						*d++ = lp0;
						*d++ = lp0;
					}
					*d++ &= ~lem; //= (*d & ~lem) | (lp0 & lem);
					*d++ &= ~lem; //= (*d & ~lem) | (lp1 & lem);
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
					*d++ &= ~lsm; //= (*d & ~lsm) | (*s++ & lsm);
					*d++ &= ~lsm; //= (*d & ~lsm) | (*s++ & lsm);
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					*d++ &= ~lsm; //= (*d & ~lsm) | (lp0 & lsm);
					*d++ &= ~lsm; //= (*d & ~lsm) | (lp1 & lsm);
					for (i = words; i > 0; i--)
					{
						*d++ = lp0;
						*d++ = lp0;
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
					*d++ &= ~lsm; //= (*d & ~lsm) | (*s++ & lsm);
					*d++ &= ~lsm; //= (*d & ~lsm) | (*s++ & lsm);
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					*d++ &= ~lsm; //= (*d & ~lsm) | (lp0 & lsm);
					*d++ &= ~lsm; //= (*d & ~lsm) | (lp1 & lsm);
					for (i = words - 1; i > 0; i--)
					{
						*d++ = lp0;
						*d++ = lp0;
					}
					*d++ &= ~lem; //= (*d & ~lem) | (lp0 & lem);
					*d++ &= ~lem; //= (*d & ~lem) | (lp1 & lem);
					d += bypl;
				}
			}
		}
	}
}
