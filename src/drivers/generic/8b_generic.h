#ifndef _STDDRV_8B_H
#define _STDDRV_8B_H

#include "ovdi_defs.h"

//extern pixel_blit dpf_8b[]; //draw_pixel dpf_8b[];
extern pixel_blit rt_ops_8b[]; //draw_pixel rt_ops_8b[];
extern raster_blit rops_8b[]; //raster_op rops_8b[];

unsigned long _cdecl	get_pixel_8b( unsigned char *base, short bypl, short x, short y);
void _cdecl		put_pixel_8b( unsigned char *base, short bpl, short x, short y, unsigned long pixel);
void _cdecl		fill_16x_8b(RASTER *r, COLINF *c, short *corners, PatAttr *ptrn);
void _cdecl		spans_16x_8b(RASTER *r, COLINF *c, short *spans, short n, PatAttr *ptrn);

void _cdecl	draw_mousecurs_8b(register XMFORM *mf, register short x, register short y);
void _cdecl	restore_msave_8b(XMSAVE *ms);

#endif	/* _STDDRV_8B_H */
