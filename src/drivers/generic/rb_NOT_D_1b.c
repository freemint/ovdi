#include "ovdi_defs.h"


void rb_NOT_D_1b(ROP_PB *rpb);

extern short shifts1b[];

void
rb_NOT_D_1b(ROP_PB *rpb)
{
	unsigned short lsm, lem;
	int i, words, bypl, height, sb;
	unsigned short *d;

	bypl = rpb->d_bypl >> 1;
	height = rpb->sy2 - rpb->sy1 + 1;
	sb = rpb->dx1 & 0xf;
	lsm = shifts1b[sb];
	lem = ~shifts1b[(rpb->dx2 & 0xf) + 1];
	words = ((sb + (rpb->sx2 - rpb->sx1) + 16) >> 4) - 1;
	d = (unsigned short *)rpb->d_addr + (rpb->dx1 >> 4) + (long)((long)rpb->dy1 * bypl);
	bypl -= words + 1;

	if (lsm == 0xffff)
	{
		if (lem == 0xffff)
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
		if (lem == 0xffff)
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
