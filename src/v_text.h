#ifndef	_V_TEXT_H
#define _V_TEXT_H

#include "ovdi.h"
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

short	lvst_load_fonts	( VIRTUAL *v );
void	lvst_alignment	( VIRTUAL *v, short halign, short valign );
void	lvst_color	( VIRTUAL *v, short color );
void	lvst_effects	( VIRTUAL *v, short style);
int	lvst_font	( VIRTUAL *v, short id );
int	lvst_point	( VIRTUAL *v, short point );
int	lvst_height	( VIRTUAL *v, short height );
void	lvst_rotation	( VIRTUAL *v, short angle );
void	lvst_wrmode	( VIRTUAL *v, short wrmode );
void	lvst_exit	( VIRTUAL *v);

#endif /* _V_TEXT_H */
