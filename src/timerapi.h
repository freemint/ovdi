#ifndef	_TIME_H
#define _TIME_H

#include "ovdi_types.h"
#include "linea_vars.h"

struct timeapi
{
	struct timeapi	*nxtapi;
	long		version;
	char		*sname;
	char		*lname;
	char		*pathname;
	char		*filename;

	short		(*install)(LINEA_VARTAB *la);
	short		(*get_tps)(void);
	short		(*add_timeint)(unsigned long function, unsigned long tics);
	void		(*del_timeint)(unsigned long function);
	unsigned long		(*set_user_tim)(unsigned long func);
	unsigned long		(*set_next_tim)(unsigned long func);
	void		(*reset_user_tim)(void);
	void		(*reset_next_tim)(void);
	void		(*reset)(void);
	void		(*enable)(void);
	void		(*disable)(void);
	
};
typedef struct timeapi TIMEAPI;

#endif /* _TIME_H */
