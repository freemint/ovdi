#ifndef _OVDI_H
#define _OVDI_H

#include "ovdi_defs.h"

long oVDI(VDIPB *pb);
void ovdi_init(void);
void v_nosys(VDIPB *pb, VIRTUAL *v);

#endif /* _OVDI_H */
