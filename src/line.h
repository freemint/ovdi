#ifndef _LINE_H
#define _LINE_H

#include "ovdi_types.h"
#include "vdi_defs.h"
#include "ovdi_defs.h"

short _cdecl code (POINT *input, VDIRECT *clip);
short _cdecl clip_line (VDIRECT *input, VDIRECT *clip);

void  _cdecl abline		( RASTER *r, COLINF *c, VDIRECT *pnts, PatAttr *ptrn);
void  _cdecl habline		( RASTER *r, COLINF *c, short x1, short x2, short y, PatAttr *ptrn);
void  _cdecl vabline		( RASTER *r, COLINF *c, short y1, short y2, short x, PatAttr *ptrn);
void  _cdecl wide_line		( RASTER *r, COLINF *c, short *pts, long n, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn);
void  _cdecl pline		( RASTER *r, COLINF *c, short *pts, long n, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn);
void  _cdecl pmarker		( RASTER *r, COLINF *c, POINT *origin, VDIRECT *clip, short type, short size, short w_in, short h_in, PatAttr *ptrn);
void  _cdecl draw_spans		( RASTER *r, COLINF *c, short x1, short x2, short y, PatAttr *ptrn);
void  _cdecl draw_mspans	( RASTER *r, COLINF *c, short x1, short x2, short y1, short y2, PatAttr *ptrn);

// short wide_setup	( RASTER *r, short width, short *q_circle);
// void quad_xform		( short quad, short x, short y, short *tx, short *ty);
// void perp_off		( short *vx, short *vy, short *q_circle, short num_qc_lines);
void arrow		( RASTER *r, COLINF *c, short *xy, short inc, short n, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn);
// void do_arrow		( RASTER *r, COLINF *c, short *pts, short n, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn);

#endif	/* _LINE_H */

