#ifndef _OVDI_H
#define _OVDI_H

#include "ovdi_defs.h"

long	oVDI		(VDIPB *pb);
long	ovdi_init	(void);
void	v_nosys		(VDIPB *pb, VIRTUAL *v);

int 	_cdecl  get_cookie	(long tag, long *ret);

#endif /* _OVDI_H */
