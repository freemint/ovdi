#ifndef _V_MOUSE_H
#define _V_MOUSE_H

#include "mouse.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

void vsc_form	(VDIPB *pb, VIRTUAL *v);
void v_hide_c	(VDIPB *pb, VIRTUAL *v);
void v_show_c	(VDIPB *pb, VIRTUAL *v);
void vq_mouse	(VDIPB *pb, VIRTUAL *v);
void v_dspcur	(VDIPB *pb, VIRTUAL *v);
void v_rmcur	(VDIPB *pb, VIRTUAL *v);

void vex_butv	(VDIPB *pb, VIRTUAL *v);
void vex_curv	(VDIPB *pb, VIRTUAL *v);
void vex_motv	(VDIPB *pb, VIRTUAL *v);
void vex_timv	(VDIPB *pb, VIRTUAL *v);

void lvsc_form	(VIRTUAL *v, O_Int index, MFORM *mf);


#endif	/* _V_MOUSE_H */
