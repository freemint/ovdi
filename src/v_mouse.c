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
lvsc_form( VIRTUAL *v, short index, MFORM *mf)
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
	unsigned long bs;

	bs = (*v->mouseapi->getbutstat)();
	(*v->mouseapi->getmcoords)((short *)&pb->ptsout[0]);

	pb->intout[0] = (unsigned short)(bs & 0xffffUL);

	pb->contrl[N_INTOUT] = 1;
	pb->contrl[N_PTSOUT] = 1;
}

void
vex_butv( VDIPB *pb, VIRTUAL *v)
{
	unsigned long oldvec;
	
	oldvec = (*v->mouseapi->setvector)( MVEC_BUT, (unsigned long)(((unsigned long)pb->contrl[7]<<16) | (unsigned short)pb->contrl[8]));

	pb->contrl[9] = (unsigned short)((unsigned long)oldvec >> 16);
	pb->contrl[10] = (unsigned short)((unsigned long)oldvec & 0xffffUL);
}

void
vex_curv( VDIPB *pb, VIRTUAL *v)
{
	unsigned long oldvec;
	
	oldvec = (*v->mouseapi->setvector)( MVEC_CUR, (unsigned long)(((unsigned long)pb->contrl[7]<<16) | (unsigned short)pb->contrl[8]));

	pb->contrl[9] = (unsigned short)((unsigned long)oldvec >> 16);
	pb->contrl[10] = (unsigned short)((unsigned long)oldvec & 0xffffUL);
}
	
void
vex_motv( VDIPB *pb, VIRTUAL *v)
{
	unsigned long oldvec;
	
	oldvec = (*v->mouseapi->setvector)( MVEC_MOV, (unsigned long)(((unsigned long)pb->contrl[7]<<16) | (unsigned short)pb->contrl[8]));

	pb->contrl[9] = (unsigned short)((unsigned long)oldvec >> 16);
	pb->contrl[10] = (unsigned short)((unsigned long)oldvec & 0xffffUL);
}
