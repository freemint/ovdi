#ifndef _POLYGON_H
#define _POLYGON_H

#include "ovdi_types.h"
#include "ovdi_defs.h"

void filled_poly(RASTER *r, COLINF *c, O_Pos *pts, O_Int n, VDIRECT *clip, O_Pos *points, long pointasize, PatAttr *ptrn);

#endif	/* _POLYGON_H */
