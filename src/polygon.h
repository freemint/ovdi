#ifndef _POLYGON_H
#define _POLYGON_H

#include "ovdi_defs.h"
#include "ovdi_dev.h"

void filled_poly(RASTER *r, COLINF *c, short *pts, short n, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn);

#endif	/* _POLYGON_H */
