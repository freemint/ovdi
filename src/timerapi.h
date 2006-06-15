#ifndef	_TIME_H
#define _TIME_H

#include "ovdi_types.h"
#include "linea_vars.h"

typedef void (*timefunc)(long arg);

struct timeapi
{
	struct timeapi	*nxtapi;
	long		version;
	char		*sname;
	char		*lname;
	char		*pathname;
	char		*filename;

	short _cdecl		(*install)(LINEA_VARTAB *la);
	short _cdecl		(*get_tps)(void);
	short _cdecl		(*add_timeint)(unsigned long function, unsigned long tics, long arg);
	void _cdecl		(*del_timeint)(unsigned long function, long arg);
	unsigned long _cdecl	(*set_user_tim)(unsigned long func);
	unsigned long _cdecl	(*set_next_tim)(unsigned long func);
	void _cdecl		(*reset_user_tim)(void);
	void _cdecl		(*reset_next_tim)(void);
	void _cdecl		(*reset)(void);
	void _cdecl		(*enable)(void);
	void _cdecl		(*disable)(void);
	
};
typedef struct timeapi TIMEAPI;

#endif /* _TIME_H */
