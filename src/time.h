#ifndef	_TIME_H
#define _TIME_H

#include "ovdi_defs.h"
#include "linea.h"

struct timedrv
{
	LINEA_VARTAB *la;
	void (*user_tim)(void);
	void (*next_tim)(void);
};

struct timeapi
{
	short		(*get_tps)(void);
	short		(*add_timeint)(unsigned long function, unsigned long tics);
	void		(*removetimeint)(unsigned long function);
	unsigned long	(*set_user_tim)(unsigned long func);
	unsigned long	(*set_next_tim)(unsigned long func);
};

struct timeapi * init_time( VIRTUAL *v, LINEA_VARTAB *la);

#endif /* _TIME_H */
