#ifndef _OVDI_MOUSEDRV_H
#define _OVDI_MOUSEDRV_H

#include "ovdi_types.h"
#include "linea_vars.h"
#include "mouse.h"
#include "pdvapi.h"

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
	unsigned char	*src;
	unsigned char	*save;
};
typedef struct xmsave XMSAVE;

struct xmform
{
	short	mx;			/* Max X of mouse area */
	short	my;			/* Max Y of mouse area */
	short	bypl;			/* bytes per line of mouse area */
	unsigned char	*scr_base;		/* start address of mouse area */
	
	short	xhot;			/* X hotspot */
	short	yhot;			/* Y hotspot */
	short	planes;			/* Mouse data color depth */
	short	mfbypl;			/* Bytes per line of mouse data */
	short	width;			/* Width of mouse data in pixels */
	short	height;			/* Height of mouse data in pixels */
	short	fg_col;			/* Foreground VDI color */
	short	bg_col;			/* Background VDI color */
	unsigned long	fg_pix;			/* Foreground HW color pixel value */
	unsigned long	bg_pix;			/* Background HW color pixel value */

	XMSAVE	*save;			/* Pointer to XMSAVE used to save background */
	unsigned short	*mask;
	unsigned char	*data;			/* Pointer to mouse form data */
};
typedef	struct xmform XMFORM;

typedef void (*draw_mc)		(struct xmform *xmf, short x, short y);
typedef void (*undraw_mc)	(struct xmsave *xms);

#define MOUSE_SEMA(n) {				\
	if (n->msema)				\
	{					\
		do {}				\
		while ( !(*n->msema)() );	\
	}					\
}

struct mousedrv
{
	struct pdvapi	*pdapi;
	struct pdvinfo	pdi;
	struct mdrv_cb	cb;

	struct raster	*raster;
	struct colinf	*colinf;
	LINEA_VARTAB	*la;
	XMFORM		*current_xmf;
	XMSAVE		*current_xms;
	draw_mc		draw_mcurs;
	undraw_mc	undraw_mcurs;
	void		(*vreschk)(short, short);
	short		(*msema)(void);
	volatile unsigned long	flags;
	volatile unsigned short	interrupt;
#define MC_ENABLED	0x01
#define MC_MOVED	0x02
#define MDRV_ENABLED	0x04
#define MC_INT		0x80
	volatile short	hide_ct;
	volatile short	current_x;
	volatile short	current_y;
	short		min_x, min_y, max_x, max_y;
	unsigned short		bs_mask;
	volatile unsigned short	current_bs;
	volatile unsigned short	changed_bs;
	volatile unsigned short	last_bs;

};
typedef struct mousedrv MOUSEDRV;

#endif	/* _OVDI_MOUSEDRV_H */
