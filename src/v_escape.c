#include "console.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "v_escape.h"




void
v_enter_cur( VDIPB *pb, VIRTUAL *v)
{
	enter_console(v->con);
	return;
}

void
v_exit_cur( VDIPB *pb, VIRTUAL *v)
{
	exit_console(v->con);
	return;
}

void
v_curtext( VDIPB *pb, VIRTUAL *v)
{
	register CONSOLE *c;
	register short count = pb->contrl[N_INTIN];
	register short *string = (short *)&pb->intin[0];

	c = v->con;
	hide_text_cursor(c);

	while(count)
	{
		(*c->csout_char)(*string++);

		if (c->la->v_cur_x < c->la->v_cel_mx)
			c->la->v_cur_x++;

		count--;
	}
	show_text_cursor(c);
	return;
}

void
v_alpha_text( VDIPB *pb, VIRTUAL *v)
{
	return;
}

void
v_offset( VDIPB *pb, VIRTUAL *v)
{
	return;
}

/* Same as VT-52 'esc-Y' code */
void
vs_curaddress( VDIPB *pb, VIRTUAL *v)
{
	move_text_cursor(v->con, pb->intin[1], pb->intin[0]);
	return;
}

/* Same as VT-52 'esc-B' code */
void
v_curdown( VDIPB *pb, VIRTUAL *v)
{
	Esc_B(v->con);
	return;
}

/* Same as VT-52 'esc-H' code */
void
v_curhome( VDIPB *pb, VIRTUAL *v)
{
	Esc_H(v->con);
	return;
}

/* Same as VT-52 'esc-D' code */
void
v_curleft( VDIPB *pb, VIRTUAL *v)
{
	Esc_D(v->con);
	return;
}

/* Same as VT-52 'esc-C' code */
void
v_curright( VDIPB *pb, VIRTUAL *v)
{
	Esc_C(v->con);
	return;
}

/* Same as VT-52 'esc-A' code */
void
v_curup( VDIPB *pb, VIRTUAL *v)
{
	Esc_A(v->con);
	return;
}

/* Same as VT-52 'esc-K' code */
void
v_eeol( VDIPB *pb, VIRTUAL *v)
{
	Esc_K(v->con);
	return;
}

/* Same as VT-52 'esc-J' code */
void
v_eeos( VDIPB *pb, VIRTUAL *v)
{
	Esc_J(v->con);
	return;
}

/* Same as VT-52 'esc-q' code */
void
v_rvoff( VDIPB *pb, VIRTUAL *v)
{
	Esc_q(v->con);
	return;
}

/* Same as VT-52 'esc-p' code */
void
v_rvon( VDIPB *pb, VIRTUAL *v)
{
	Esc_p(v->con);
	return;
}

void
vq_chcells( VDIPB *pb, VIRTUAL *v)
{

	pb->intout[1] = v->con->la->v_cel_mx + 1;
	pb->intout[0] = v->con->la->v_cel_my + 1;

	pb->contrl[N_INTOUT] = 2;
	return;
}

void
vq_curaddress( VDIPB *pb, VIRTUAL *v)
{
	pb->intout[1] = v->con->la->v_cur_x;
	pb->intout[0] = v->con->la->v_cur_y;

	pb->contrl[N_INTOUT] = 2;
	return;
}
