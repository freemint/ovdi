#ifndef _V_PMARKER_H
#define _V_PMARKER_H

void	vsm_color	( VDIPB *pb, VIRTUAL *v );
void	vsm_height	( VDIPB *pb, VIRTUAL *v );
void	vsm_type	( VDIPB *pb, VIRTUAL *v );
void	v_pmarker	( VDIPB *pb, VIRTUAL *v );
void	vqm_attributes	( VDIPB *pb, VIRTUAL *v );

void	lvsm_linetype	( VIRTUAL *v, register short linetype );

void	lvsm_color	( VIRTUAL *v, register short color );
void	lvsm_height	( VIRTUAL *v, register short height );
void	lvsm_type	( VIRTUAL *v, register short type );


#endif	/* _V_PMARKER_H */