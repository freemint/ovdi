#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "v_mouse.h"
#include "mouseapi.h"

void
vsc_form( VDIPB *pb, VIRTUAL *v)
{
	lvsc_form(v, 0, (MFORM *)&pb->intin[0]);
}

void
lvsc_form( VIRTUAL *v, O_Int index, MFORM *mf)
{
	(*v->mouseapi->setnewmform)(mf);
}

void
v_hide_c( VDIPB *pb, VIRTUAL *v)
{
	(*v->mouseapi->hidemcurs)();
}

void
v_show_c( VDIPB *pb, VIRTUAL *v)
{
	if (pb->intin[0] == 0)
		(*v->mouseapi->showmcurs)(1);
	else
		(*v->mouseapi->showmcurs)(0);
}

void
v_dspcur( VDIPB *pb, VIRTUAL *v)
{
	(*v->mouseapi->hidemcurs)();
	(*v->mouseapi->absmovmcurs)(pb->ptsin[0], pb->ptsin[1]);
	(*v->mouseapi->showmcurs)(1);
}

void
v_rmcur( VDIPB *pb, VIRTUAL *v)
{
	(*v->mouseapi->hidemcurs)();
}

void
vq_mouse( VDIPB *pb, VIRTUAL *v)
{
	O_u32 bs;

	bs = (*v->mouseapi->getbutstat)();
	(*v->mouseapi->getmcoords)((O_16 *)&pb->ptsout[0]);

	pb->intout[0] = (O_u16)(bs & 0xffffUL);

	pb->contrl[N_INTOUT] = 1;
	pb->contrl[N_PTSOUT] = 1;
}

void
vex_butv( VDIPB *pb, VIRTUAL *v)
{
	*(O_u32 *)&pb->contrl[9] = (*v->mouseapi->setvector)( MVEC_BUT, *(O_u32 *)&pb->contrl[7]);
}

void
vex_curv( VDIPB *pb, VIRTUAL *v)
{
	*(O_u32 *)&pb->contrl[9] = (*v->mouseapi->setvector)( MVEC_CUR, *(O_u32 *)&pb->contrl[7]);
}
	
void
vex_motv( VDIPB *pb, VIRTUAL *v)
{
	*(O_u32 *)&pb->contrl[9] = (*v->mouseapi->setvector)( MVEC_MOV, *(O_u32 *)&pb->contrl[7]);
}
