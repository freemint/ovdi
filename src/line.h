#ifndef _LINE_H
#define _LINE_H

#include "vdi_defs.h"
#include "ovdi_defs.h"

short code (POINT *input, VDIRECT *clip);
short clip_line (VDIRECT *input, VDIRECT *clip);

void abline		( RASTER *r, COLINF *c, VDIRECT *pnts, PatAttr *ptrn);
void habline		( RASTER *r, COLINF *c, short x1, short x2, short y, PatAttr *ptrn);
void vabline		( RASTER *r, COLINF *c, short y1, short y2, short x, PatAttr *ptrn);
void wide_line		( RASTER *r, COLINF *c, short *pts, long n, VDIRECT *clip, short *points, long pointasize, LINE_ATTRIBS *latr, PatAttr *ptrn);
void pline		( RASTER *r, COLINF *c, short *pts, long n, VDIRECT *clip, short *points, long pointasize, LINE_ATTRIBS *latr, PatAttr *ptrn);
void pmarker		( RASTER *v, COLINF *c, POINT *origin, VDIRECT *clip, short type, short size, short w_in, short h_in, PatAttr *ptrn);
void draw_spans		( RASTER *r, COLINF *c, short x1, short x2, short y, PatAttr *ptrn);
void draw_mspans	( RASTER *r, COLINF *c, short x1, short x2, short y1, short y2, PatAttr *ptrn);

short wide_setup	( RASTER *r, short width, short *q_circle);
void quad_xform		( short quad, short x, short y, short *tx, short *ty);
void perp_off		( short *vx, short *vy, short *q_circle, short num_qc_lines);
void arrow		( RASTER *r, COLINF *c, short *xy, short inc, short n, VDIRECT *clip, short *points, long pointasize, LINE_ATTRIBS *latr, PatAttr *ptrn);
void do_arrow		( RASTER *r, COLINF *c, short *pts, short n, VDIRECT *clip, short *points, long pointasize, LINE_ATTRIBS *latr, PatAttr *ptrn);

#endif	/* _LINE_H */

