#ifndef	_OVDI_RASTERS_H
#define _OVDI_RASTERS_H

#include "ovdi_defs.h"

void	init_raster		(OVDI_DRIVER *drv, RASTER *r);
void	raster_reschange	(RASTER *r, COLINF *c);
COLINF * new_colinf		(RASTER *r);
void	init_colinf		(RASTER *r, COLINF *c);

#endif	/* _OVDI_RASTERS_H */