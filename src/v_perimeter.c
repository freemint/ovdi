#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "patattr.h"
#include "v_fill.h"
#include "v_perimeter.h"

/*
 * Permieters are internally represented by a 'line' PatAttr
*/
void
lvsprm_initial( VIRTUAL *v)
{
	PatAttr *prm = &v->perimeter;
	int i;

	prm->exp_data	= (O_u16 *)&v->perimeterdata.edata;
	prm->mask	= (O_u16 *)&v->perimeterdata.mask;

	set_fill_params( FIS_SOLID, 0, prm, &prm->interior, 0);

	prm->t.l.index = -1;
	prm->wrmode = -1;

	for (i = 0; i < 4; i++)
		prm->color[i] = prm->bgcol[i] = -1;
}

void
lvsprm_color( VIRTUAL *v, O_Int color )
{
	set_pa_fgcolor(&v->perimeter, v->colinf, color);
}

void
lvsprm_bgcolor( VIRTUAL *v, O_Int color )
{
	set_pa_bgcolor(&v->perimeter, v->colinf, color);
}

void
lvsprm_wrmode( VIRTUAL *v, O_Int wrmode )
{
	set_pa_writemode(&v->perimeter, wrmode);
}

void
lvsprm_ends( VIRTUAL *v, O_Int begin, O_Int end)
{
	set_pa_lineends(&v->perimeter, begin, end);
}

void
lvsprm_type( VIRTUAL *v, O_Int index)
{
	set_pa_lineindex(&v->perimeter, index);
}

void
lvsprm_udsty( VIRTUAL *v, O_u16 pattern)
{
	set_pa_udline(&v->perimeter, pattern);
}

void
lvsprm_width( VIRTUAL *v, O_Int width)
{
	set_pa_linewidth(&v->perimeter, width);
}
