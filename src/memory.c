#include <mintbind.h>
#include <osbind.h>
#include "memory.h"

extern int MiNT;

unsigned char *vram_base = 0;
unsigned long vram_size = 0;

void * _cdecl
omalloc(O_u32 amount, O_Int type)
{
	O_32 start;

	if (MiNT)
		start = (O_32)Mxalloc( amount, type);
	else
		start = (O_32)Malloc( amount );

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