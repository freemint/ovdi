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
	
	short _cdecl	(*install)(void);

	short _cdecl	(*get_tics)(void);
	short _cdecl	(*add_func)(unsigned long func, unsigned long tics, long arg);
	void _cdecl	(*del_func)(unsigned long func, long arg);
	void _cdecl	(*reset)(void);
	void _cdecl	(*enable)(void);
	void _cdecl	(*disable)(void);
};
typedef struct vbiapi VBIAPI;
	
//void init_vbi(OVDI_LIB *l, struct module_desc *ret);

#endif	/* _VBI_H */
