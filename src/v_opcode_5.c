
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "v_escape.h"
#include "v_mouse.h"
#include "v_opcode_5.h"

#define MAX_V5_SUBFUNCTS	210

vdi_function v5_subops[] =
{
	0,
	vq_chcells,
	v_exit_cur,
	v_enter_cur,
	v_curup,
	v_curdown,
	v_curright,
	v_curleft,
	v_curhome,
	v_eeos,

	v_eeol,			/* 10 */
	vs_curaddress,
	v_curtext,
	v_rvon,
	v_rvoff,
	vq_curaddress,
	0,
	0,
	v_dspcur,
	v_rmcur,

	0,			/* 20 */
	0,
	0,
	0,
	0,
	v_alpha_text,
	0,
	0,
	0,
	0,

	0,			/* 30 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 40 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 50 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 60 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 70 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 80 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 90 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 100 */
	v_offset,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 110 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 120 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 130 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 140 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 150 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 160 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 170 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 180 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 190 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,			/* 200 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
			/* 210 */
};

void
v_opcode_5(VDIPB *pb, VIRTUAL *v)
{
	short sub;
	vdi_function func;

	sub = pb->contrl[SUBFUNCTION];

	if (sub < MAX_V5_SUBFUNCTS)
	{
		func = v5_subops[sub];

		if (func)
			(*func)(pb, v);
	}
	return;
}
