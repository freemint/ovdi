#ifndef _OVDI_DEFS_H
#define _OVDI_DEFS_H

typedef struct virtual VIRTUAL;
typedef struct ovdi_lib OVDI_LIB;

#include "vdi_defs.h"
#include "mousedrv.h"

//#include "ovdi_dev.h"

#define MAX_VIRTUALS		512
#define MAX_VDI_FUNCTIONS	140
#define SCRATCH_BUFFER_SIZE	1024

/* Flag definitions for 'flags' in struct raster */
#define R_IS_SCREEN	1
#define R_IN_VRAM	2

/* Structure describing a raster - all drawing primitives get its info from */
/* this structure */
struct raster
{
	/* The top part of raster structure is filled	*/
	/* in by the device driver upon resolution	*/
	/* changes, etc.				*/
	unsigned char		*base;
	unsigned long		lenght;
	short			flags;
	short			format;
	short			w, h;
	short			x1, y1, x2, y2;
	short			clut;
	short			planes;
	short			pixlen;
	short			bypl;
	char			*pixelformat;

	/* This part is filled in by the VDI.		*/
	/* Device driver must NOT touch this area!	*/
	short			wpixel, hpixel;
	struct ovdi_drawers	*drawers;
	struct ovdi_drawers	(**odrawers);
	struct ovdi_utils	*utils;
	RGB_LIST		rgb_levels;
	RGB_LIST		rgb_bits;
};
typedef struct raster RASTER;

struct colinf
{
	short		pens;
	short		planes;
	short		*color_vdi2hw;
	short		*color_hw2vdi;
	char		*pixelformat;
	long		*pixelvalues;		/* Pixelvalues, as they're written into video-ram */
	RGB_LIST	*request_rgb;		/* Requested, relative RGB values (0 - 1000) */
	RGB_LIST	*actual_rgb;		/* Actual RGB values, used to construct pixelvalues */
//	RGB_LIST	*rgb_levels;		/* Number of levels for Red, Green, Blue, Alpha and Ovl parts */
//	RGB_LIST	*rgb_bits;		/* Number of bits used for Red, Green, Blue, Alpha and Ovl */
};
typedef struct colinf COLINF;

struct drawinf
{
	RASTER		*r;
	COLINF		*c;
};
typedef struct drawinf DRAWINF;

struct pattern_attribs
{
	short		expanded;		/* True if the pattern is expanded and valid. */
	short		color[4];		/* wrmode is used as index into color array */
	short		bgcol[4];		/* background color */
	short		width;			/* Width of pattern in pixels */
	short		height;			/* Height of pattern in pixels */
	short		wwidth;			/* Width of fill pattern in words */
	short		planes;			/* True if fill is multiplane */
	short		wrmode;			/* writing mode (unused as of yet) */
	unsigned short	mask;
	unsigned short *data;
	unsigned short *exp_data;
};
typedef struct pattern_attribs PatAttr;

struct	currfont
{
	short	defid;
	short	defht;
	struct	xgdf_head *current;
	struct	font_head *header;
	struct	xgdf_head *loaded;
	short	color;
	short	bgcol;
	short	wrmode;
	short	style;
	short	angle;
	short	fx_x_ofst;
	short	fx_y_ofst;
	short	pts;			/* true if height set in points */
	short	size;			/* in height or points */
	short	chup;			/* Character Up vector */
	short	lcount;			/* number of loaded fonts */
	short	num;			/* Total number of faces */
	short	halign;
	short	valign;
	short	scaled;
	short	sclsts;			/* True if scaling up */
	struct	font_head *scratch_head;
};
typedef struct currfont CURRFONT;

struct	fill_attribs
{
	PatAttr	*ptrn;
	short	color;
	short	bgcol;
	short	interior;
	short	perimeter;
	short	style;
};

struct line_attribs
{
	PatAttr		*ptrn;
	short		color;
	short		bgcol;
	short		wrmode;
	short		index;
	unsigned short	data;
	unsigned short	ud;

	short		width;
	short		beg;
	short		end;
};
typedef struct line_attribs LINE_ATTRIBS;

struct pmarker_attribs
{
	PatAttr		*ptrn;
	short		color;
	short		bgcol;
	short		wrmode;
	short		type;
	short		data;
	unsigned short	ud;

