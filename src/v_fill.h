#ifndef _V_FILL_H
#define _V_FILL_H
void	vsf_color	( VDIPB *pb, VIRTUAL *v );
void	vsf_interior	( VDIPB *pb, VIRTUAL *v );
void	vsf_perimeter	( VDIPB *pb, VIRTUAL *v );
void	vsf_style	( VDIPB *pb, VIRTUAL *v );
void	vsf_udpat	( VDIPB *pb, VIRTUAL *v );
void	v_fillarea	( VDIPB *pb, VIRTUAL *v );
void	vr_recfl	( VDIPB *pb, VIRTUAL *v );

void	vqf_attributes	( VDIPB *pb, VIRTUAL *v );

void	lvsf_color	( VIRTUAL *v, short color );
void	lvsf_bgcolor	( VIRTUAL *v, short color );
void	lvsf_perimeter	( VIRTUAL *v, short flag );
void	lvsprm_color	( VIRTUAL *v, short color );
void	lvsprm_bgcolor	( VIRTUAL *v, short color );

void	lvsf_wrmode	( VIRTUAL *v, short wrmode );
void	lvsprm_wrmode	( VIRTUAL *v, short wrmode );
void	lvsudf_wrmode	( VIRTUAL *v, short wrmode );

void	set_fill_params	( short interior, short style, PatAttr *ptrn, short *iout, short *sout );
void	set_udfill	( VIRTUAL *v, short planes, short *ud, short width, short height );

#endif 	/* _V_FILL_H */
