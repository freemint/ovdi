#ifndef _COLORS_H
#define _COLORS_H

#include "ovdi_defs.h"
#include "vdi_defs.h"

void reqrgb_2_actrgb	( char *pixelformat, RGB_LIST *levels, RGB_LIST *requested, RGB_LIST *rgbvalues, unsigned long *pixelvalue);
void get_rgb_relatives	( RGB_LIST *values, RGB_LIST *levels, RGB_LIST *result);
void get_rgb_levels	( char *pixelformat, RGB_LIST *levels);
void get_rgb_bits	( char *pf, RGB_LIST *nbits);
short get_color_bits	( char *pixelformat, unsigned long pixel, short which);
unsigned long calc_pixelvalue( char *pixelformat, RGB_LIST *rgbvalues);

#endif	/* _COLORS_H */