#ifndef _OVDI_H
#define _OVDI_H

#include "ovdi_defs.h"
#ifndef _cdecl
#define _cdecl
#endif

long	oVDI		(VDIPB *pb);
long	ovdi_init	(void);
void	v_nosys		(VDIPB *pb, VIRTUAL *v);

O_Int 	_cdecl  get_cookie	(O_32 tag, O_32 *ret);
O_Int	_cdecl	install_cookie	(O_32 tag, O_32 value);
#endif /* _OVDI_H */
