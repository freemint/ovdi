#ifndef _v_perimeter_h
#define _v_perimeter_h

#include "ovdi_defs.h"

void lvsprm_initial	( VIRTUAL *v);
void lvsprm_color	( VIRTUAL *v, short color );
void lvsprm_bgcolor	( VIRTUAL *v, short color );
void lvsprm_wrmode	( VIRTUAL *v, short wrmode );
void lvsprm_ends	( VIRTUAL *v, short begin, short end);
void lvsprm_type	( VIRTUAL *v, short index);
void lvsprm_udsty	( VIRTUAL *v, unsigned short pattern);
void lvsprm_width	( VIRTUAL *v, short width);
void lvsprm_wrmode	( VIRTUAL *b, short wrmode);

#endif	/* _v_perimeter_h */
