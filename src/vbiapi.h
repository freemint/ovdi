#ifndef _VBI_H
#define _VBI_H

#include "ovdi_types.h"

struct vbiapi
{
	struct vbiapi	*nxtapi;
	O_32		version;
	char		*sname;
	char		*lname;
	char		*filename;
	char		*pathname;
	
	O_Int		(*install)(void);

	O_Int		(*get_tics)(void);
	O_Int		(*add_func)(O_u32 func, O_u32 tics);
	void		(*del_func)(O_u32 func);
	void		(*reset)(void);
	void		(*enable)(void);
	void		(*disable)(void);
};
typedef struct vbiapi VBIAPI;
	
//void init_vbi(OVDI_LIB *l, struct module_desc *ret);

#endif	/* _VBI_H */
