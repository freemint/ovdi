#ifndef _OVDI_DEFS_H
#define _OVDI_DEFS_H

#include "ovdi_types.h"
typedef struct raster RASTER;
typedef struct colinf COLINF;
typedef struct virtual VIRTUAL;
typedef struct ovdi_hwapi OVDI_HWAPI;

typedef struct ovdi_driver OVDI_DRIVER;
typedef struct ovdi_device OVDI_DEVICE;

#include "vdi_defs.h"
#include "ovdi_mouse.h"
#include "vdi_printer.h"

#define MAX_VIRTUALS		 512
#define MAX_VDI_FUNCTIONS	 259
#define MAX_VDI_NEGFUNCTIONS	   4
#define SCRATCH_BUFFER_SIZE	1024

/*
* Flag definitions for 'flags' in struct raster
*/
#define R_IS_SCREEN	1		/* Indicates that this raster is the physical screen raster */
#define R_IN_VRAM	2		/* Indicates that raster is accessible hardware accelerator, where it exists */
#define R_MALLOCED	4		/* Indicates if raster base can be deallocated via free_mem */
/*
 * Macro to check if wheter or not the raster is accessed by hardware,
 * for example if accelerator is finished
*/
#define SYNC_RASTER(n) {			\
	if (n->sync)				\
	{					\
		do {}				\
		while ( !(*n->sync)() );	\
	}					\
}
struct resfmt
{
	O_16	planes;
	O_16	clut;
	O_16	format;
	O_16	pixlen;
	char	*pixelformat;
};
typedef struct resfmt RESFMT;

/*
 * Structure describing a raster - all drawing primitives get its info from
 * this structure
*/
struct raster
{
	/* The top part of raster structure is filled	*/
	/* in by the device driver upon resolution	*/
	/* changes, etc.				*/
	long			(*sync)(void);		/* Hardware ready? (accelerator done, etc) */
	unsigned char		*base;			/* Address of raster */
	O_u32		lenght;			/* Lenght of raster in bytes */
	long			bypl;			/* Size of one line in bytes */
	O_16			flags;			/* Flags */
	O_16			realflags;		/* Real flags */
	O_Pos			w, h;			/* width and height of raster in pixels */
	O_Pos			x1, y1, x2, y2;		/* Bounding rectangle (usually 0,0, w-1, h-1) */

	RESFMT			res;

	/* This part is filled in by the VDI.		*/
	/* Device driver must NOT touch this area!	*/
	O_16			wpixel, hpixel;
	struct ovdi_drawers	*drawers;		/* Drawers used with current resolution of raster */
	struct ovdi_drawers	(**odrawers);		/* Pointer to table of different resolution drawers - used when changing raster resolution */
	struct ovdi_utils	*utils;			/* Pointer to table of different utility functions */
	RGB_LIST		rgb_levels;		/* Number of levels for Red, Green, Blue, Alpha and Ovl parts */
	RGB_LIST		rgb_bits;		/* Number of bits used for Red, Green, Blue, Alpha and Ovl */
};
//typedef struct raster RASTER;

/*
 * This structure carries all information regarding color.
*/
struct colinf
{
	O_16		pens;			/* Number of pens */
	O_16		planes;			/* Planes */
	O_16		*color_vdi2hw;		/* vdi to hardware colorindex conversion table */
	O_16		*color_hw2vdi;		/* hardware to vdi colorindex conversion table */
	char		*pixelformat;		/* Pointer to pixel layout description */
	long		*pixelvalues;		/* Pixelvalues, as they're written into video-ram */
	RGB_LIST	*request_rgb;		/* Requested, relative RGB values (0 - 1000) */
	RGB_LIST	*actual_rgb;		/* Actual RGB values, used to construct pixelvalues */
};
//typedef struct colinf COLINF;

