#ifndef _OVDI_MOUSEDRV_H
#define _OVDI_MOUSEDRV_H

#include "linea.h"
#include "mouse.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"

#define	MB_LEFT		1
#define MB_RIGHT	2
#define MB_MIDDLE	4

/* *********************************************************************** */
/*  MOUSE DRIVER DEFINITIONs */
/* *********************************************************************** */
struct xmsave
{
	short	width;
	short	height;
	short	bypl;
	short	valid;
	unsigned char *src;
	unsigned char *save;
};

struct xmform
{
	short	mx;			/* Max X of mouse area */
	short	my;			/* Max Y of mouse area */
	short	bypl;			/* bytes per line of mouse area */
	unsigned char *scr_base;	/* start address of mouse area */
	
	short	xhot;			/* X hotspot */
	short	yhot;			/* Y hotspot */
	short	planes;			/* Mouse data color depth */
	short	mfbypl;			/* Bytes per line of mouse data */
	short	width;			/* Width of mouse data in pixels */
	short	height;			/* Height of mouse data in pixels */
	short	fg_col;			/* Foreground color (only used with mono mouse forms) */
	short	bg_col;			/* Background color (only used with mono mouse forms) */
	long	fg_pix;			/* Foreground color pixel value */
	long	bg_pix;			/* Background color pixel value */

	XMSAVE	*save;			/* Pointer to XMSAVE used to save background */
	unsigned short *mask;
	unsigned char *data;		/* Pointer to mouse form data */
};


struct mousedrv
{
	short			buttons;
	short			wheels;
	
	void			(*start)(void);
	void			(*stop)(void);

	void			(*relmovmcurs)(register short x, register short y);
	void			(*absmovmcurs)(register short x, register short y);
	void			(*butchg)(register unsigned short bs);

	LINEA_VARTAB		*la;
	XMFORM			*current_xmf;
	XMSAVE			*current_xms;
	draw_mc			draw_mcurs;
	undraw_mc		undraw_mcurs;
	void			(*vreschk)(short, short);
	volatile unsigned long	flags;
	volatile unsigned short	interrupt;
#define MC_ENABLED	0x01
#define MC_MOVED	0x02
#define MDRV_ENABLED	0x04
#define MC_INT		0x80
	volatile short		hide_ct;
	volatile short		current_x;
	volatile short		current_y;
	short			min_x, min_y, max_x, max_y;
	unsigned short		bs_mask;
	volatile unsigned short	current_bs;
	volatile unsigned short	changed_bs;
	volatile unsigned short	last_bs;

};

struct mouseapi
{
	short		buttons;
	short		wheels;
	
	void		(*enable)(void);
	void		(*disable)(void);

	void		(*setxmfres)(VIRTUAL *v);
	void		(*setnewmform)(VIRTUAL *v, MFORM *mf);
	void		(*resetmcurs)();
	void		(*enablemcurs)();
	void		(*disablemcurs)();
	void		(*showmcurs)(short);
	void		(*hidemcurs)();

	void		(*relmovmcurs)(register short x, register short y);
	void		(*absmovmcurs)(register short x, register short y);
	void		(*butchg)(register unsigned short bs);

	unsigned long 	(*getbutstat)();
	void		(*getmcoords)(short *xy);

#define MVEC_BUT	0
#define MVEC_CUR	1
#define MVEC_MOV	2
	unsigned long	(*setvector)(short vecnum, unsigned long vector);
	void		(*housekeep)(void);

};

MOUSEAPI * init_mouse	(VIRTUAL *v, LINEA_VARTAB *la);

extern void init_mouse_device	(MOUSEDRV *mdrv);

#endif	/* _OVDI_MOUSEDRV_H */