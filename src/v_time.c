
#include "ovdi_defs.h"
#include "timerapi.h"
#include "vdi_defs.h"
#include "v_time.h"

void
vex_timv( VDIPB *pb, VIRTUAL *v)
{
	unsigned long oldvec;
	
	oldvec = (*v->timeapi->set_user_tim)((unsigned long)(((unsigned long)pb->contrl[7]<<16) | (unsigned short)pb->contrl[8]));
	pb->contrl[9] = (unsigned short)((unsigned long)oldvec >> 16);
	pb->contrl[10] = (unsigned short)((unsigned long)oldvec & 0xffffUL);
	pb->intout[0] = (*v->timeapi->get_tps)();

	pb->contrl[N_INTOUT] = 1;
}
