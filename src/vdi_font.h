#ifndef	_vdi_font_h
#define _vdi_font_h

#include "ovdi_types.h"
//#include "fontdrv_api.h"

#define F_SUPPORTED	0

#define MAX_HALIGN	2
#define MAX_VALIGN	5

/* style bits */
#define F_THICKEN	1
#define F_LIGHT		2
#define F_SKEW		4
#define F_UNDER		8
#define F_OUTLINE	16
#define F_SHADOW	32

/* Font Flag defines */
#define FF_MONOSPACED	1

/* Font Type defines */
#define FT_BITMAP	1
#define FT_SPEEDO	2
#define FT_TRUETYPE	4
#define FT_TYPE1	8

/*
 * NVDI 3.x  and above..
*/
/** structure to store information about a font */
#define XFNT_NAME	1
#define XFNT_FAMILYNAME	1<<1
#define XFNT_STYLENAME	1<<2
#define XFNT_FILENAME1	1<<3
#define XFNT_FILENAME2	1<<4
#define XFNT_FILENAME3	1<<5
#define XFNT_HEIGHT	1<<8
#define XFNT_DHEIGHT	1<<9

typedef struct
{
	long		size;		/**< length of the structure, initialize this entry before calling vqt_xfntinfo() */
	short		format;		/**< font format, e.g. 4 for TrueType */
	short		id;		/**< font ID, e.g. 6059 */
	short		index;		/**< index */
	char		font_name[50];	/**< font name, e.g. "Century 725 Italic BT" */
	char		family_name[50];/**< name of the font family, e.g. "Century725 BT" */
	char		style_name[50];	/**< name of the font style, e.g. "Italic" */
	char		file_name1[200];/**< name of the first font file, e.g. "C:\\FONTS\\TT1059M_.TTF" */
	char		file_name2[200];/**< name of the 2nd font file */
	char		file_name3[200];/**< name of the 3rd font file */
	short		pt_cnt;		/**< number of available point sizes (vst_point()), e.g. 10 */
	short		pt_sizes[64];	/**< available point sizes, e.g. { 8, 9, 10, 11, 12, 14, 18, 24, 36, 48 } */
} XFNT_INFO;


struct spd_trnfmd
{
	short	y_pos;
	short	x_scale;
	short	y_scale;
};
typedef struct spd_trnfmd SPD_TRNFMD;

struct	vf_bitmap_size
{
	short	height;
	short	width;

	long	size;

	long	x_ppem;
	long	y_ppem;
};
typedef struct vf_bitmap_size VF_BITMAP_SIZE;

struct vf_generic
{
	void	*data;
	void	(*finalizer)(void *object);
};

struct vf_bbox
{
	long	xmin, ymin;
	long	xmax, ymax;
};
typedef struct vf_bbox VF_BBOX;

struct vf_glyphslotrec
{
	long			lib_handle;
	struct vf_face		*face;
	struct vf_glyphslotrec	*next;
	unsigned long		reserved;
};	

struct vf_charmaprec
{
	struct vf_face	*face;
	long		encoding;
	unsigned short	platform_id;
	unsigned short	encoding_id;
};

struct vf_size_metrics
{
	/* Hor & Vert pixels per EM */
	unsigned short	x_ppem;
	unsigned short	y_ppem;
	/* Two scales used to convert font units to 26.6 frac. pixel coordinates */
	long		x_scale;
	long		y_scale;

	/* Metrics in 26.6 frac. pixels */
	long		top;
	long		ascent;
	long		half;
	long		descent;
	long		bottom;
	long		height;

	long		left_offset;
	long		right_offset;
	long		thicken;

	long		max_advance;

};
typedef struct vf_size_metrics VF_SIZE_METRICS;
		
struct vf_face
{
	long		lib_handle;
	long		face_handle;
	struct fontapi	*api;

	long		id;
	long		num_faces;
	long		face_index;
	long		face_flags;
	long		style_flags;
	long		num_glyphs;

	char		family_name[200];
	char		style_name[200];

	long		num_fixed_sizes;
	long		num_charmaps;

	VF_SIZE_METRICS	size;
	/*
	 * Only used with bitmap fonts
	*/
	VF_BITMAP_SIZE	*available_sizes;

	/*
	 * Only used with scalable faces
	*/
	VF_BBOX		bbox;
	unsigned short	units_per_em;
	short		ascender;
	short		descender;
	short		height;
	short		max_advance_width;
	short		max_advance_height;
	short		underline_position;
	short		underline_thickness;

	struct vf_charmaprec	charmap;
};
typedef struct vf_face VF_FACE;

struct vf_glyph_metrics
{
	long	width;
	long	height;

	long	horiBearingX;
	long	horiBearingY;
	long	horiAdvance;

	long	vertBearingX;
	long	vertBearingY;
	long	vertAdvance;

};

struct vf_glyph_bitmap
{
	struct vf_glyph_metrics	metrics;
	long			linearHoriAdvance;
	long			linearVertAdvance;
	long			advance;
	short			bitmap_left;
	short			bitmap_top;
	MFDB			mfdb;
};
	
struct speedo_header
{
	char	fmver[8];
	long	font_size;
	long	min_fontbuff_size;
	short	min_charbuff_size;
	short	header_size;
	short	src_font_id;
	short	src_font_version;
	char	src_font_name[70];
	char	mdate[10];
	char	layout_name[70];
	char	copyright[78];
	short	layout_size;
	short	nchars;
	short	first_char;
	short	kern_tracks;
	short	kern_pairs;
	char	flags;
	char	class_flags;
	char	family_flags;
	char	fontform_class_flags;
	char	short_fontname[32];
	char	short_facename[16];
	char	font_form[14];
	short	italic_angle;
	short	oru_per_em;
	short	width_wordspace;
	short	width_enspace;
	short	width_thinspace;
	short	width_figspace;
	short	fontwide_minx;
	short	fontwide_miny;
	short	fontwide_maxx;
	short	fontwide_maxy;
	short	uline_pos;
	short	uline_thickness;
	
	SPD_TRNFMD	small_caps;
	SPD_TRNFMD	display_sups;
	SPD_TRNFMD	footnote_sups;
	SPD_TRNFMD	alpha_sups;
	SPD_TRNFMD	chemical_infs;
	SPD_TRNFMD	small_nums;
	SPD_TRNFMD	small_denoms;
	SPD_TRNFMD	medium_nums;
	SPD_TRNFMD	medium_denoms;
	SPD_TRNFMD	large_nums;
	SPD_TRNFMD	large_denoms;
};	
	
	
#endif	/* _vdi_font_h */
