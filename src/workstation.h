#ifndef	_WORKSTATION_H
#define _WORKSTATION_H

#include "ovdi.h"
#include "vdi_defs.h"

void v_opnwk	( VDIPB *pb, VIRTUAL *v, VIRTUAL *lav, OVDI_HWAPI *hwapi); //OVDI_DEVICE *dev);
void v_clswk	( VDIPB *pb, VIRTUAL *v );
void v_clrwk	( VDIPB *pb, VIRTUAL *v );
void v_updwk	( VDIPB *pb, VIRTUAL *v );
void v_opnvwk	( VDIPB *pb, VIRTUAL *v );
void v_clsvwk	( VDIPB *pb, VIRTUAL *v );

void vq_extnd	( VDIPB *pb, VIRTUAL *v );

void lv_clrwk	( VIRTUAL *v);

void copy_common_virtual_vars(VIRTUAL *new, VIRTUAL *root);
void get_MiNT_info(VIRTUAL *);

#endif	/* _WORKSTATION_H */