	short		width;
	short		height;
	short		scale;	/* Current scale factor for marker data */
};

struct pattern_data
{
	unsigned short data[(2*16) * 16];
	unsigned short edata[(2*16) * 16];
};
	
struct virtual
{
	short			func;
	short			handle;
	short			pid;
	char			procname[32];
	short			dda_inc;		/* Fraction to be added to the DDA	*/
	char			*scratchp;		/* Pointer to text scratch buffer	*/
	short			scratchs;		/* Offset to large text buffer		*/

	struct xgdf_head	*fring;
	struct currfont		font;

	struct fill_attribs	fill;
	struct pattern_attribs	pattern;

	struct line_attribs	perimeter;
	struct pattern_attribs	perimdata;

	struct pattern_attribs	udpat;
	struct pattern_data	patdata;
	struct pattern_data	udpatdata;

	struct line_attribs	line;
	struct pattern_attribs	linedat;

	struct pmarker_attribs	pmarker;
	struct pattern_attribs	pmrkdat;
	
#define REQ_MODE	1

	short			locator;
	short			valuator;
	short			choise;
	short			string;
	
	VIRTUAL			*lawk;			/* Physical workstation used by the console */
	VIRTUAL			*root;			/* Physical parent. NULL if this is parent */
	short			wrmode;			/* Current writing mode			*/
	short			xfm_mode;		/* Transformation mode requested	*/

	CLIPRECT		clip;
	
	struct console		*con;
	struct ovdi_driver	*driver;
	struct ovdi_driver	*physical;
	struct raster		*raster;
	struct colinf		*colinf;
	struct kbdapi		*kbdapi;
	struct mouseapi		*mouseapi;
	struct timeapi		*timeapi;

#if 0
	short			*color_vdi2hw;
	short			*color_hw2vdi;
	long			*pixelvalues;		/* Pixelvalues, as they're written into video-ram */
	RGB_LIST		*request_rgb;		/* Requested, relative RGB values (0 - 1000) */
	RGB_LIST		*actual_rgb;		/* Actual RGB values, used to construct pixelvalues */
	RGB_LIST		*rgb_levels;		/* Number of levels for Red, Green, Blue, Alpha and Ovl parts */
	RGB_LIST		*rgb_bits;		/* Number of bits used for Red, Green, Blue, Alpha and Ovl */
#endif

	long			ptsbuffsiz;
	long			spanbuffsiz;
	short			ptsbuff[PTSBUFF_SIZ >> 1];
	short			spanbuff[SPANBUFF_SIZ >> 1];

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
	short pid;
};
typedef struct ovdi_vtab OVDI_VTAB;


struct ovdi_lib
{
	int	(*getcookie)(long tag, long *ret);
};
/* *************************************************************************** */
/* *************************************************************************** */
/* *************************************************************************** */
struct rop_pb
{
	short	sx1, sy1, sx2, sy2;
	short	dx1, dy1, dx2, dy2;


	void	*s_addr;
	short	s_is_scrn;
	short	s_bypl;
	short	s_w, s_h;

	void	*d_addr;
	short	d_is_scrn;
	short	d_bypl;
	short	d_w, d_h;
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
//typedef unsigned long	(*get_pixel)(unsigned char *scrbase, short bypl, short x, short y);
//typedef void		(*raster_op)(unsigned char *srcbase, short srcbypl, unsigned char *dstbase, short dstbypl, short width, short height, short dir);

typedef raster_blit raster_blits[16];
typedef pixel_blit pixel_blits[16];


