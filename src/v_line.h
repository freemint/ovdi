#ifndef _V_LINE_H
#define _V_LINE_H

#include "vdi_defs.h"
#include "ovdi_defs.h"

void vsl_color		( VDIPB *pb, VIRTUAL *v );
void vsl_ends		( VDIPB *pb, VIRTUAL *v );
void vsl_type		( VDIPB *pb, VIRTUAL *v );
void vsl_udsty		( VDIPB *pb, VIRTUAL *v );
void vsl_width		( VDIPB *pb, VIRTUAL *v );

void v_pline		( VDIPB *pb, VIRTUAL *v );

void vql_attributes	( VDIPB *pb, VIRTUAL *v );

void lvsl_initial	( VIRTUAL *v );
void lvsl_color		( VIRTUAL *v, short color );
void lvsl_bgcolor	( VIRTUAL *v, short color );
void lvsl_ends		( VIRTUAL *v, short beg, short end );
void lvsl_type		( VIRTUAL *v, short index );
void lvsl_udsty		( VIRTUAL *v, short pattern );
void lvsl_width		( VIRTUAL *v, short width );

void lvsl_wrmode	( VIRTUAL *v, short wrmode );

#endif	/* _V_LINE_H */