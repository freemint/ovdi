#ifndef _V_PMARKER_H
#define _V_PMARKER_H

void	lvsm_initial	( VIRTUAL *v );
void	vsm_color	( VDIPB *pb, VIRTUAL *v );
void	vsm_height	( VDIPB *pb, VIRTUAL *v );
void	vsm_type	( VDIPB *pb, VIRTUAL *v );
void	v_pmarker	( VDIPB *pb, VIRTUAL *v );
void	vqm_attributes	( VDIPB *pb, VIRTUAL *v );

void	lvsm_linetype	( VIRTUAL *v, O_Int linetype );

void	lvsm_color	( VIRTUAL *v, O_Int color );
void	lvsm_bgcolor	( VIRTUAL *v, O_Int color );
void	lvsm_height	( VIRTUAL *v, O_Int height );
void	lvsm_type	( VIRTUAL *v, O_Int type );
void	lvsm_wrmode	( VIRTUAL *v, O_Int wrmode );

#endif	/* _V_PMARKER_H */