 /* The default functions here work with all color depths, and is copied
  * to the current drawers structure. Drivers contain one such structure for each 
  * color-depth mode (1, 2, 4, 8, 15, 16, 24 and 32 bit color modes) respectively.
 */
struct ovdi_drawers
{
/* REMEMBER TO UPDATE THE TABLES IN WORKSTATION WHEN MODIFYING HERE !!!!!!!!!!!!!!!!11 */ 
	void		(*draw_filledrect)	( RASTER *r, COLINF *c, VDIRECT *corners, VDIRECT *clip, PatAttr *ptrn, short fis);
	void		(*draw_arc)		( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
	void		(*draw_pieslice)	( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
	void		(*draw_circle)		( VIRTUAL *v, short xc, short yc, short xrad, short *points, PatAttr *ptrn);
	void		(*draw_ellipse)		( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short *points, PatAttr *ptrn);
	void		(*draw_ellipsearc)	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
	void		(*draw_ellipsepie)	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
	void		(*draw_rbox)		( VIRTUAL *v, short gdp_code, VDIRECT *corners, PatAttr *ptrn);
	void		(*draw_abline)		( RASTER *r, COLINF *c, struct vdirect *pnts, PatAttr *ptrn);
	void		(*draw_habline)		( RASTER *r, COLINF *c, short x1, short x2, short y, PatAttr *ptrn);
	void		(*draw_vabline)		( RASTER *r, COLINF *c, short y1, short y2, short x, PatAttr *ptrn);
	void		(*draw_wideline)	( RASTER *r, COLINF *c, short *pts, long numpts, VDIRECT *clip, short *points, long pointasize, LINE_ATTRIBS *latr, PatAttr *ptrn);
	void		(*draw_spans)		( RASTER *r, COLINF *c, short x1, short x2, short y, PatAttr *ptrn);
	void		(*draw_filledpoly)	( RASTER *r, COLINF *c, short *pts, short n, VDIRECT *clip, short *points, long pointasize, PatAttr *ptrn);
	void		(*draw_pmarker)		( RASTER *v, COLINF *c, POINT *origin, VDIRECT *clip, short type, short size, short w_in, short h_in, PatAttr *ptrn);

	void		(*rt_cpyfm)		( RASTER *r, COLINF *c, MFDB *src, MFDB *dst, short *pnts, VDIRECT *clip, short fgcol, short bgcol, short wrmode);
	void		(*ro_cpyfm)		( RASTER *r, MFDB *src, MFDB *dst, short *pnts, VDIRECT *clip, short wrmode);

 /* Functions below here is always color-depth dependant. If driver dont provide
  * these, standard and VERY, VERY slow ones are used. */
	void		(*draw_pixel)		( unsigned char *adr, long data);
	void		(*read_pixel)		( unsigned char *adr, long data);
	void		(*put_pixel)		( unsigned char *base, short bypl, short x, short y, unsigned long data);
	unsigned long	(*get_pixel)		( unsigned char *base, short bypl, short x, short y);

	void		(*draw_solid_rect)	( RASTER *r, COLINF *c, short *corners, short wrmode, short color);
	/*pixel_blits	vdi_pixels;*/
	pixel_blits	drp;
	pixel_blits	dlp;
	pixel_blits	pixel_blits;
	raster_blits	raster_blits;

	draw_mc		draw_mcurs;
	undraw_mc	undraw_mcurs;

};
typedef struct ovdi_drawers OVDI_DRAWERS;

struct ovdi_utils
{
/*  color/hardware indipendant helper functions */
	short	(*clc_nsteps)		( short xrad, short yrad);
	void	(*clc_arc)		( VIRTUAL *v, short gdp_code, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short del_ang, short n_steps, short *points, PatAttr *ptrn);

	short	(*clipbox)		( VDIRECT *corners, VDIRECT *clip);
	void	(*sortcpy_corners)	( short *source, short *dest );

	short	(*code)			( POINT *input, VDIRECT *clip);
	short	(*clip_line)		( VDIRECT *input, VDIRECT *clip);

	short	(*fix_raster_coords)	( short *spts, short *dpts, short *c);

	void	(*trnfm)		( VIRTUAL *v, MFDB *src, MFDB *dst);

	void	(*cnv_v2d_1b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_v2d_2b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_v2d_4b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_v2d_8b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_v2d_15b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_v2d_16b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_v2d_24b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_v2d_32b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	long	cnv_v2d_rsv[4];

	void	(*cnv_d2v_1b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_d2v_2b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_d2v_4b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_d2v_8b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_d2v_15b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_d2v_16b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_d2v_24b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	void	(*cnv_d2v_32b)		( unsigned short *src, unsigned short *dst, unsigned long splen);
	long	cnv_d2v_rsv[4];
};
typedef struct ovdi_utils OVDI_UTILS;

#endif	/* _OVDI_DEFS_H */
