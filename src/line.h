#ifndef _LINE_H
#define _LINE_H

#include "ovdi_types.h"
#include "vdi_defs.h"
#include "ovdi_defs.h"

O_Int code (POINT *input, VDIRECT *clip);
O_Int clip_line (VDIRECT *input, VDIRECT *clip);

void abline		( RASTER *r, COLINF *c, VDIRECT *pnts, PatAttr *ptrn);
void habline		( RASTER *r, COLINF *c, O_Pos x1, O_Pos x2, O_Pos y, PatAttr *ptrn);
void vabline		( RASTER *r, COLINF *c, O_Pos y1, O_Pos y2, O_Pos x, PatAttr *ptrn);
void wide_line		( RASTER *r, COLINF *c, O_Pos *pts, long n, VDIRECT *clip, O_Pos *points, long pointasize, PatAttr *ptrn);
void pline		( RASTER *r, COLINF *c, O_Pos *pts, long n, VDIRECT *clip, O_Pos *points, long pointasize, PatAttr *ptrn);
void pmarker		( RASTER *r, COLINF *c, POINT *origin, VDIRECT *clip, O_Int type, O_Int size, O_Int w_in, O_Int h_in, PatAttr *ptrn);
void draw_spans		( RASTER *r, COLINF *c, O_Pos x1, O_Pos x2, O_Pos y, PatAttr *ptrn);
void draw_mspans	( RASTER *r, COLINF *c, O_Pos x1, O_Pos x2, O_Pos y1, O_Pos y2, PatAttr *ptrn);

O_Int wide_setup	( RASTER *r, O_Int width, O_Pos *q_circle);
void quad_xform		( O_Int quad, O_Pos x, O_Pos y, O_Pos *tx, O_Pos *ty);
void perp_off		( O_Pos *vx, O_Pos *vy, O_Pos *q_circle, O_Int num_qc_lines);
void arrow		( RASTER *r, COLINF *c, O_Pos *xy, O_Int inc, O_Int n, VDIRECT *clip, O_Pos *points, long pointasize, PatAttr *ptrn);
void do_arrow		( RASTER *r, COLINF *c, O_Pos *pts, O_Int n, VDIRECT *clip, O_Pos *points, long pointasize, PatAttr *ptrn);

#endif	/* _LINE_H */

