#ifndef	_EXPAND_H
#define _EXPAND_H

#include "ovdi_defs.h"

//void	expand	(long *pixelvalues, short splanes, unsigned short *src, short dplanes, unsigned short *dst, short width, short height );
void	expand	(short width, short height, short splanes, short sfmt, void *src, short dplanes, short dfmt, void *dst, short *dcols, short *mask );

#endif	/* _EXPAND_H */
