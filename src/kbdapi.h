#ifndef _OVDI_KBDDRV_H
#define _OVDI_KBDDRV_H

struct kbdapi
{
	struct kbdapi	*nxtapi;
	long		version;
	char		*sname;
	char		*lname;
	short		(*install)	(void);
	short		(*keywaiting)	(void);
	short		(*getkey)	(short *ret_asci, short *ret_scan, long *ret_state);
	void		(*waitkey)	(short *ret_asci, short *ret_scan, long *ret_state);
	long		(*getks)	(void);
};
typedef struct kbdapi KBDAPI;

//void init_keyboard(OVDI_LIB *l, struct module_desc *ret);

#endif	/* _OVDI_KBDDRV_H */