/*
 * All drawing primitives use this structure.
*/
struct pattern_attribs
{
	O_16		expanded;		/* planes in expanded data or NULL if not expanded. */
	O_16		interior;		/* */
	O_16		wrmode;			/* writing mode (unused as of yet) */
	O_16		width;			/* Width of pattern in pixels */
	O_16		height;			/* Height of pattern in pixels */
	O_16		wwidth;			/* Width of fill pattern in words */
	O_16		planes;			/* True if fill is multiplane */
	O_16		color[4];		/* wrmode is used as index into color array */
	O_16		bgcol[4];		/* background color */
	O_16		ud;			/* User define data */
	O_u16	*data;			/* Original data */
	O_u16	*mask;			/* Mask - created when expanded */
	O_u16	*exp_data;		/* Expanded data - device dependant */
	union
	{
		struct /* Fill */
		{
			O_16 style;
			struct pattern_attribs *perimeter; /* NULL == no perimeter, else points to pattern_attribs used to render permimeter */
		} f;
		struct /* Lines */
		{
			O_16 index;
			O_16 width;
			O_16 beg, end;
		} l;
		struct /* Polymarkers */
		{
			O_16 index;		/* Line index used to draw poly marker */
			O_16 type;		/* Polymarker type */
			O_16 width;
			O_16 height;
			O_16 scale;
		} p;
	} t;
};
typedef struct pattern_attribs PatAttr;

struct	currfont
{
	O_16	defid;
	O_16	defht;
	struct	xgdf_head *current;
	struct	font_head *header;
	struct	xgdf_head *loaded;
	O_16	color;
	O_16	bgcol;
	O_16	wrmode;
	O_16	style;
	O_16	angle;
	O_16	fx_x_ofst;
	O_16	fx_y_ofst;
	O_16	pts;			/* true if height set in points */
	O_16	size;			/* in height or points */
	O_16	chup;			/* Character Up vector */
	O_16	lcount;			/* number of loaded fonts */
	O_16	num;			/* Total number of faces */
	O_16	halign;
	O_16	valign;
	O_16	scaled;
	O_16	sclsts;			/* True if scaling up */
	struct	font_head *scratch_head;
};
typedef struct currfont CURRFONT;

/*
 * Used to hold 16x16 fill pattern expansions
*/
struct pattern_data
{
	O_u16 data[(2*16) * 16];
	O_u16 mask[(2*16) * 16];
	O_u16 edata[(2*16) * 16];
};
typedef struct pattern_data PatDat;

/*
 * This is the main VIRTUAL structure.
*/
#define V_OSBM		1
#define V_OWN_APPBUFF	2

struct virtual
{
	O_16			flags;
	O_16			func;
	O_16			handle;
	O_16			pid;
	char			procname[32];
	O_16			dda_inc;		/* Fraction to be added to the DDA	*/
	char			*scratchp;		/* Pointer to text scratch buffer	*/
	O_16			scratchs;		/* Offset to large text buffer		*/

	struct xgdf_head	*fring;
	struct currfont		font;

	PatAttr			*currfill;		/* Points to either fill or udfill */
	PatAttr			fill;
	PatAttr			udfill;
	PatAttr			perimeter;
	PatAttr			line;
	PatAttr			pmarker;

#define REQ_MODE	1

	O_16			locator;
	O_16			valuator;
	O_16			choise;
	O_16			string;
	
	VIRTUAL			*lawk;			/* Physical workstation used by the console */
	VIRTUAL			*root;			/* Physical parent. NULL if this is parent */
	O_16			wrmode;			/* Current writing mode			*/
	O_16			xfm_mode;		/* Transformation mode requested	*/

	CLIPRECT		clip;

	PRN_SETTINGS		*prn_settings;

	struct ovdi_hwapi	*hw;
	struct console		*con;
	struct ovdi_driver	*driver;
	struct ovdi_driver	*physical;
	struct raster		*raster;
	struct colinf		*colinf;
	struct kbdapi		*kbdapi;
	struct mouseapi		*mouseapi;
	struct timeapi		*timeapi;
	struct vbiapi		*vbiapi;
	struct fontapi		*fontapi;

	long			app_buff_size;
	void			*app_buff;

	long			ptsbuffsiz;
	long			spanbuffsiz;

