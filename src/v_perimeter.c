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

	prm->exp_data	= (unsigned short *)&v->perimeterdata.edata;
	prm->mask	= (unsigned short *)&v->perimeterdata.mask;

	set_fill_params( FIS_SOLID, 0, prm, &prm->interior, 0);

	prm->t.l.index = -1;
	prm->wrmode = -1;

	for (i = 0; i < 4; i++)
		prm->color[i] = prm->bgcol[i] = -1;
}

void
lvsprm_color( VIRTUAL *v, short color )
{
	set_pa_fgcolor(&v->perimeter, v->colinf, color);
}

void
lvsprm_bgcolor( VIRTUAL *v, short color )
{
	set_pa_bgcolor(&v->perimeter, v->colinf, color);
}

void
lvsprm_wrmode( VIRTUAL *v, short wrmode )
{
	set_pa_writemode(&v->perimeter, wrmode);
}

void
lvsprm_ends( VIRTUAL *v, short begin, short end)
{
	set_pa_lineends(&v->perimeter, begin, end);
}

void
lvsprm_type( VIRTUAL *v, short index)
{
	set_pa_lineindex(&v->perimeter, index);
}

void
lvsprm_udsty( VIRTUAL *v, unsigned short pattern)
{
	set_pa_udline(&v->perimeter, pattern);
}

void
lvsprm_width( VIRTUAL *v, short width)
{
	set_pa_linewidth(&v->perimeter, width);
}
