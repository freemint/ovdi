#ifndef _OVDI_H
#define _OVDI_H

#include "ovdi_defs.h"
#ifndef _cdecl
#define _cdecl
#endif

long	oVDI		(VDIPB *pb);
long	ovdi_init	(void);
void	v_nosys		(VDIPB *pb, VIRTUAL *v);

short 	_cdecl  get_cookie	(long tag, long *ret);
short	_cdecl	install_cookie	(long tag, long value);
#endif /* _OVDI_H */
