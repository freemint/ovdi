#ifndef _STDDRV_1B_H
#define _STDDRV_1B_H

#include "ovdi_defs.h"
#include "drawing.h"

extern pixel_blit dpf_1b[]; //draw_pixel dpf_8b[];
extern pixel_blit rt_ops_1b[]; //draw_pixel rt_ops_8b[];
extern raster_blit rops_1b[]; //raster_op rops_8b[];

unsigned long _cdecl	get_pixel_1b(unsigned char *base, short bypl, short x, short y);
void _cdecl		put_pixel_1b(unsigned char *base, short bpl, short x, short y, unsigned long pixel);
//void draw_solid_rect_1b		(RASTER *r, COLINF *c, int *corners, int wrmode, int color);
void _cdecl fill_16x_1b		(RASTER *r, COLINF *c, short *corners, PatAttr *ptrn);
void _cdecl spans_16x_1b	(RASTER *r, COLINF *c, short *spans, short n, PatAttr *ptrn);

void _cdecl draw_mousecurs_1b	(register XMFORM *mf, register short x, register short y);
void _cdecl restore_msave_1b	(XMSAVE *ms);

void _cdecl ds_REPLACE_1b	(struct fill16x_api *);
void _cdecl ds_TRANS_1b		(struct fill16x_api *);
void _cdecl ds_XOR_1b		(struct fill16x_api *);
void _cdecl ds_ERASE_1b		(struct fill16x_api *);

#endif	/* _STDDRV_8B_H */
