#ifndef	_V_TEXT_H
#define _V_TEXT_H


#include "ovdi_defs.h"
void	vst_load_fonts	( VDIPB *pb, VIRTUAL *v );
void	vst_unload_fonts( VDIPB *pb, VIRTUAL *v );
void	vst_alignment	( VDIPB *pb, VIRTUAL *v );
void	vst_color	( VDIPB *pb, VIRTUAL *v );
void	vst_effects	( VDIPB *pb, VIRTUAL *v );
void	vst_font	( VDIPB *pb, VIRTUAL *v );
void	vst_point	( VDIPB *pb, VIRTUAL *v );
void	vst_height	( VDIPB *pb, VIRTUAL *v );
void	vst_rotation	( VDIPB *pb, VIRTUAL *v );

void	v_gtext		( VDIPB *pb, VIRTUAL *v );

void	vqt_attributes	( VDIPB *pb, VIRTUAL *v );
void	vqt_extent	( VDIPB *pb, VIRTUAL *v );
void	vqt_fontinfo	( VDIPB *pb, VIRTUAL *v );
void	vqt_name	( VDIPB *pb, VIRTUAL *v );
void	vqt_width	( VDIPB *pb, VIRTUAL *v );
void	vqt_xfntinfo	( VDIPB *pb, VIRTUAL *v );
void	vst_name	( VDIPB *pb, VIRTUAL *v );
void	vst_width	( VDIPB *pb, VIRTUAL *v );

O_Int	lvst_load_fonts	( VIRTUAL *v );
void	lvst_alignment	( VIRTUAL *v, O_Int halign, O_Int valign );
void	lvst_color	( VIRTUAL *v, O_Int color );
void	lvst_effects	( VIRTUAL *v, O_Int style);
O_Int	lvst_font	( VIRTUAL *v, O_Int id );
O_Int	lvst_point	( VIRTUAL *v, O_Int point );
O_Int	lvst_height	( VIRTUAL *v, O_Int height );
O_Int	lvst_width	( VIRTUAL *v, O_Int width );
void	lvst_rotation	( VIRTUAL *v, O_Int angle );
void	lvst_wrmode	( VIRTUAL *v, O_Int wrmode );
void	lvst_exit	( VIRTUAL *v);

#endif /* _V_TEXT_H */
