#ifndef _v_perimeter_h
#define _v_perimeter_h

#include "ovdi_defs.h"

void lvsprm_initial	( VIRTUAL *v);
void lvsprm_color	( VIRTUAL *v, O_Int color );
void lvsprm_bgcolor	( VIRTUAL *v, O_Int color );
void lvsprm_wrmode	( VIRTUAL *v, O_Int wrmode );
void lvsprm_ends	( VIRTUAL *v, O_Int begin, O_Int end);
void lvsprm_type	( VIRTUAL *v, O_Int index);
void lvsprm_udsty	( VIRTUAL *v, O_u16 pattern);
void lvsprm_width	( VIRTUAL *v, O_Int width);
void lvsprm_wrmode	( VIRTUAL *b, O_Int wrmode);

#endif	/* _v_perimeter_h */