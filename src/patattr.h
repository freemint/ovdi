#ifndef _patattr_h
#define _patattr_h

#include "ovdi_defs.h"

/* Common */
void set_pa_fgcolor		(PatAttr *p, COLINF *c, O_Int color);
void set_pa_bgcolor		(PatAttr *p, COLINF *c, O_Int color);
void set_pa_writemode		(PatAttr *p, O_Int wrm);
/* Line specific */
void set_pa_lineends		(PatAttr *p, O_Int beg, O_Int end);
void set_pa_lineindex		(PatAttr *p, O_Int index);
void set_pa_udline		(PatAttr *p, O_u16 udline);
void set_pa_linewidth		(PatAttr *p, O_Int width);
/* Polymarker specific */
void set_pa_pmrk_lineindex	(PatAttr *p, O_Int index);
void set_pa_pmrk_size		(PatAttr *p, O_Int width, O_Int height);
void set_pa_pmrk_type		(PatAttr *p, O_Int type);

#endif	/* _patattr_h */
