#ifndef _STDDRV_16B_H
#define _STDDRV_16B_H

#include "ovdi_defs.h"

extern pixel_blit dpf_16b[];
extern pixel_blit rt_ops_16b[];
extern raster_blit rops_16b[];


unsigned long get_pixel_16b(unsigned char *base, O_Int bypl, O_Pos x, O_Pos y);
void put_pixel_16b(unsigned char *base, O_Int bypl, O_Pos x, O_Pos y, unsigned long pixel);

void draw_mousecurs_16b(XMFORM *mx, short x, short y);
void restore_msave_16b(XMSAVE *ms);

#endif	/* _STDDRV_16B_H */
