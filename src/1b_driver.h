#ifndef _STDDRV_1B_H
#define _STDDRV_1B_H

#include "ovdi_defs.h"
#include "ovdi_dev.h"

extern pixel_blit dpf_1b[]; //draw_pixel dpf_8b[];
extern pixel_blit rt_ops_1b[]; //draw_pixel rt_ops_8b[];
extern raster_blit rops_1b[]; //raster_op rops_8b[];

unsigned long get_pixel_1b(unsigned char *base, short bypl, short x, short y);
void put_pixel_1b(unsigned char *base, short bpl, short x, short y, unsigned long pixel);
void draw_solid_rect_1b(RASTER *r, COLINF *c, short *corners, short wrmode, short color);

void draw_mousecurs_1b(register XMFORM *mf, register short x, register short y);
void restore_msave_1b(XMSAVE *ms);

#endif	/* _STDDRV_8B_H */
