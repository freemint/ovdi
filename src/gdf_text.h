#ifndef _GDF_TEXT_H
#define _GDF_TEXT_H

#include "ovdi_types.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

void output_gdftext( VIRTUAL *v, POINT *xy, O_16 *text, O_Int textlen, O_Int len, O_Int wf, O_Int cf);
void expand_gdf_font( FONT_HEAD *f, MFDB *font, O_Int chr);

#endif	/* _GDF_TEXT_H */
