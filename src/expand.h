#ifndef	_EXPAND_H
#define _EXPAND_H

#include "ovdi_types.h"
#include "ovdi_defs.h"

//void	expand	(long *pixelvalues, int splanes, unsigned short *src, int dplanes, unsigned short *dst, int width, int height );
void	expand	(short width, short height, short splanes, short sfmt, void *src, short dplanes, short dfmt, void *dst, short *dcols, unsigned short *mask );

#endif	/* _EXPAND_H */
