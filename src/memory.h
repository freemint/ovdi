#ifndef	_oMEMORY_C
#define _oMEMORY_C

#include "ovdi_types.h"
#include "libkern.h"

#define	MX_STRAM	0
#define MX_TTRAM	1
#define MX_PREFSTRAM	2
#define MX_PREFTTRAM	3
#define MX_MPSEL	(1 << 3)
#define MX_HEADER	MX_MPSEL | (0 << 4)
#define MX_PRIVATE	MX_MPSEL | (1 << 4)
#define MX_GLOBAL	MX_MPSEL | (2 << 4)
#define MX_SUPER	MX_MPSEL | (3 << 4)
#define MX_READABLE	MX_MPSEL | (4 << 4)

void *	_cdecl	omalloc		(O_u32 amount, O_Int type);
void 	_cdecl	free_mem	(void *loc);
void *	_cdecl	vrmalloc	(O_u32 amount);

#endif	/* _oMEMORY_C */
