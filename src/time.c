/*
 This file contains the time-related stuff.
*/
#include <osbind.h>

#include "display.h"
#include "linea.h"
#include "ovdi_defs.h"
#include "ovdi_types.h"
#include "time.h"
#include "vdi_defs.h"
#include "../../sys/mint/arch/asm_spl.h"

#define TPS	50
#define	etv_timer	0x400

extern unsigned long old_timeint;
extern void time_interruptw(void);
static void donothing(void);

void time_interrupt(void);

static short get_tics_per_sec(void);
static short add_time_interrupt(unsigned long function, unsigned long tics);
static void delete_time_interrupt(unsigned long func);
static unsigned long set_user_tim(unsigned long func);
static unsigned long set_next_tim(unsigned long func);

static void enable_tint(void);
static void disable_tint(void);
static void reset_time(void);
static void reset_user_tim(void);
static void reset_next_tim(void);

struct timedrv tdrv;
struct timeapi tapi;

#define MAX_TINTS	10
unsigned long timeints[] =
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
struct timeapi *
init_time(LINEA_VARTAB *la)
{
	short sr;
	struct timedrv *td = &tdrv;
	struct timeapi *ta = &tapi;
	long usp;

	usp = Super(1);
	if (!usp)
		usp = Super(0);
	else
		usp = 0;

	if (old_timeint != 0)
	{
		reset_time();
		return ta;
	}

	td->flags = 0;
	td->la = la;
	
	reset_time();

 /* Setup the API interface for VDI to use */
	ta->get_tps		= get_tics_per_sec;
	ta->add_timeint		= add_time_interrupt;
	ta->del_timeint		= delete_time_interrupt;
	ta->set_user_tim	= set_user_tim;
	ta->set_next_tim	= set_next_tim;
	ta->reset_user_tim	= reset_user_tim;
	ta->reset_next_tim	= reset_next_tim;

	ta->reset		= reset_time;
	ta->enable		= enable_tint;
	ta->disable		= disable_tint;

	sr = spl7();
	old_timeint = (long)Setexc(etv_timer >> 2, time_interruptw);
	spl(sr);

	if (usp)
		Super(usp);

	return ta;
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
	short	i;
	short	sr;
	unsigned long *tints = timeints;

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

static short
get_tics_per_sec(void)
{
	return (1000/TPS);
}

/* Main etv_timer routine */
void
time_interrupt(void)
{
	register unsigned long *tints = (unsigned long *)&timeints;
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
static short
add_time_interrupt(unsigned long function, unsigned long tics)
{
	short i;
	register unsigned long *tints = (unsigned long *)&timeints;

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
delete_time_interrupt(unsigned long function)
{
	short sr;
	short i;
	register unsigned long *tints = (unsigned long *)&timeints;

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
	(void)set_user_tim((unsigned long)&donothing);
	return;
}
static void
reset_next_tim(void)
{
	(void)set_next_tim((unsigned long)&donothing);
	return;
}

static unsigned long
set_user_tim(unsigned long function)
{
	short sr;
	register struct timedrv *td = &tdrv;
	unsigned long old;

	sr = spl7();
	old = (unsigned long)td->user_tim;
	(unsigned long)td->user_tim = function;
	(unsigned long)td->la->user_tim = function;
	spl(sr);
	return old;
}

static unsigned long
set_next_tim(unsigned long function)
{
	short sr;
	register struct timedrv *td = &tdrv;
	unsigned long old;

	sr = spl7();
	old = (unsigned long)td->next_tim;
	(unsigned long)td->next_tim = function;
	(unsigned long)td->la->next_tim = function;
	spl(sr);
	return old;
}

static void
donothing()
{
	return;
}
