#include <mintbind.h>
#include <osbind.h>
#include "memory.h"

extern short MiNT;

long
omalloc(unsigned long amount, unsigned short type)
{
	long start;

	if (MiNT)
		start = (long)Mxalloc( amount, type);
	else
		start = (long)Malloc( amount );

	return start;
}

void
free_mem(void *loc)
{
	Mfree(loc);
}
