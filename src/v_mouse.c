
#include "mouse.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "v_mouse.h"

void
vsc_form( VDIPB *pb, VIRTUAL *v)
{
	lvsc_form(v, 0, (MFORM *)&pb->intin[0]);
	return;
}

void
lvsc_form( VIRTUAL *v, short index, MFORM *mf)
{
	(*v->mouseapi->setnewmform)(v, mf);
	return;
}

void
v_hide_c( VDIPB *pb, VIRTUAL *v)
{
	(*v->mouseapi->hidemcurs)();
	return;
}

void
v_show_c( VDIPB *pb, VIRTUAL *v)
{
	if (pb->intin[0] == 0)
		(*v->mouseapi->showmcurs)(1);
	else
		(*v->mouseapi->showmcurs)(0);
	return;
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

	return;
}

void
vex_butv( VDIPB *pb, VIRTUAL *v)
{
	unsigned long oldvec;
	
	oldvec = (*v->mouseapi->setvector)( MVEC_BUT, (unsigned long)(((unsigned long)pb->contrl[7]<<16) | (unsigned short)pb->contrl[8]));

	pb->contrl[9] = (unsigned short)((unsigned long)oldvec >> 16);
	pb->contrl[10] = (unsigned short)((unsigned long)oldvec & 0xffffUL);
	return;
}

void
vex_curv( VDIPB *pb, VIRTUAL *v)
{
	unsigned long oldvec;
	
	oldvec = (*v->mouseapi->setvector)( MVEC_CUR, (unsigned long)(((unsigned long)pb->contrl[7]<<16) | (unsigned short)pb->contrl[8]));

	pb->contrl[9] = (unsigned short)((unsigned long)oldvec >> 16);
	pb->contrl[10] = (unsigned short)((unsigned long)oldvec & 0xffffUL);
	return;
}
	
void
vex_motv( VDIPB *pb, VIRTUAL *v)
{
	unsigned long oldvec;
	
	oldvec = (*v->mouseapi->setvector)( MVEC_MOV, (unsigned long)(((unsigned long)pb->contrl[7]<<16) | (unsigned short)pb->contrl[8]));

	pb->contrl[9] = (unsigned short)((unsigned long)oldvec >> 16);
	pb->contrl[10] = (unsigned short)((unsigned long)oldvec & 0xffffUL);
	return;
}
