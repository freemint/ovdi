#ifndef _V_FILL_H
#define _V_FILL_H

#include "ovdi_defs.h"

void	lvsf_initial	( VIRTUAL *v );
void	vsf_color	( VDIPB *pb, VIRTUAL *v );
void	vsf_interior	( VDIPB *pb, VIRTUAL *v );
void	vsf_perimeter	( VDIPB *pb, VIRTUAL *v );
void	vsf_style	( VDIPB *pb, VIRTUAL *v );
void	vsf_udpat	( VDIPB *pb, VIRTUAL *v );
void	v_fillarea	( VDIPB *pb, VIRTUAL *v );
void	vr_recfl	( VDIPB *pb, VIRTUAL *v );

void	vqf_attributes	( VDIPB *pb, VIRTUAL *v );

void	lvsf_color	( VIRTUAL *v, O_Int color );
void	lvsf_interior	( VIRTUAL *v, O_Int interior );
void	lvsf_bgcolor	( VIRTUAL *v, O_Int color );
void	lvsf_perimeter	( VIRTUAL *v, O_Int flag );

void	lvsf_wrmode	( VIRTUAL *v, O_Int wrmode );
void	lvsudf_wrmode	( VIRTUAL *v, O_Int wrmode );

void	set_fill_params	( O_Int interior, O_Int style, PatAttr *ptrn, O_16 *iout, O_16 *sout );
void	set_udfill	( VIRTUAL *v, O_Int planes, O_u16 *ud, O_Int width, O_Int height );

#endif 	/* _V_FILL_H */
