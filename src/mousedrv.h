#ifndef _MOUSEDRV_H
#define _MOUSEDRV_H

#include "mouseapi.h"
#include "ovdi_defs.h"
#include "linea_vars.h"

MOUSEAPI * init_mouse(OVDI_HWAPI *hw, LINEA_VARTAB *la);

#endif /* _MOUSEDRV_H */