	O_Pos			ptsbuff[PTSBUFF_SIZ >> 1];
	O_Pos			spanbuff[SPANBUFF_SIZ >> 1];

	PatDat			filldata;
	PatDat			udfilldata;
	PatDat			perimeterdata;
	PatDat			linedata;
	PatDat			pmarkerdata;

};

/*
 * Root hardware API. Together the pointers in this structure
 * makes up a complete physical VDI device. Should be possible
 * for oVDI to drive several cards this way.
*/
struct ovdi_hwapi
{
	struct	ovdi_device	*device;	/* Device API */
	struct	ovdi_driver	*driver;	/* Driver for this physical */
	struct	ovdi_drawers	(**odrawers);	/* Pointer to table of different resolution drawers */
	struct	ovdi_utils	*utils;		/* Pointer to utility functions specific to this device */
	struct	console		*console;	/* Console driver */
	struct	colinf		*colinf;	/* Root colinf */
	struct	kbdapi		*keyboard;	/* Keyboard driver */
	struct	mouseapi	*mouse;		/* Mouse driver */
	struct	pdvapi		*pointdev;	/* Pointing device driver */
	struct	timeapi		*time;		/* Time driver */
	struct	vbiapi		*vbi;		/* VBI driver */
	struct	fontapi		*font;		/* Font driver */
};

typedef void (*GDP_function)(VDIPB *pb, VIRTUAL *v);
typedef void (*vdi_function)(VDIPB *pb, VIRTUAL *v);

typedef void VDI_function(VDIPB *pb, VIRTUAL *v, ... );

struct ovdi_jmptab
{
	VDI_function *f;
};
typedef struct ovdi_jmptab OVDI_JMPTAB;

struct ovdi_vtab
{
	VIRTUAL *v;
	O_16 pid;
};
typedef struct ovdi_vtab OVDI_VTAB;


/* *************************************************************************** */
/* *************************************************************************** */
/* *************************************************************************** */
struct rop_pb
{
	O_Pos	sx1, sy1, sx2, sy2;
	O_Pos	dx1, dy1, dx2, dy2;


	void	*s_addr;
	O_16	s_is_scrn;
	O_16	s_bypl;
	O_Pos	s_w, s_h;

	void	*d_addr;
	O_16	d_is_scrn;
	O_16	d_bypl;
	O_Pos	d_w, d_h;
};
typedef struct rop_pb ROP_PB;

#if 0
struct pixel_blits
{
	void (*all_white)	(unsigned char *addr, long data);
	void (*s_and_d)		(unsigned char *addr, long data);
	void (*s_and_notd)	(unsigned char *addr, long data);
	void (*s_only)		(unsigned char *addr, long data);
	void (*nots_and_d)	(unsigned char *addr, long data);
	void (*d_only)		(unsigned char *addr, long data);
	void (*s_xor_d)		(unsigned char *addr, long data);
	void (*s_or_d)		(unsigned char *addr, long data);
	void (*not_sord)	(unsigned char *addr, long data);
	void (*not_sxord)	(unsigned char *addr, long data);
	void (*not_d)		(unsigned char *addr, long data);
	void (*s_or_notd)	(unsigned char *addr, long data);
	void (*not_s)		(unsigned char *addr, long data);
	void (*nots_or_d)	(unsigned char *addr, long data);
	void (*not_sandd)	(unsigned char *addr, long data);
	void (*all_black)	(unsigned char *addr, long data);
};

struct raster_blits
{
	void (*all_white)	(ROP_PB *);
	void (*s_and_d)		(ROP_PB *);
	void (*s_and_notd)	(ROP_PB *);
	void (*s_only)		(ROP_PB *);
	void (*nots_and_d)	(ROP_PB *);
	void (*d_only)		(ROP_PB *);
	void (*s_xor_d)		(ROP_PB *);
	void (*s_or_d)		(ROP_PB *);
	void (*not_sord)	(ROP_PB *);
	void (*not_sxord)	(ROP_PB *);
	void (*not_d)		(ROP_PB *);
	void (*s_or_notd)	(ROP_PB *);
	void (*not_s)		(ROP_PB *);
	void (*nots_or_d)	(ROP_PB *);
	void (*not_sandd)	(ROP_PB *);
	void (*all_black)	(ROP_PB *);
};

