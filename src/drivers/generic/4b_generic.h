#ifndef _STDDRV_4B_H
#define _STDDRV_4B_H

#include "ovdi_defs.h"
#include "drawing.h"

extern pixel_blit dpf_4b[];
extern pixel_blit rt_ops_4b[];
extern raster_blit rops_4b[];

O_u32	get_pixel_4b(unsigned char *base, O_Int bypl, O_Pos x, O_Pos y);
void	put_pixel_4b(unsigned char *base, O_Int bpl, O_Pos x, O_Pos y, O_u32 pixel);
void	fill_16x_4b(RASTER *r, COLINF *c, O_Pos *corners, PatAttr *ptrn);
void	spans_16x_4b(RASTER *r, COLINF *c, O_Pos *spans, O_Int n, PatAttr *ptrn);

void draw_mousecurs_4b(register XMFORM *mf, register short x, register short y);
void restore_msave_4b(XMSAVE *ms);

void ds_REPLACE_4b	(struct fill16x_api *);
void ds_TRANS_4b	(struct fill16x_api *);
void ds_XOR_4b		(struct fill16x_api *);
void ds_ERASE_4b	(struct fill16x_api *);

#endif	/* _STDDRV_4B_H */
