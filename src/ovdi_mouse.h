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
	O_16	width;
	O_16	height;
	O_16	bypl;
	O_16	valid;
	O_u8	*src;
	O_u8	*save;
};
typedef struct xmsave XMSAVE;

struct xmform
{
	O_16	mx;			/* Max X of mouse area */
	O_16	my;			/* Max Y of mouse area */
	O_16	bypl;			/* bytes per line of mouse area */
	O_u8	*scr_base;		/* start address of mouse area */
	
	O_16	xhot;			/* X hotspot */
	O_16	yhot;			/* Y hotspot */
	O_16	planes;			/* Mouse data color depth */
	O_16	mfbypl;			/* Bytes per line of mouse data */
	O_16	width;			/* Width of mouse data in pixels */
	O_16	height;			/* Height of mouse data in pixels */
	O_16	fg_col;			/* Foreground VDI color */
	O_16	bg_col;			/* Background VDI color */
	O_u32	fg_pix;			/* Foreground HW color pixel value */
	O_u32	bg_pix;			/* Background HW color pixel value */

	XMSAVE	*save;			/* Pointer to XMSAVE used to save background */
	O_u16	*mask;
	O_u8	*data;			/* Pointer to mouse form data */
};
typedef	struct xmform XMFORM;

typedef void (*draw_mc)		(struct xmform *xmf, O_Int x, O_Int y);
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
	void		(*vreschk)(O_Int, O_Int);
	O_Int		(*msema)(void);
	volatile O_u32	flags;
	volatile O_u16	interrupt;
#define MC_ENABLED	0x01
#define MC_MOVED	0x02
#define MDRV_ENABLED	0x04
#define MC_INT		0x80
	volatile O_16	hide_ct;
	volatile O_16	current_x;
	volatile O_16	current_y;
	O_16		min_x, min_y, max_x, max_y;
	O_u16		bs_mask;
	volatile O_u16	current_bs;
	volatile O_u16	changed_bs;
	volatile O_u16	last_bs;

};
typedef struct mousedrv MOUSEDRV;

#endif	/* _OVDI_MOUSEDRV_H */
