#ifndef _POLYGON_H
#define _POLYGON_H

#include "ovdi_types.h"
#include "ovdi_defs.h"

void  _cdecl filled_poly(RASTER *r, COLINF *c, short *pts, short n, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn);

#endif	/* _POLYGON_H */
