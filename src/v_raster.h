#ifndef _V_RASTER_H
#define _V_RASTER_H

#include "ovdi_defs.h"
#include "vdi_defs.h"

void vro_cpyfm		( VDIPB *pb, VIRTUAL *v);
void vrt_cpyfm		( VDIPB *pb, VIRTUAL *v);
void vr_trnfm		( VDIPB *pb, VIRTUAL *v);
void v_get_pixel	( VDIPB *pb, VIRTUAL *v);

#endif	/* _V_RASTER_H */
