#ifndef	_GDF_DEFS_H
#define _GDF_DEFS_H

#include "vdi_defs.h"
	/* descibes a font */
struct font_head
{
	short		id;
	short		point;
	char		name[32];
	unsigned short	first_ade;
	unsigned short	last_ade;
	unsigned short	top;
	unsigned short	ascent;
	unsigned short	half;
	unsigned short	descent;
	unsigned short	bottom;
	unsigned short	max_char_width;
	unsigned short	max_cell_width;
	unsigned short	left_offset;	/* amount character slants left when skewed */
	unsigned short	right_offset;	/* amount character slants right */
	unsigned short	thicken;	/* number of pixels to smear */
	unsigned short	ul_size;	/* size of the underline */
	unsigned short	lighten;	/* mask to and with to lighten  */
	unsigned short	skew;	/* mask for skewing */
	unsigned short	flags;
	unsigned char	*hor_table;	/* horizontal offsets */
	unsigned short	*off_table;	/* character offsets  */
	unsigned short	*dat_table;	/* character definitions */
	unsigned short	form_width;
	unsigned short	form_height;

	struct font_head *next;		/* pointer to next font */
	unsigned short	font_seg;
};
typedef struct font_head FONT_HEAD;

struct gdf_cached
{
	struct xgdf_head	*font;
	short			chridx;
	struct mfdb		mfdb;
};
typedef struct gdf_cached GDF_CACHED;

struct	xgdf_head
{
	short			links;
	struct xgdf_head	*next;
	FONT_HEAD		*font_head;
	struct gdf_cached	*cache[256];
};
typedef struct xgdf_head XGDF_HEAD;
	
/* gemdos fonts string offsets */
/* used to calculate offsets into a text string */
struct gdfso
{
	short str_poff;
	short chr_poff;
	short str_coff;
	short width;
};
typedef struct gdfso GDFSO;

struct gdf_membuff
{
	char	*base;
	long	size;
	char	*free;
};


#endif	/* _GDF_DEFS_H */
