#ifndef _vdi_defs_h
#define _vdi_defs_h

#define PTSBUFF_SIZ		1024*4			/* Size of PTSBUFF */
#define SPANBUFF_SIZ		(400*5*2) << 1		/* Buffer used by polygon drawers */
#define MAX_COLOR		16
#define MAX_LN_STYLE		 7
#define MAX_LN_WIDTH		40
#define MAX_LN_ENDS		 2
//#define MAX_MARK_INDEX		 6
#define MIN_PMARKERTYPE		 1
#define MAX_PMARKERTYPE		 6
#define MAX_FONT		 1
#define MAX_FIL_STYLE		 4
#define MAX_FIL_HAT_INDEX	12
#define MAX_FIL_PAT_INDEX	24
#define MAX_MODE		 3
#define MAX_ARC_CT		(PTSBUFF_SIZ >> 1) / 3 /*70*/	/* maximum number of points on circle */

/* wrmode definitions */
#define MD_REPLACE	1
#define MD_TRANS	2
#define MD_XOR		3
#define MD_ERASE	4
#define MIN_MD_MODE	1
#define MAX_MD_MODE	4

/* Line definitions */
#define LI_USER		7
#define MAX_L_WIDTH	64

/* Line End definitions */
#define LE_SQUARE	0
#define LE_ARROW	1
#define LE_ROUND	2

/* Coordinate pair sorting modes (corners) */
#define LLUR		0	/* Lower Left to Upper Right */
#define ULLR		1	/* Upper Left to Lower Right */

/* Fill interior styles.. */
#define FIS_HOLLOW	0
#define FIS_SOLID	1
#define FIS_PATTERN	2
#define FIS_HATCH	3
#define FIS_USER	4
#define MAX_FIS		4

/* Defines for CONTRL[] */
#define FUNCTION	0
#define N_PTSIN		1
#define N_PTSOUT	2
#define N_INTIN		3
#define N_INTOUT	4
#define SUBFUNCTION	5
#define HANDLE		6

/* vq_color modes */
#define COLOR_REQUESTED	0
#define COLOR_ACTUAL	1

#define v_bas_ad	*(long *)(0x43eL)

#if 0
short contrl[128];
short intin[1024];
short ptsin[1024];
short intout[1024];
short ptsout[1024];
#endif

struct vdipb
{
	short		*contrl;
	short		*intin;
	short		*ptsin;
	short		*intout;
	short		*ptsout;
};
typedef struct vdipb VDIPB;



/* font header defs */

#define F_DEFAULT	1	/* this is the default font (face and size) */
#define F_HORZ_OFF	2	/* there are left and right offset tables */
#define F_STDFORM	4	/* is the font in standard format */
#define F_MONOSPACE	8	/* is the font monospaced */

/* style bits */

#define F_THICKEN	1
#define F_LIGHT		2
#define F_SKEW		4
#define F_UNDER		8
#define F_OUTLINE	16
#define F_SHADOW	32

#define F_SUPPORTED	0

#define MAX_HALIGN	2
#define MAX_VALIGN	5

/* VDI Memory Form Definition Block */
struct mfdb
{
	void	*fd_addr;
	short	fd_w;		/* Form Width in Pixels */
	short	fd_h; 		/* Form Height in Pixels */
	short	fd_wdwidth;	/* Form Width in shorts(fd_w/sizeof(short) */
	short	fd_stand;	/* Form format 0= device spec 1=standard */
	short	fd_nplanes;	/* Number of memory planes */
	short	fd_r1;		/* Reserved */
	short	fd_r2;		/* Reserved */
	short	fd_r3;		/* Reserved */
};
typedef struct mfdb MFDB;

struct vdirect
{
	short x1;
	short y1;
	short x2;
	short y2;
};
typedef struct vdirect VDIRECT;

struct point
{
	short x;
	short y;
};
typedef struct point POINT;

struct rgb
{
	char reserved;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};
typedef struct rgb RGB;

struct rgb_list
{
	unsigned short red;
	unsigned short green;
	unsigned short blue;
	unsigned short alpha;
	unsigned short ovl;
};
typedef struct rgb_list RGB_LIST;

