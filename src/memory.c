#include <mintbind.h>
#include <osbind.h>
#include "memory.h"

extern short MiNT;

void * _cdecl
omalloc(unsigned long amount, short type)
{
	long start;

	if (MiNT)
		start = (long)Mxalloc( amount, type);
	else
		start = (long)Malloc( amount );

	return (void *)start;
}

void _cdecl
free_mem(void *loc)
{
	Mfree(loc);
}
	