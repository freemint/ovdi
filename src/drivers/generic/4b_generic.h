#ifndef _STDDRV_4B_H
#define _STDDRV_4B_H

#include "ovdi_defs.h"

extern pixel_blit dpf_4b[];
extern pixel_blit rt_ops_4b[];
extern raster_blit rops_4b[];

unsigned long get_pixel_4b(unsigned char *base, short bypl, short x, short y);
void put_pixel_4b(unsigned char *base, short bpl, short x, short y, unsigned long pixel);
void fill_16x_4b(RASTER *r, COLINF *c, short *corners, PatAttr *ptrn);
//void draw_solid_rect_4b(RASTER *r, COLINF *c, short *corners, short wrmode, short color);

void draw_mousecurs_4b(register XMFORM *mf, register short x, register short y);
void restore_msave_4b(XMSAVE *ms);


#endif	/* _STDDRV_4B_H */
