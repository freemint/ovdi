#ifndef	_WORKSTATION_H
#define _WORKSTATION_H

#include "ovdi.h"
#include "vdi_defs.h"
#include "ovdi_dev.h"

void v_opnwk	( VDIPB *pb, VIRTUAL *v, VIRTUAL *lav, OVDI_DEVICE *dev);
void v_clswk	( VDIPB *pb, VIRTUAL *v );
void v_clrwk	( VDIPB *pb, VIRTUAL *v );
void v_updwk	( VDIPB *pb, VIRTUAL *v );
void v_opnvwk	( VDIPB *pb, VIRTUAL *v );
void v_clsvwk	( VDIPB *pb, VIRTUAL *v );

void vq_extnd	( VDIPB *pb, VIRTUAL *v );

void lv_clrwk	( VIRTUAL *v);

#endif	/* _WORKSTATION_H */
