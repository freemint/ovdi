#ifndef _LINE_H
#define _LINE_H

#include "vdi_defs.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
short code (POINT *input, VDIRECT *clip);
short clip_line (VDIRECT *input, VDIRECT *clip);

void abline (VIRTUAL *v, VDIRECT *pnts, PatAttr *ptrn);
void habline (VIRTUAL *v, short x1, short x2, short y, PatAttr *ptrn);
void wide_line (VIRTUAL *v, short *pts, long n, short *points, long pointasize, PatAttr *ptrn);
void pline (VIRTUAL *v, short *pts, long n, short *points, long pointasize, PatAttr *ptrn);
void pmarker (VIRTUAL *v, POINT *origin, short type, short size, short w_in, short h_in, PatAttr *ptrn);
void draw_spans (VIRTUAL *v, short x1, short x2, short y, PatAttr *ptrn);

short wide_setup(VIRTUAL *v, short width, short *q_circle);
void quad_xform(short quad, short x, short y, short *tx, short *ty);
void perp_off(short *vx, short *vy, short *q_circle, short num_qc_lines);
void arrow(VIRTUAL *v, short *xy, short inc, short n, short *points, long pointasize, PatAttr *ptrn);
void do_arrow(VIRTUAL *v, short *pts, short n, short *points, long pointasize, PatAttr *ptrn);

#endif	/* _LINE_H */

