#ifndef _patattr_h
#define _patattr_h

#include "ovdi_defs.h"

/* Common */
void set_pa_fgcolor		(PatAttr *p, COLINF *c, short color);
void set_pa_bgcolor		(PatAttr *p, COLINF *c, short color);
void set_pa_writemode		(PatAttr *p, short wrm);
/* Line specific */
void set_pa_lineends		(PatAttr *p, short beg, short end);
void set_pa_lineindex		(PatAttr *p, short index);
void set_pa_udline		(PatAttr *p, unsigned short udline);
void set_pa_linewidth		(PatAttr *p, short width);
/* Polymarker specific */
void set_pa_pmrk_lineindex	(PatAttr *p, short index);
void set_pa_pmrk_size		(PatAttr *p, short width, short height);
void set_pa_pmrk_type		(PatAttr *p, short type);

#endif	/* _patattr_h */
