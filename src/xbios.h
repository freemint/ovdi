#ifndef _OVDI_XBIOS_H
#define _OVDI_XBIOS_H

#include "ovdi_types.h"

void install_xbios	(void);
void uninstall_xbios	(void);

void enable_xbios	(OVDI_HWAPI *hw);
void disable_xbios	(void);

#endif	/* _OVDI_XBIOS_H */
