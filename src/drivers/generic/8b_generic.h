#ifndef _STDDRV_8B_H
#define _STDDRV_8B_H

#include "ovdi_defs.h"

//extern pixel_blit dpf_8b[]; //draw_pixel dpf_8b[];
extern pixel_blit rt_ops_8b[]; //draw_pixel rt_ops_8b[];
extern raster_blit rops_8b[]; //raster_op rops_8b[];

O_u32	get_pixel_8b( unsigned char *base, O_Int bypl, O_Pos x, O_Pos y);
void		put_pixel_8b( unsigned char *base, O_Int bpl, O_Pos x, O_Pos y, O_u32 pixel);
void		fill_16x_8b(RASTER *r, COLINF *c, O_Pos *corners, PatAttr *ptrn);
void		spans_16x_8b(RASTER *r, COLINF *c, O_Pos *spans, O_Int n, PatAttr *ptrn);

void	draw_mousecurs_8b(register XMFORM *mf, register short x, register short y);
void	restore_msave_8b(XMSAVE *ms);

#endif	/* _STDDRV_8B_H */
