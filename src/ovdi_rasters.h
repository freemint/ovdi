#ifndef	_OVDI_RASTERS_H
#define _OVDI_RASTERS_H

#include "ovdi_defs.h"

void	init_raster		(OVDI_DRIVER *drv, RASTER *r);
void	raster_reschange	(RASTER *r, COLINF *c);
COLINF * new_colinf		(RASTER *r);
void	init_colinf		(RASTER *r, COLINF *c);
void	clone_colinf		(COLINF *dst, COLINF *src);

void	reschange_devtab	(DEV_TAB *dt, RASTER *r);
void	reschange_inqtab	(INQ_TAB *it, RASTER *r);

#endif	/* _OVDI_RASTERS_H */