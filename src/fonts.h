#ifndef _OVDIFONTS_H
#define _OVDIFONTS_H

#include "vdi_defs.h"

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

long	load_font		( char *fn, long *flen, long *fadr );
void	fixup_font		( FONT_HEAD *font );
void	init_systemfonts	( SIZ_TAB *st, DEV_TAB *dt);
short	add_font		( XGDF_HEAD *start, XGDF_HEAD *new );
short	find_fontbyindex	( XGDF_HEAD *start, short index, long *ret);
short	find_fontbypoint	( XGDF_HEAD *start, short id, short point, long *ret );
short	find_fontbyheight	( XGDF_HEAD *start, short id, short point, long *ret );

void	get_font_chrfx_ofst	( FONT_HEAD *f, short style, short *ret_x, short *ret_y );
short	gdf_string_width	( FONT_HEAD *f, short *string, short len);
short	gdf_get_so		( FONT_HEAD *f, short *string, short len, short pixeloffset, short chrfx, GDFSO *inf);

#endif	/* _OVDIFONTS_H */
