#include <mintbind.h>
#include <osbind.h>
#include "memory.h"

extern short MiNT;

unsigned char *vram_base = 0;
unsigned long vram_size = 0;

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

/*
 * vrmalloc is supposed to return video-ram. If this call succeeds,
 * the returned block is RAM that is accessible by the current
 * hardware's accellerator, etc. For now this always returns 0;
*/
void * _cdecl
vrmalloc(unsigned long amount)
{
	if (vram_size)
	{
		/* alloc here */
		return 0;
	}
	else
		return 0;
}