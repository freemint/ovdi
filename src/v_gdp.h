#ifndef _V_GDP_H
#define _V_GDP_H

#include "ovdi_defs.h"
#include "vdi_defs.h"

void v_gdp		( VDIPB *pb, VIRTUAL *v);

void v_bar		( VDIPB *pb, VIRTUAL *v);
void v_arc		( VDIPB *pb, VIRTUAL *v);
void v_pieslice		( VDIPB *pb, VIRTUAL *v);
void v_circle		( VDIPB *pb, VIRTUAL *v);
void v_ellipse		( VDIPB *pb, VIRTUAL *v);
void v_ellarc		( VDIPB *pb, VIRTUAL *v);
void v_ellpie		( VDIPB *pb, VIRTUAL *v);
void v_rbox		( VDIPB *pb, VIRTUAL *v);
void v_rfbox		( VDIPB *pb, VIRTUAL *v);
void v_justified	( VDIPB *pb, VIRTUAL *v);
void v_bez_onoff	( VDIPB *pb, VIRTUAL *v);

#endif	/* _V_GDP_H */
