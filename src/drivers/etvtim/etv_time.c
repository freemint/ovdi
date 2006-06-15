/*
 This file contains the time-related stuff.
*/
#include <osbind.h>
#include <stddef.h>
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
	volatile short	flags;
	LINEA_VARTAB	*la;
	void _cdecl	(*user_tim)(void);
	void _cdecl	(*next_tim)(void);
};

void _cdecl init	(OVDI_LIB *l, struct module_desc *ret, char *p, char *f);

extern unsigned long old_timeint;
extern void time_interruptw(void);
static void donothing(void);

void time_interrupt(void);

static short _cdecl		install(LINEA_VARTAB *la);
static short _cdecl		get_tics_per_sec(void);
static short _cdecl		add_time_interrupt(unsigned long function, unsigned long tics, long arg);
static void _cdecl		delete_time_interrupt(unsigned long func, long arg);
static unsigned long _cdecl	set_user_tim(unsigned long func);
static unsigned long _cdecl	set_next_tim(unsigned long func);

static void _cdecl enable_tint(void);
static void _cdecl disable_tint(void);
static void _cdecl reset_time(void);
static void _cdecl reset_user_tim(void);
static void _cdecl reset_next_tim(void);

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

struct tint_entry
{
	long	ticks;
	long	rticks;
	timefunc func;
	long	arg;
};

#define MAX_TINTS	10
static struct tint_entry Tints[] =
{
	{0,0,NULL,0},
	{0,0,NULL,0},
	{0,0,NULL,0},
	{0,0,NULL,0},
	{0,0,NULL,0},
	{0,0,NULL,0},
	{0,0,NULL,0},
	{0,0,NULL,0},
	{0,0,NULL,0},
	{0,0,NULL,0},
	{0,0,NULL,0},
};
/* At the time when this function is called, the linea variable table is not
*  not initialized! So, do not rely on anything in it, and only setup
*  own things.
*/
void _cdecl
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

static short _cdecl
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
		if (usp)
			Super(usp);
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

static void _cdecl
enable_tint(void)
{
	struct timedrv *td = &tdrv;

	if (old_timeint)
		td->flags |= TIME_ACTIVE;
}
static void _cdecl
disable_tint(void)
{
	struct timedrv *td = &tdrv;

	td->flags &= ~TIME_ACTIVE;
}

static void _cdecl
reset_time(void)
{
	int	i;
	short	sr;
	struct tint_entry *tints = Tints;

	sr = spl7();
	disable_tint();
	for (i = 0; i < MAX_TINTS; i++)
		tints[i] = tints[MAX_TINTS];
	spl(sr);

	reset_user_tim();
	reset_next_tim();
}

static short _cdecl
get_tics_per_sec(void)
{
	return (1000/TPS);
}

/* Main etv_timer routine */
void _cdecl
time_interrupt(void)
{
	register struct tint_entry *tints;
	register struct timedrv *td = &tdrv;

	if (!(td->flags & TIME_ACTIVE))
		return;

	tints = Tints;

	while (tints->func)
	{
		if (!tints->rticks)
		{
			tints->rticks = tints->ticks;
			(*tints->func)(tints->arg);
		}
		else
			tints->rticks--;
		tints++;
	}
 /* Call user_tim and next_tim */
	__asm__ volatile
	(
		"movem.l	d0-d7/a1-a6,-(sp)\n\t"		\
		"move.l		%0,a0\n\t"			\
		"jsr		(a0)\n\t"			\
		"movem.l	(sp),d0-d7/a1-a6\n\t"		\
		"move.l		%1,a0\n\t"			\
		"jsr		(a0)\n\t"			\
		"movem.l	(sp)+,d0-d7/a1-a6\n\t"		\
		:
		: "a"(td->user_tim), "a"(td->next_tim)
		: "a0"
	);
}	

/* Installs a function that is called each timer tick	*/
/* Returns the 'handle' of the function or a negative	*/
/* number on error.					*/
static short _cdecl
add_time_interrupt(unsigned long function, unsigned long ticks, long arg)
{
	short i;
	register struct tint_entry *tints = Tints;

	for (i = 0; i < MAX_TINTS; i++) {
		if (!tints[i].func) {
			short sr;
			sr = spl7();
			tints[i].ticks = ticks;
			tints[i].rticks = ticks;
			tints[i].func = (void *)function;
			tints[i].arg = arg;
			spl(sr);
			return i;
		}
	}
	return -1;
}

static void _cdecl
delete_time_interrupt(unsigned long function, long arg)
{
	short sr;
	int i;
	register struct tint_entry *tints = Tints;

	for (i = 0; i < MAX_TINTS; i++) {
		if (tints[i].func == (void *)function && tints[i].arg == arg) {
			sr = spl7();
			while (i < MAX_TINTS) {
				tints[i] = tints[i + 1];
				i++;
			}
			spl(sr);
			break;
		}
	}
}

static void _cdecl
reset_user_tim(void)
{
	(void)set_user_tim((unsigned long)&donothing);
	return;
}
static void _cdecl
reset_next_tim(void)
{
	(void)set_next_tim((unsigned long)&donothing);
	return;
}

static unsigned long _cdecl
set_user_tim(unsigned long function)
{
	short sr;
	register struct timedrv *td = &tdrv;
	unsigned long old;

	sr = spl7();
	old = (unsigned long)td->user_tim;
	td->user_tim = (void _cdecl (*)(void))function;
	td->la->user_tim = (void _cdecl (*)(void))function;
	spl(sr);
	return old;
}

static unsigned long
set_next_tim(unsigned long function)
{
	short sr;
	register struct timedrv *td = &tdrv;
	void (*old)(void);

	sr = spl7();
	old = td->next_tim;
	td->next_tim = (void _cdecl (*)(void))function;
	td->la->next_tim = (void _cdecl (*)(void))function;
	spl(sr);
	return (unsigned long)old;
}

static void
donothing(void)
{
	return;
}
