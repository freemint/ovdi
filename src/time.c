/*
 This file contains the time-related stuff.
*/

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
void donothing(void);

void time_interrupt(void);

short get_tics_per_sec(void);
short add_time_interrupt(unsigned long function, unsigned long tics);
void remove_time_interrupt(unsigned long func);
unsigned long set_user_tim(unsigned long func);
unsigned long set_next_tim(unsigned long func);

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
init_time(VIRTUAL *v, LINEA_VARTAB *la)
{
	int i;
#ifndef PRG_TEST
	short sr;
#endif
	struct timedrv *td = &tdrv;
	struct timeapi *ta = &tapi;
	unsigned long *tints = timeints;

	td->la = la;

	for (i = 0; i < MAX_TINTS; i++)
	{
		tints[0] = tints[1] = tints[2] = 0;
		tints += 3;
	}

	set_user_tim((unsigned long)&donothing);
	set_next_tim((unsigned long)&donothing);

 /* Setup the API interface for VDI to use */
	ta->get_tps = get_tics_per_sec;
	ta->add_timeint = add_time_interrupt;
	ta->set_user_tim = set_user_tim;
	ta->set_next_tim = set_next_tim;

#ifndef PRG_TEST
	sr = spl7();
	old_timeint = *(long *)etv_timer;
	*(long *)etv_timer = (unsigned long)&time_interruptw;
	spl(sr);
#else
	display("Install etv_timer\n");
#endif

	return ta;
}

short
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
short
add_time_interrupt(unsigned long function, unsigned long tics)
{
	short i;
	register unsigned long *tints = (unsigned long *)&timeints;

	for (i = 0; i < MAX_TINTS; i++)
	{
		if (!tints[2])
		{
#ifndef PRG_TEST
			short sr;

			sr = spl7();
#endif
			tints[0] = 0;
			tints[1] = tics;
			tints[2] = function;
#ifndef PRG_TEST
			spl(sr);
#endif
			return i;
		}
		tints += 3;
	}
	return -1;
}

void
remove_time_interrupt(unsigned long function)
{
#ifndef PRG_TEST
	short sr;
#endif
	short i;
	register unsigned long *tints = (unsigned long *)&timeints;

	for (i = 0; i < MAX_TINTS; i++)
	{
		if (tints[2] == function)
		{
#ifndef PRG_TEST
			sr = spl7();
#endif
			while (i < MAX_TINTS)
			{
				tints[0] = tints[0+3];
				tints[1] = tints[1+3];
				tints[2] = tints[2+3];
				tints += 3;
				i++;
			}
				
#ifndef PRG_TEST
			spl(sr);
#endif
			return;
		}
	}
	return;
}

unsigned long
set_user_tim(unsigned long function)
{
#ifndef PRG_TEST
	short sr;
#endif
	register struct timedrv *td = &tdrv;
	unsigned long old;

#ifndef PRG_TEST
	sr = spl7();
#endif
	old = (unsigned long)td->user_tim;
	(unsigned long)td->user_tim = function;
	(unsigned long)td->la->user_tim = function;
#ifndef PRG_TEST
	spl(sr);
#endif
	return old;
}

unsigned long
set_next_tim(unsigned long function)
{
#ifndef PRG_TEST
	short sr;
#endif
	register struct timedrv *td = &tdrv;
	unsigned long old;

#ifndef PRG_TEST
	sr = spl7();
#endif
	old = (unsigned long)td->next_tim;
	(unsigned long)td->next_tim = function;
	(unsigned long)td->la->next_tim = function;
#ifndef PRG_TEST
	spl(sr);
#endif
	return old;
}

void
donothing()
{
	return;
}
