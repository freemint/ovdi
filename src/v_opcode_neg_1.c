#include "ovdi_defs.h"
#include "v_line.h"

#include "v_opcode_neg_1.h"

#define MIN_NEG1_SUB	0
#define MAX_NEG1_SUB	6

static vdi_function neg_1[] =
{
	0,
	0,
	0,
	0,
	0,
	0,
	v_set_app_buff,
};

void
v_opcode_neg_1(VDIPB *pb, VIRTUAL *v)
{
	int sub = pb->contrl[SUBFUNCTION];
	vdi_function f;

	if (sub < MIN_NEG1_SUB || sub > MAX_NEG1_SUB)
		return;
	else
		f = neg_1[sub];

	if (f)
		(*f)(pb, v);
}
