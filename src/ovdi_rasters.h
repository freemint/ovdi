#ifndef	_OVDI_RASTERS_H
#define _OVDI_RASTERS_H

#include "ovdi_defs.h"

void	init_device_jumptable	(OVDI_HWAPI *hw, OVDI_DRIVER *drv, char *mem);
OVDI_DRIVER * open_device	(OVDI_HWAPI *hw);

void	init_raster		(OVDI_HWAPI *hw, RASTER *r);
RASTER * new_raster		(OVDI_HWAPI *hw, char *base, short x2, short y2, RESFMT *rf);
void	free_raster		(RASTER *r);
void	raster_reschange	(RASTER *r, COLINF *c);
void	init_raster_rgb		(RASTER *r);
COLINF * new_colinf		(char *);
void	init_colinf		(RASTER *r, COLINF *c);
void	clone_colinf		(COLINF *dst, COLINF *src);

void	reschange_devtab	(DEV_TAB *dt, RASTER *r);
void	reschange_inqtab	(INQ_TAB *it, RASTER *r);

#endif	/* _OVDI_RASTERS_H */
