#ifndef	_EXPAND_H
#define _EXPAND_H

#include "ovdi_types.h"
#include "ovdi_defs.h"

//void	expand	(long *pixelvalues, int splanes, unsigned short *src, int dplanes, unsigned short *dst, int width, int height );
void	expand	(O_Int width, O_Int height, O_Int splanes, O_Int sfmt, void *src, O_Int dplanes, O_Int dfmt, void *dst, O_16 *dcols, O_u16 *mask );

#endif	/* _EXPAND_H */
