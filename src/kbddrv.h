#ifndef _OVDI_KBDDRV_H
#define _OVDI_KBDDRV_H


struct kbdapi
{
	short		(*keywaiting)	(void);
	short		(*getkey)	(short *ret_asci, short *ret_scan, long *ret_state);
	void		(*waitkey)	(short *ret_asci, short *ret_scan, long *ret_state);
	long		(*getks)	(void);
};
typedef struct kbdapi KBDAPI;

KBDAPI * init_keyboard( VIRTUAL *v);

#endif	/* _OVDI_KBDDRV_H */
