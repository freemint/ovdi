/*
 This file contains the time-related stuff.
*/
#include <osbind.h>

#include "timerapi.h"

#include "modinf.h"
#include "ovdi_lib.h"
#include "linea_vars.h"
#include "ovdi_types.h"
#include "../../sys/mint/arch/asm_spl.h"

#define TPS	50
#define	etv_timer	0x400

#define TIME_ACTIVE	1

struct timedrv
{
	volatile O_16	flags;
	LINEA_VARTAB	*la;
	void		(*user_tim)(void);
	void		(*next_tim)(void);
};

void	init	(OVDI_LIB *l, struct module_desc *ret, char *p, char *f);

extern O_u32 old_timeint;
extern void time_interruptw(void);
static void donothing(void);

void time_interrupt(void);

static O_Int install(LINEA_VARTAB *la);
static O_Int get_tics_per_sec(void);
static O_Int add_time_interrupt(O_u32 function, O_u32 tics);
static void delete_time_interrupt(O_u32 func);
static O_u32 set_user_tim(O_u32 func);
static O_u32 set_next_tim(O_u32 func);

static void enable_tint(void);
static void disable_tint(void);
static void reset_time(void);
static void reset_user_tim(void);
static void reset_next_tim(void);

static char sname[] =	"'etv_timer' time device driver";
static char lname[] =	"Time device driver using standard\n" \
			"Atari 'etv_timer' for oVDI";
static char fpath[128] = { "0" };
static char fname[64] = { "0" };

static struct timedrv tdrv;
static struct timeapi tapi =
{
	0,
	0x00000001,
	sname,
	lname,
	fpath,
	fname,
	install,
	get_tics_per_sec,
	add_time_interrupt,
	delete_time_interrupt,
	set_user_tim,
	set_next_tim,
	reset_user_tim,
	reset_next_tim,

	reset_time,
	enable_tint,
	disable_tint,
};

#define MAX_TINTS	10
static O_u32 timeints[] =
{
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
};
/* At the time when this function is called, the linea variable table is not
*  not initialized! So, do not rely on anything in it, and only setup
*  own things.
*/
void
init(OVDI_LIB *l, struct module_desc *ret, char *path, char *file)
{
	struct timeapi *ta = &tapi;

	old_timeint	= 0;
	ret->types	= D_TIM;
	ret->tim	= (void *)ta;
	{
		char *t;

		t = ta->pathname;
		while (*path)
			*t++ = *path++;
		*t = 0;
		t = ta->filename;
		while (*file)
			*t++ = *file++;
		*t = 0;
	}
}

static O_Int
install(LINEA_VARTAB *la)
{
	short sr;
	struct timedrv *td = &tdrv;
	long usp;

	usp = Super(1);
	if (!usp)
		usp = Super(0);
	else
		usp = 0;

	if (old_timeint != 0)
	{
		reset_time();
		return 0;
	}

	td->flags = 0;
	td->la = la;
	
	reset_time();

	sr = spl7();
	old_timeint = (long)Setexc(etv_timer >> 2, time_interruptw);
	spl(sr);

	if (usp)
		Super(usp);

	return 0;
}

static void
enable_tint(void)
{
	struct timedrv *td = &tdrv;

	if (old_timeint)
		td->flags |= TIME_ACTIVE;
	return;
}
static void
disable_tint(void)
{
	struct timedrv *td = &tdrv;

	td->flags &= ~TIME_ACTIVE;
	return;
}

static void
reset_time(void)
{
	int	i;
	O_16	sr;
	O_u32 *tints = timeints;

	sr = spl7();
	disable_tint();
	for (i = 0; i < MAX_TINTS; i++)
	{
		tints[0] = tints[1] = tints[2] = 0;
		tints += 3;
	}
	spl(sr);

	reset_user_tim();
	reset_next_tim();

	return;
}

static O_Int
get_tics_per_sec(void)
{
	return (1000/TPS);
}

/* Main etv_timer routine */
void
time_interrupt(void)
{
	register O_u32 *tints = (O_u32 *)&timeints;
	register void (*func)(void);
	register struct timedrv *td = &tdrv;

	if (!(td->flags & TIME_ACTIVE))
		return;

	while (tints[2])
	{
		if (!tints[0])
		{
			tints[0] = tints[1];
			func = (void (*))tints[2];
			(*func)();
		}
		else
			tints[0]--;

		tints += 3;
	}

 /* Call user_tim and next_tim */
	__asm__ volatile
	("
		movem.l	d0-d7/a1-a6,-(sp)
		move.l	%0,a0
		jsr	(a0)
		movem.l (sp),d0-d7/a1-a6
		move.l	%1,a0
		jsr	(a0)
		movem.l	(sp)+,d0-d7/a1-a6
	"	:
		: "a"(td->user_tim), "a"(td->next_tim)
		: "a0"
	);

	return;
}	

/* Installs a function that is called each timer tick	*/
/* Returns the 'handle' of the function or a negative	*/
/* number on error.					*/
static O_Int
add_time_interrupt(O_u32 function, O_u32 tics)
{
	O_Int i;
	register O_u32 *tints = (O_u32 *)&timeints;

	for (i = 0; i < MAX_TINTS; i++)
	{
		if (!tints[2])
		{
			short sr;

			sr = spl7();
			tints[0] = 0;
			tints[1] = tics;
			tints[2] = function;
			spl(sr);
			return i;
		}
		else if (tints[2] == function)
			return i;

		tints += 3;
	}
	return -1;
}

static void
delete_time_interrupt(O_u32 function)
{
	O_16 sr;
	int i;
	register O_u32 *tints = (O_u32 *)&timeints;

	for (i = 0; i < MAX_TINTS; i++)
	{
		if (tints[2] == function)
		{
			sr = spl7();
			while (i < MAX_TINTS)
			{
				tints[0] = tints[0+3];
				tints[1] = tints[1+3];
				tints[2] = tints[2+3];
				tints += 3;
				i++;
			}
				
			spl(sr);
			break;
		}
		tints += 3;
	}
	return;
}

static void
reset_user_tim(void)
{
	(void)set_user_tim((O_u32)&donothing);
	return;
}
static void
reset_next_tim(void)
{
	(void)set_next_tim((O_u32)&donothing);
	return;
}

static O_u32
set_user_tim(O_u32 function)
{
	O_16 sr;
	register struct timedrv *td = &tdrv;
	O_u32 old;

	sr = spl7();
	old = (O_u32)td->user_tim;
	(O_u32)td->user_tim = function;
	(O_u32)td->la->user_tim = function;
	spl(sr);
	return old;
}

static O_u32
set_next_tim(O_u32 function)
{
	O_16 sr;
	register struct timedrv *td = &tdrv;
	O_u32 old;

	sr = spl7();
	old = (O_u32)td->next_tim;
	(O_u32)td->next_tim = function;
	(O_u32)td->la->next_tim = function;
	spl(sr);
	return old;
}

static void
donothing()
{
	return;
}
