#ifndef _OVDI_KBDDRV_H
#define _OVDI_KBDDRV_H


struct kbdapi
{
	short		(*keywaiting)(void);
	unsigned long	(*getkey)(void);
	unsigned long 	(*waitkey)(void);
	unsigned long	(*getks)(void);
};
typedef struct kbdapi KBDAPI;

KBDAPI * init_keyboard( VIRTUAL *v);

#endif	/* _OVDI_KBDDRV_H */
