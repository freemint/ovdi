#ifndef _V_GLOBL_H
#define _V_GLOBL_H

#include "ovdi_defs.h"
#include "vdi_defs.h"

void	vswr_mode	( VDIPB *pb, VIRTUAL *v);
void	lvswr_mode	( VIRTUAL *v, short mode);

void	vs_clip		( VDIPB *pb, VIRTUAL *v);
void	lvs_clip	( VIRTUAL *v, short flag, VDIRECT *cliprect);

void	vs_color	( VDIPB *pb, VIRTUAL *v);
void	lvs_color	( VIRTUAL *v, short vdipen, RGB_LIST *request);

void	vq_color	( VDIPB *pb, VIRTUAL *v);

void	set_writingmode	( short wrmode, short *ret);

#endif	/* _V_GLOBL_H */