struct wr_modes
{
	void (*rep_fb)(unsigned char *addr, long data);
	void (*rep_bg)(unsigned char *addr, long data);
	
	void (*trns_fg)(unsigned char *addr, long data);
	void (*trns_bg)(unsigned char *addr, long data);

	void (*xor_fg)(unsigned char *addr, long data);
	void (*xor_bg)(unsigned char *addr, long data);

	void (*erase_fg)(unsigned char *addr, long data);
	void (*erase_bg)(unsigned char *addr, long data);
};
#endif

typedef void	(*pixel_blit)(unsigned char *addr, long data);
typedef void	(*raster_blit)(ROP_PB *);

//typedef void		(*draw_pixel)(unsigned char *adr, long data);
//typedef long		(*read_pixel)(unsigned char *adr);
//typedef void		(*put_pixel)(short x, short y, long data);
//typedef O_u32	(*get_pixel)(unsigned char *scrbase, short bypl, short x, short y);
//typedef void		(*raster_op)(unsigned char *srcbase, short srcbypl, unsigned char *dstbase, short dstbypl, short width, short height, short dir);

typedef raster_blit raster_blits[16];
typedef pixel_blit pixel_blits[16];


 /* The default functions here work with all color depths, and is copied
  * to the current drawers structure. Drivers contain one such structure for each 
  * color-depth mode (1, 2, 4, 8, 15, 16, 24 and 32 bit color modes) respectively.
 */
typedef	void (*Ffilled_rect)	( RASTER *r, COLINF *c, VDIRECT *corners, VDIRECT *clip, PatAttr *ptrn);
typedef	void (*Ffilledpoly)	( RASTER *r, COLINF *c, O_Pos *pts, O_Int n, VDIRECT *clip, O_Pos *points, long pointasize, PatAttr *ptrn);

typedef	void (*Farc)		( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn);
typedef	void (*Fcircle)		( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos *points, PatAttr *ptrn);
typedef	void (*Fellipse)	( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Pos *points, PatAttr *ptrn);
typedef	void (*Fellarc)		( VIRTUAL *v, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Int beg_ang, O_Int end_ang, O_Pos *points, PatAttr *ptrn);
typedef	void (*Frbox)		( VIRTUAL *v, O_Int gdp_code, VDIRECT *corners, PatAttr *ptrn);

typedef	void (*Fpline)		( RASTER *r, COLINF *c, O_Pos *pts, long numpts, VDIRECT *clip, O_Pos *points, long pointasize, PatAttr *ptrn);
typedef	void (*Fhvline)		( RASTER *r, COLINF *c, O_Pos xory1, O_Pos xory2, O_Pos xory, PatAttr *ptrn);
typedef	void (*Fabline)		( RASTER *r, COLINF *c, struct vdirect *pnts, PatAttr *ptrn);

typedef	void (*Fspans)		( RASTER *r, COLINF *c, O_Pos x1, O_Pos x2, O_Pos y, PatAttr *ptrn);
typedef void (*Fmspans)		( RASTER *r, COLINF *c, O_Pos x1, O_Pos x2, O_Pos y1, O_Pos y2, PatAttr *ptrn);

typedef	void (*Fpmarker)	( RASTER *v, COLINF *c, POINT *origin, VDIRECT *clip, O_Int type, O_Int size, O_Int w_in, O_Int h_in, PatAttr *ptrn);

typedef	void (*Ftcpyfm)		( RASTER *r, COLINF *c, MFDB *src, MFDB *dst, O_Pos *pnts, VDIRECT *clip, O_Int fgcol, O_Int bgcol, O_Int wrmode);
typedef	void (*Focpyfm)		( RASTER *r, MFDB *src, MFDB *dst, O_Pos *pnts, VDIRECT *clip, O_Int wrmode);

 /*
  * This table contains pointers to the VDI primitives.
  * A graphics hardware driver can provide its own code by filling in the
  * pointer to its replacement function in the drawing function table it
  * returns via OVDI_DRIVER. 
 */

