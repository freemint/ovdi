#include "ovdi_defs.h"


void rb_NOT_D_4b(ROP_PB *rpb);

extern short shifts4b[];

void
rb_NOT_D_4b(ROP_PB *rpb)
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
	d = (unsigned long *)rpb->d_addr + (long)((rpb->dx1 >> 4) << 1) + (long)((long)rpb->dy1 * bypl);
	lp0 = 0;

	if (lsm == 0xffffffff)
	{
		if (lem == 0xffffffff)
		{
			if (!words)
			{
				for (; height > 0; height--)
				{
					*d++ = ~*d;
					*d++ = ~*d;
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					for (i = words + 1; i > 0; i--)
					{
						*d++ = ~*d;
						*d++ = ~*d;
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
					*d++ = (*d & ~lem) | ~(*d & lem);
					*d++ = (*d & ~lem) | ~(*d & lem);
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{	
					for (i = words; i > 0; i--)
					{
						*d++ = ~*d;
						*d++ = ~*d;
					}
					*d++ = (*d & ~lem) | ~(*d & lem);
					*d++ = (*d & ~lem) | ~(*d & lem);
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
					*d++ = (*d & ~lsm) | ~(*d & lsm);
					*d++ = (*d & ~lsm) | ~(*d & lsm);
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					*d++ = (*d & ~lsm) | ~(*d & lsm);
					*d++ = (*d & ~lsm) | ~(*d & lsm);
					for (i = words; i > 0; i--)
					{
						*d++ = ~*d;
						*d++ = ~*d;
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
					*d++ = (*d & ~lsm) | ~(*d & lsm);
					*d++ = (*d & ~lsm) | ~(*d & lsm);
					d += bypl;
				}
			}
			else
			{
				for (; height > 0; height--)
				{
					*d++ = (*d & ~lsm) | ~(*d & lsm);
					*d++ = (*d & ~lsm) | ~(*d & lsm);
					for (i = words - 1; i > 0; i--)
					{
						*d++ = ~*d;
						*d++ = ~*d;
					}
					*d++ = (*d & ~lem) | ~(*d & lem);
					*d++ = (*d & ~lem) | ~(*d & lem);
					d += bypl;
				}
			}
		}
	}
}
