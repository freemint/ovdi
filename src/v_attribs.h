#ifndef _V_GLOBL_H
#define _V_GLOBL_H

#include "ovdi_defs.h"
#include "vdi_defs.h"

void	vswr_mode	( VDIPB *pb, VIRTUAL *v);
void	lvswr_mode	( VIRTUAL *v, O_Int mode);

void	vs_clip		( VDIPB *pb, VIRTUAL *v);
void	lvs_clip	( VIRTUAL *v, O_Int flag, O_16 *cliprect);

void	vs_color	( VDIPB *pb, VIRTUAL *v);
void	lvs_color	( VIRTUAL *v, O_Int hwpen, RGB_LIST *color);
O_Int	calc_vdicolor	( RASTER *r, COLINF *c, O_Int vdipen, RGB_LIST *color);
void	vq_color	( VDIPB *pb, VIRTUAL *v);

void	set_writingmode	( O_Int wrmode, O_16 *ret);

#endif	/* _V_GLOBL_H */
