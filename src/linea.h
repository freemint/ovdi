#ifndef	_OVDILINEA_H
#define _OVDILINEA_H


#include "ovdi_defs.h"
#include "mouse.h"

void init_linea_vartab(VIRTUAL *v, LINEA_VARTAB *la);
void linea_reschange(LINEA_VARTAB *la, RASTER *r, COLINF *c);
void set_linea_vector(void);
void get_linea_addresses(long *vt, long *fr, long *ft);

#endif	/* _OVDILINEA_H */
