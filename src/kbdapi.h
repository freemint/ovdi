#ifndef _OVDI_KBDDRV_H
#define _OVDI_KBDDRV_H

#include "ovdi_types.h"

struct kbdapi
{
	struct kbdapi	*nxtapi;
	O_32		version;
	char		*sname;
	char		*lname;
	char		*pathname;
	char		*filename;
	O_Int		(*install)	(void);
	O_Int		(*keywaiting)	(void);
	O_Int		(*getkey)	(O_16 *ret_asci, O_16 *ret_scan, O_32 *ret_state);
	void		(*waitkey)	(O_16 *ret_asci, O_16 *ret_scan, O_32 *ret_state);
	O_32		(*getks)	(void);
};
typedef struct kbdapi KBDAPI;

//void init_keyboard(OVDI_LIB *l, struct module_desc *ret);

#endif	/* _OVDI_KBDDRV_H */
