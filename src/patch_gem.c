#include "patch_gem.h"

#define	SYSBASE 0x4f2
#define MEMBOT	0x432

static short patched = 0;
void
patch_gem(short planes, short max_x)
{
	if (planes < 15)
		return;

	if (!patched)
	{
		short *ptr;
		short offset;

		ptr = (short *)*(long *)SYSBASE;
		if (ptr[1] < 0x200)
			offset = 0x1ee;
		else
			offset = 0x1e0;

		max_x++;

		ptr = (short *)0x2000;

		while ((long)ptr < *(long *)MEMBOT)
		{
			if (*ptr == 8 && *(short *)((char *)ptr + offset) == max_x)
			{
				*ptr = planes;
				patched = 1;
				return;
			}
			ptr++;
		}
	}
	return;
}

		
		