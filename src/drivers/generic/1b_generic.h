#ifndef _STDDRV_1B_H
#define _STDDRV_1B_H

#include "ovdi_defs.h"
#include "drawing.h"

extern pixel_blit dpf_1b[]; //draw_pixel dpf_8b[];
extern pixel_blit rt_ops_1b[]; //draw_pixel rt_ops_8b[];
extern raster_blit rops_1b[]; //raster_op rops_8b[];

O_u32 get_pixel_1b		(unsigned char *base, O_Int bypl, O_Pos x, O_Pos y);
void put_pixel_1b		(unsigned char *base, O_Int bpl, O_Pos x, O_Pos y, O_u32 pixel);
//void draw_solid_rect_1b		(RASTER *r, COLINF *c, int *corners, int wrmode, int color);
void fill_16x_1b		(RASTER *r, COLINF *c, O_Pos *corners, PatAttr *ptrn);
void spans_16x_1b		(RASTER *r, COLINF *c, O_Pos *spans, O_Int n, PatAttr *ptrn);

void draw_mousecurs_1b		(register XMFORM *mf, register short x, register short y);
void restore_msave_1b		(XMSAVE *ms);

void ds_REPLACE_1b	(struct fill16x_api *);
void ds_TRANS_1b	(struct fill16x_api *);
void ds_XOR_1b		(struct fill16x_api *);
void ds_ERASE_1b	(struct fill16x_api *);

#endif	/* _STDDRV_8B_H */
