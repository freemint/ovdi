#ifndef _VBI_H
#define _VBI_H

#include "ovdi_types.h"

struct vbiapi
{
	struct vbiapi	*nxtapi;
	long		version;
	char		*sname;
	char		*lname;
	char		*filename;
	char		*pathname;
	
	short		(*install)(void);

	short		(*get_tics)(void);
	short		(*add_func)(unsigned long func, unsigned long tics);
	void		(*del_func)(unsigned long func);
	void		(*reset)(void);
	void		(*enable)(void);
	void		(*disable)(void);
};
typedef struct vbiapi VBIAPI;
	
//void init_vbi(OVDI_LIB *l, struct module_desc *ret);

#endif	/* _VBI_H */