struct dev_tab
{
	short xres;		/* X resolution */
	short yres;		/* Y resolution */
	short noscale;		/* Device precision 0=excat, 1=not exact */
	short wpixel;		/* Width of pixel in microns */
	short hpixel;		/* Height of pixel in microns */
	short cheights;		/* Number of character heights */
	short linetypes;	/* Number of linetypes */
	short linewidths;	/* Number of linewidths (0 = continuous scaling) */
	short markertypes;	/* Number of marker types */
	short markersizes;	/* Number of marker sizes (0 = continuous scaling) */
	short faces;		/* Number of faces supported */
	short patterns;		/* Number of patterns */
	short hatches;		/* Number of hatches */
	short colors;		/* Number of predefined colors/pens */
	short ngdps;		/* Number of supported GDP's */

	struct			/* List of GDP's supported */
	{
		short bar;
		short arc;
		short pie;
		short circle;
		short ellipse;
		short elliptical_arc;
		short elliptical_pie;
		short rounded_rect;
		short filled_rounded_rect;
		short justified_gtext;
	}cangdps;

	struct			/* For each GDP listed above, gdpattr indicates */
	{			/* the attributes which are applied to that GDP */
		short bar;	/* as follows; */
		short arc;
		short pie;
		short circle;
		short ellipse;
		short elliptical_arc;
		short elliptical_pie;
		short rounded_rect;
		short filled_rounded_rect;
		short justified_gtext;
	}gdpattr;

	short cancolor;		/* Color capability flag */
	short cantextrot;	/* Text rotation flag */
	short canfillarea;	/* Fill area capability flag */
	short cancellarray;	/* Cell array cabability flag */
	short palette;		/* Number of colors in palette (0 = >32767, 2 = monochrome, >2 = color) */
	short locators;		/* Number of locator devices */
	short valuators;	/* Number of valuator devices */
	short choicedevs;	/* Number of choice devices */
	short stringdevs;	/* Number of string devices */
	short wstype;		/* Workstation type (0 = Outpout, 1 = Input, 2 = In/Output, 3 = Metafile) */
};
typedef struct dev_tab DEV_TAB;

struct siz_tab
{
	short minwchar;		/* Min character width in pixels */
	short minhchar;		/* Min character height in pixels */
	short maxwchar;		/* Max character width in pixels */
	short maxhchar;		/* Max character height in pixels */
	short minwline;		/* Min line width */
	short zero5;		/* reserved */
	short maxwline;		/* Max line width */
	short xero7;		/* reserved */
	short minwmark;		/* Min marker width */
	short minhmark;		/* Min marker height */
	short maxwmark;		/* Max marker width */
	short maxhmark;		/* Max marker height */
};
typedef struct siz_tab SIZ_TAB;

struct inq_tab
{
	short screentype;	/* Type of display screen */
	short bgcolors;		/* Number of background colors */
	short textfx;		/* Text effects supported */
	short canscale;		/* Raster scaling capability flag */
	short planes;		/* Number of planes */
	short lut;		/* Lookup table flag */
	short rops;		/* Performance factor (Number of 16x16 raster operations per sec) */
	short cancontourfill;	/* v_contourfill() availability flag */
	short textrot;		/* Character rotation capability (0 = none, 1 = 90 degree incs, 2 = any angle) */
	short writemodes;	/* Number of writing modes */
	short inputmodes;	/* Highest level of input modes available (0 = none, 1 = request, 2 = Sample) */
	short textalign;	/* Text alignment capability flag */
	short inking;		/* Inking capability flag */
	short rubberbanding;	/* Rubberbanding capability flag (0 = none, 1 = lines, 2 = lines and rectangles) */
	short maxvertices;	/* Max vertices for polylines (-1 = no limit) */
	short maxintin;		/* Max length of intin array (-1 = no limit) */
	short mousebuttons;	/* Number of mouse buttons */
	short widestyles;	/* Styles available for wide lines (0 = no, 1 = yes) */
	short widemodes;	/* Writing modes available for wide lines (0 = no, 1 = yes) */
	short clipping;		/* Filled in with clip flag */
	short reserved[45-20];	/* reserved for future use */
};
typedef struct inq_tab INQ_TAB;

struct opnwk_input
{
	short	id;
	short	linetype;
	short	linecolor;
	short	markertype;
	short	markercolor;
	short	fontid;
	short	textcolor;
	short	fillinterior;
	short	fillstyle;
	short	fillcolor;
	short	coordflag;

	/* Since EdDI v1.00 */
	short	max_x;
	short	max_y;
	short	wpixel, hpixel;

	/* Since EdDI v1.10 */
	long	colors;
	short	planes;
	short	pixelformat;
	short	endian;
};

struct vscr
{
	char		name[8];
	unsigned short	version;
	short		x;
	short		y;
	short		width;
	short		height;
};
typedef struct vscr VSCR;

	
	
#endif /* _vdi_defs_h */
