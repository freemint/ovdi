#ifndef _STDDRV_4B_H
#define _STDDRV_4B_H

#include "ovdi_defs.h"
#include "drawing.h"

extern pixel_blit dpf_4b[];
extern pixel_blit rt_ops_4b[];
extern raster_blit rops_4b[];

unsigned long _cdecl	get_pixel_4b(unsigned char *base, short bypl, short x, short y);
void _cdecl	put_pixel_4b(unsigned char *base, short bpl, short x, short y, unsigned long pixel);
void _cdecl	fill_16x_4b(RASTER *r, COLINF *c, short *corners, PatAttr *ptrn);
void _cdecl	spans_16x_4b(RASTER *r, COLINF *c, short *spans, short n, PatAttr *ptrn);

void _cdecl draw_mousecurs_4b(register XMFORM *mf, register short x, register short y);
void _cdecl restore_msave_4b(XMSAVE *ms);

void _cdecl ds_REPLACE_4b	(struct fill16x_api *);
void _cdecl ds_TRANS_4b		(struct fill16x_api *);
void _cdecl ds_XOR_4b		(struct fill16x_api *);
void _cdecl ds_ERASE_4b		(struct fill16x_api *);

#endif	/* _STDDRV_4B_H */