#define DRAW_FILLEDRECT_PTR(a)	(a->drawers->p.draw_filledrect)

#define DRAW_ARC_PTR(a)		(a->raster->drawers->p.draw_arc)
#define DRAW_PIESLICE_PTR(a)	(a->raster->drawers->p.draw_pieslice)
#define DRAW_CIRCLE_PTR(a)	(a->raster->drawers->p.draw_circle)
#define DRAW_ELLIPSE_PTR(a)	(a->raster->drawers->p.draw_ellipse)
#define DRAW_ELLIPSEARC_PTR(a)	(a->raster->drawers->p.draw_ellipsearc)
#define DRAW_ELLIPSEPIE_PTR(a)	(a->raster->drawers->p.draw_ellipsepie)
#define DRAW_RBOX_PTR(a)	(a->raster->drawers->p.draw_rbox)

#define DRAW_ABLINE_PTR(a)	(a->drawers->p.draw_abline)
#define DRAW_HLINE_PTR(a)	(a->drawers->p.draw_hline)
#define DRAW_VLINE_PTR(a)	(a->drawers->p.draw_vline)
#define DRAW_PLINE_PTR(a)	(a->drawers->p.draw_pline)
#define DRAW_WIDELINE_PTR(a)	(a->drawers->p.draw_wideline)

#define DRAW_SPANS_PTR(a)	(a->drawers->p.draw_spans)
#define DRAW_MSPANS_PTR(a)	(a->drawers->p.draw_mspans)

#define DRAW_FILLEDPOLY_PTR(a)	(a->drawers->p.draw_filledpoly)
#define DRAW_PMARKER_PTR(a)	(a->drawers->p.draw_pmarker)

#define RT_CPYFM_PTR(a)		(a->drawers->p.rt_cpyfm)
#define RO_CPYFM_PTR(a)		(a->drawers->p.ro_cpyfm)

#define DRAW_FILLEDRECT(a,b,c,d,e)		({(*DRAW_FILLEDRECT_PTR(a))(a,b,c,d,e);})
#define DRAW_ARC(a,b,c,d,e,f,g,h)		({(*DRAW_ARC_PTR(a))(a,b,c,d,e,f,g,h);})
#define DRAW_PIESLICE(a,b,c,d,e,f,g,h)		({(*DRAW_PIESLICE_PTR(a))(a,b,c,d,e,f,g,h);})
#define DRAW_CIRCLE(a,b,c,d,e,f)		({(*DRAW_CIRCLE_PTR(a))(a,b,c,d,e,f);})
#define DRAW_ELLIPSE(a,b,c,d,e,f,g)		({(*DRAW_ELLIPSE_PTR(a))(a,b,c,d,e,f,g);})
#define DRAW_ELLIPSEARC(a,b,c,d,e,f,g,h,i)	({(*DRAW_ELLIPSEARC_PTR(a))(a,b,c,d,e,f,g,h,i);})
#define DRAW_ELLIPSEPIE(a,b,c,d,e,f,g,h,i)	({(*DRAW_ELLIPSEPIE_PTR(a))(a,b,c,d,e,f,g,h,i);})
#define DRAW_RBOX(a,b,c,d)			({(*DRAW_RBOX_PTR(a))(a,b,c,d);})

#define DRAW_ABLINE(a,b,c,d)			({(*DRAW_ABLINE_PTR(a))(a,b,c,d);})
#define DRAW_HLINE(a,b,c,d,e,f)			({(*DRAW_HLINE_PTR(a))(a,b,c,d,e,f);})
#define DRAW_VLINE(a,b,c,d,e,f)			({(*DRAW_VLINE_PTR(a))(a,b,c,d,e,f);})
#define DRAW_PLINE(a,b,c,d,e,f,g,h)		({(*DRAW_PLINE_PTR(a))(a,b,c,d,e,f,g,h);})
#define DRAW_WIDELINE(a,b,c,d,e,f,g,h)		({(*DRAW_WIDELINE_PTR(a))(a,b,c,d,e,f,g,h);})

