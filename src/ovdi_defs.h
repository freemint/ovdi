#ifndef _OVDI_DEFS_H
#define _OVDI_DEFS_H

typedef struct virtual VIRTUAL;
typedef struct ovdi_lib OVDI_LIB;

#include "vdi_defs.h"
#include "ovdi_dev.h"

#define MAX_VIRTUALS		512
#define MAX_VDI_FUNCTIONS	140
#define SCRATCH_BUFFER_SIZE	1024

/* Flag definitions for 'flags' in struct raster */
#define R_IS_SCREEN	1
#define R_IN_VRAM	2

struct	currfont
{
	short	defid;
	short	defht;
	struct	font_head *header;
	struct	font_head *loaded;
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
	short	color;
	short	bgcol;
	short	interior;
	short	perimeter;
	short	style;
};

struct line_attribs
{
	short		color;
	short		bgcol;
	short		wrmode;
	short		index;
	unsigned short	data;
	short		width;
	short		beg;
	short		end;
	unsigned short	ud;
};

struct pmarker_attribs
{
	short	color;
	short	bgcol;
	short	wrmode;
	short	type;
	short	height;
	short	width;
	short	scale;	/* Current scale factor for marker data */
	short	data;
};

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

	struct font_head	*fring;
	struct currfont		font;

	struct ovdi_drawers	*drawers;
	struct ovdi_utils	*utils;

	struct fill_attribs	fill;
	struct pattern_attribs	pattern;
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

	short			clip_flag;		/* Clipping flag */
	VDIRECT			clip;

	struct console		*con;
	struct ovdi_driver	*driver;
	struct ovdi_driver	*physical;
	struct raster		*raster;
	struct kbdapi		*kbdapi;
	struct mouseapi		*mouseapi;
	struct timeapi		*timeapi;

	short			*color_vdi2hw;
	short			*color_hw2vdi;
	long			*pixelvalues;		/* Pixelvalues, as they're written into video-ram */
	RGB_LIST		*request_rgb;		/* Requested, relative RGB values (0 - 1000) */
	RGB_LIST		*actual_rgb;		/* Actual RGB values, used to construct pixelvalues */
	RGB_LIST		*rgb_levels;		/* Number of levels for Red, Green, Blue, Alpha and Ovl parts */
	RGB_LIST		*rgb_bits;		/* Number of bits used for Red, Green, Blue, Alpha and Ovl */

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

typedef void (*draw_mc)			(register XMFORM *xmf, register short x, register short y);
typedef void (*undraw_mc)		(register XMSAVE *xms);

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
	void (*all_white)	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*s_and_d)		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*s_and_notd)	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*s_only)		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*nots_and_d)	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*d_only)		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*s_xor_d)		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*s_or_d)		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*not_sord)	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*not_sxord)	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*not_d)		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*s_or_notd)	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*not_s)		(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*nots_or_d)	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*not_sandd)	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
	void (*all_black)	(unsigned char *src, short srcbypl, unsigned char *dst, short dstbypl, short w, short h, short dir);
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
typedef void	(*raster_blit)(unsigned char *srcbase, short srcbypl, unsigned char *dstbase, short dstbypl, short width, short height, short dir);

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
 
	void		(*draw_filledrect)	( VIRTUAL *v, VDIRECT *corners, PatAttr *ptrn);
	void		(*draw_arc)		( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
	void		(*draw_pieslice)	( VIRTUAL *v, short xc, short yc, short xrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
	void		(*draw_circle)		( VIRTUAL *v, short xc, short yc, short xrad, short *points, PatAttr *ptrn);
	void		(*draw_ellipse)		( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short *points, PatAttr *ptrn);
	void		(*draw_ellipsearc)	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
	void		(*draw_ellipsepie)	( VIRTUAL *v, short xc, short yc, short xrad, short yrad, short beg_ang, short end_ang, short *points, PatAttr *ptrn);
	void		(*draw_rbox)		( VIRTUAL *v, short gdp_code, VDIRECT *corners, PatAttr *ptrn);
	void		(*draw_abline)		( VIRTUAL *v, struct vdirect *pnts, PatAttr *ptrn);
	void		(*draw_habline)		( VIRTUAL *v, short x1, short x2, short y, PatAttr *ptrn);
	void		(*draw_wideline)	( VIRTUAL *v, short *pts, long numpts, short *points, long pointasize, PatAttr *ptrn);
	void		(*draw_spans)		( VIRTUAL *v, short x1, short x2, short y, PatAttr *ptrn);
	void		(*draw_filledpoly)	( VIRTUAL *v, short *pts, short n, short *points, long pointasize, PatAttr *ptrn);

	void		(*rt_cpyfm)		( VIRTUAL *v, MFDB *src, MFDB *dst, short *pnts, short fgcol, short bgcol, short wrmode);
	void		(*ro_cpyfm)		( VIRTUAL *v, MFDB *src, MFDB *dst, short *pts, short wrmode);

 /* Functions below here is always color-depth dependant. If driver dont provide
  * these, standard and VERY, VERY slow ones are used. */
	void		(*draw_pixel)		( unsigned char *adr, long data);
	void		(*read_pixel)		( unsigned char *adr, long data);
	void		(*put_pixel)		( unsigned char *base, short bypl, short x, short y, unsigned long data);
	unsigned long	(*get_pixel)		( unsigned char *base, short bypl, short x, short y);

	void		(*draw_solid_rect)	( RASTER *r, short *corners, PatAttr *ptrn);
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
