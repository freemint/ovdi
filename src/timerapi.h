#ifndef	_TIME_H
#define _TIME_H

#include "ovdi_types.h"
#include "linea_vars.h"

struct timeapi
{
	struct timeapi	*nxtapi;
	O_32		version;
	char		*sname;
	char		*lname;
	char		*pathname;
	char		*filename;

	O_Int		(*install)(LINEA_VARTAB *la);
	O_Int		(*get_tps)(void);
	O_Int		(*add_timeint)(O_u32 function, O_u32 tics);
	void		(*del_timeint)(O_u32 function);
	O_u32		(*set_user_tim)(O_u32 func);
	O_u32		(*set_next_tim)(O_u32 func);
	void		(*reset_user_tim)(void);
	void		(*reset_next_tim)(void);
	void		(*reset)(void);
	void		(*enable)(void);
	void		(*disable)(void);
	
};
typedef struct timeapi TIMEAPI;

#endif /* _TIME_H */