#define DRAW_SPANS(a,b,c,d,e,f)			({(*DRAW_SPANS_PTR(a))(a,b,c,d,e,f);})
#define DRAW_MSPANS(a,b,c,d,e,f,g)		({(*DRAW_MSPANS_PTR(a))(a,b,c,d,e,f,g);})

#define DRAW_FILLEDPOLY(a,b,c,d,e,f,g,h)	({(*DRAW_FILLEDPOLY_PTR(a))(a,b,c,d,e,f,g,h);})
#define DRAW_PMARKER(a,b,c,d,e,f,g,h,i)		({(*DRAW_PMARKER_PTR(a))(a,b,c,d,e,f,g,h,i);})

#define RT_CPYFM(a,b,c,d,e,f,g,h,i)		({(*RT_CPYFM_PTR(a))(a,b,c,d,e,f,g,h,i);})
#define RO_CPYFM(a,b,c,d,e,f)			({(*RO_CPYFM_PTR(a))(a,b,c,d,e,f);})

struct vdiprimitives
{
	Ffilled_rect	draw_filledrect;
	Farc		draw_arc;
	Farc		draw_pieslice;
	Fcircle		draw_circle;
	Fellipse	draw_ellipse;
	Fellarc		draw_ellipsearc;
	Fellarc		draw_ellipsepie;
	Frbox		draw_rbox;

	Fabline		draw_abline;
	Fhvline		draw_hline;
	Fhvline		draw_vline;
	Fpline		draw_pline;
	Fpline		draw_wideline;

	Fspans		draw_spans;
	Fmspans		draw_mspans;
	
	Ffilledpoly	draw_filledpoly;
	Fpmarker	draw_pmarker;

	Ftcpyfm		rt_cpyfm;
	Focpyfm		ro_cpyfm;
};
typedef struct vdiprimitives VDIPRIMITIVES;

#define FILL_16X_PTR(a)		(a->drawers->fill_16x)
#define SPANS_16X_PTR(a)	(a->drawers->spans_16x)

#define FILL_16X(a,b,c,d)		({(*FILL_16X_PTR(a))(a,b,c,d);})
#define SPANS_16X(a,b,c,d,e)		({(*SPANS_16X_PTR(a))(a,b,c,d,e);})

struct ovdi_drawers
{
/* REMEMBER TO UPDATE THE TABLES IN OVDI_RASTERS.C WHEN MODIFYING HERE !!!!!!!!!!!!!!!!11 */ 
 /*
  * The VDI primitive functions
 */
	VDIPRIMITIVES	p;
	
 /*	
  * Functions below here is always color-depth dependant. If driver dont provide
  * these, standard and VERY, VERY slow ones are used.
 */
	RESFMT		*res;

	void		(*draw_pixel)		( unsigned char *adr, long data);
	void		(*read_pixel)		( unsigned char *adr, long data);
	void		(*put_pixel)		( unsigned char *base, O_Int bypl, O_Pos x, O_Pos y, O_u32 data);
	O_u32		(*get_pixel)		( unsigned char *base, O_Int bypl, O_Pos x, O_Pos y);

	void		(*fill_16x)		( RASTER *r, COLINF *c, O_Pos *corners, PatAttr *ptrn);
	void		(*spans_16x)		( RASTER *r, COLINF *c, O_Pos *spans, O_Int n, PatAttr *ptrn);

	pixel_blits	drp;	/* Draw Raster Points */
	pixel_blits	dlp;	/* Draw line Points */
	pixel_blits	pixel_blits;
	raster_blits	raster_blits;

	/* Mouse cursor rendering */
	draw_mc		draw_mcurs;
	undraw_mc	undraw_mcurs;

};
typedef struct ovdi_drawers OVDI_DRAWERS;

