#ifndef _GDF_TEXT_H
#define _GDF_TEXT_H

#include "ovdi_defs.h"
#include "vdi_defs.h"

void output_gdftext( VIRTUAL *v, POINT *xy, short *text, short textlen, short len, short wf, short cf);
void expand_gdf_font( FONT_HEAD *f, MFDB *font, short chr, long *ret);

#endif	/* _GDF_TEXT_H */
