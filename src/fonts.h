#ifndef _OVDIFONTS_H
#define _OVDIFONTS_H

#include "gdf_defs.h"

long		load_font		( char *fn, long *flen, long *fadr, struct gdf_membuff *m);
void		fixup_font		( FONT_HEAD *font );
void		init_systemfonts	( SIZ_TAB *st, DEV_TAB *dt);
GDF_CACHED *	gdf_get_cachemem	( XGDF_HEAD *xf, short chr);
void		gdf_free_cache		( void );
short		add_font		( XGDF_HEAD *start, XGDF_HEAD *new );
short		find_fontbyindex	( XGDF_HEAD *start, short index, long *ret);
short		find_fontbypoint	( XGDF_HEAD *start, short id, short point, long *ret );
short		find_fontbyheight	( XGDF_HEAD *start, short id, short point, long *ret );

void		get_font_chrfx_ofst	( FONT_HEAD *f, short style, short *ret_x, short *ret_y );
short		gdf_string_width	( FONT_HEAD *f, short *string, short len);
short		gdf_get_so		( FONT_HEAD *f, short *string, short len, short pixeloffset, short chrfx, GDFSO *inf);

#endif	/* _OVDIFONTS_H */