struct ovdi_utils
{
/*  color/hardware indipendant helper functions */
	O_Int	(*clc_nsteps)		( O_Pos xrad, O_Pos yrad);
	void	(*clc_arc)		( VIRTUAL *v, O_Int gdp_code, O_Pos xc, O_Pos yc, O_Pos xrad, O_Pos yrad, O_Int beg_ang, O_Int end_ang, O_Int del_ang, O_Int n_steps, O_Pos *points, PatAttr *ptrn);

	O_Int	(*clipbox)		( VDIRECT *corners, VDIRECT *clip);
	void	(*sortcpy_corners)	( O_16 *source, O_Pos *dest );

	O_Int	(*code)			( POINT *input, VDIRECT *clip);
	O_Int	(*clip_line)		( VDIRECT *input, VDIRECT *clip);

	O_Int	(*fix_raster_coords)	( O_Pos *spts, O_Pos *dpts, O_Pos *c);

	void	(*trnfm)		( MFDB *src, MFDB *dst);

	void	(*cnv_v2d_1b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_v2d_2b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_v2d_4b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_v2d_8b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_v2d_15b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_v2d_16b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_v2d_24b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_v2d_32b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	long	cnv_v2d_rsv[4];

	void	(*cnv_d2v_1b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_d2v_2b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_d2v_4b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_d2v_8b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_d2v_15b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_d2v_16b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_d2v_24b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	void	(*cnv_d2v_32b)		( O_u16 *src, O_u16 *dst, O_u32 splen);
	long	cnv_d2v_rsv[4];
};
typedef struct ovdi_utils OVDI_UTILS;

/*
 * This is the graphics hardware driver structure. Contains information about
 * current video mode (raster), and diverse function tables. A driver can
 * provide its own routines for just about any VDI drawing primitives via this
 * structure. All function pointers can be NULL's in which case oVDI fills in
 * its internal generic functions. Some of these generic functions might be
 * VERY SLOW!
*/ 
struct ovdi_driver
{
	O_32		version;
	O_16		format;
	
#define PF_ATARI	1
#define PF_PLANES	2
#define PF_PACKED	4
#define	PF_FALCON	8
#define	PF_BE		16
#define	PF_BS		32

	RASTER		r;
	
	O_u32		palette;
	O_16		v_top, v_bottom, v_left, v_right;

	void		*vram_start;
	long		vram_size;

	O_16		scr_count;
	O_32		scr_size;

	void		*log_base;

	OVDI_DEVICE	*dev;

/*
 * These are pointers to a ovdi_drawers function table for each
 * color depth the driver supports.
 * These pointers must be set, even if it points to a function table
 * of only NULL pointers.
*/
	struct ovdi_drawers	*drawers_1b;
	struct ovdi_drawers	*drawers_2b;
	struct ovdi_drawers	*drawers_4b;
	struct ovdi_drawers	*drawers_8b;
	struct ovdi_drawers	*drawers_15b;
	struct ovdi_drawers	*drawers_16b;
	struct ovdi_drawers	*drawers_24b;
	struct ovdi_drawers	*drawers_32b;
	
};

/*
 * This is the graphics hardware device API. All graphics hardware
 * access and driver functions goes via this API.
*/
struct ovdi_device
{
	struct ovdi_device	*nxtapi;
	O_32 			version;
	char 			*sname;
	char			*lname;
	char			*pathname;
	char			*filename;

	OVDI_DRIVER *	(*open)(struct ovdi_device *dev);
	long		(*close)(OVDI_DRIVER *drv);
	O_Int		(*set_vdires)(OVDI_DRIVER *drv, O_Int scrndev_id);
	O_Int		(*get_res_info)(OVDI_DRIVER *drv);
	O_u8 *		(*setpscr)(OVDI_DRIVER *drv, O_u8 *scrnadr);
	O_u8 *		(*setlscr)(OVDI_DRIVER *drv, O_u8 *logscr);
	void		(*setcol)(OVDI_DRIVER *drv, O_Int pen, RGB_LIST *colors);
	void		(*vsync)(OVDI_DRIVER *drv);

	void		(*vreschk)(O_Int x, O_Int y);
	O_Int		(*msema)(void);
};

#endif	/* _OVDI_DEFS_H */
