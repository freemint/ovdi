#ifndef _V_LINE_H
#define _V_LINE_H

#include "ovdi_types.h"
#include "vdi_defs.h"
#include "ovdi_defs.h"

void vsl_color		( VDIPB *pb, VIRTUAL *v );
void vsl_ends		( VDIPB *pb, VIRTUAL *v );
void vsl_type		( VDIPB *pb, VIRTUAL *v );
void vsl_udsty		( VDIPB *pb, VIRTUAL *v );
void vsl_width		( VDIPB *pb, VIRTUAL *v );

void v_pline		( VDIPB *pb, VIRTUAL *v );
void v_set_app_buff	( VDIPB *pb, VIRTUAL *v );

void vql_attributes	( VDIPB *pb, VIRTUAL *v );

void lvsl_initial	( VIRTUAL *v );
void lvsl_color		( VIRTUAL *v, O_Int color );
void lvsl_bgcolor	( VIRTUAL *v, O_Int color );
void lvsl_ends		( VIRTUAL *v, O_Int beg, O_Int end );
void lvsl_type		( VIRTUAL *v, O_Int index );
void lvsl_udsty		( VIRTUAL *v, O_u16 pattern );
void lvsl_width		( VIRTUAL *v, O_Int width );
void lv_set_app_buff	( VIRTUAL *v, void *buff, long size );
void lvsl_wrmode	( VIRTUAL *v, O_Int wrmode );

#endif	/* _V_LINE_H */