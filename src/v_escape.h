#ifndef _V_ESCAPE_H
#define _V_ESCAPE_H

#include "console.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

void v_enter_cur	( VDIPB *pb, VIRTUAL *v);
void v_exit_cur		( VDIPB *pb, VIRTUAL *v);
void v_curtext		( VDIPB *pb, VIRTUAL *v);
void v_alpha_text	( VDIPB *pb, VIRTUAL *v);
void v_offset		( VDIPB *pb, VIRTUAL *v);
void vs_curaddress	( VDIPB *pb, VIRTUAL *v);
void v_curdown		( VDIPB *pb, VIRTUAL *v);
void v_curhome		( VDIPB *pb, VIRTUAL *v);
void v_curleft		( VDIPB *pb, VIRTUAL *v);
void v_curright		( VDIPB *pb, VIRTUAL *v);
void v_curup		( VDIPB *pb, VIRTUAL *v);
void v_eeol		( VDIPB *pb, VIRTUAL *v);
void v_eeos		( VDIPB *pb, VIRTUAL *v);
void v_rvoff		( VDIPB *pb, VIRTUAL *v);
void v_rvon		( VDIPB *pb, VIRTUAL *v);
void vq_chcells		( VDIPB *pb, VIRTUAL *v);
void vq_curaddress	( VDIPB *pb, VIRTUAL *v);

#endif	/* _V_ESCAPE_H */
