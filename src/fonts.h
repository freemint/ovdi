#ifndef _OVDIFONTS_H
#define _OVDIFONTS_H

#include "ovdi_types.h"
#include "gdf_defs.h"

long		load_font		( char *fn, long *flen, long *fadr, struct gdf_membuff *m);
void		fixup_font		( FONT_HEAD *font );
void		init_systemfonts	( SIZ_TAB *st, DEV_TAB *dt);
GDF_CACHED *	gdf_get_cachemem	( XGDF_HEAD *xf, O_Int chr);
void		gdf_free_cache		( void );
O_Int		add_font		( XGDF_HEAD *start, XGDF_HEAD *new );
O_Int		find_fontbyindex	( XGDF_HEAD *start, O_Int index, long *ret);
O_Int		find_fontbyid		( XGDF_HEAD *start, O_Int id, long *ret);
O_Int		find_fontbyidindex	( XGDF_HEAD *start, O_Int id, O_Int index, long *ret );
O_Int		find_fontbypoint	( XGDF_HEAD *start, O_Int id, O_Int point, long *ret );
O_Int		find_fontbyheight	( XGDF_HEAD *start, O_Int id, O_Int point, long *ret );
O_Int		find_fontbycharwidth	( XGDF_HEAD *start, O_Int id, O_Int width, long *ret );
O_Int		find_fontbyname		( XGDF_HEAD *start, char *name, long *ret );

void		get_font_fullname	( XGDF_HEAD *f, char *n);
void		get_font_familyname	( XGDF_HEAD *f, char *n);
void		get_font_stylename	( XGDF_HEAD *f, char *n);
void		get_font_filename	( XGDF_HEAD *f, char *n);

void		get_font_chrfx_ofst	( FONT_HEAD *f, O_Int style, O_16 *ret_x, O_16 *ret_y );
O_Int		gdf_string_width	( FONT_HEAD *f, O_16 *string, O_Int len);
O_Int		gdf_get_so		( FONT_HEAD *f, O_16 *string, O_Int len, O_Pos pixeloffset, O_Pos chrfx, GDFSO *inf);

#endif	/* _OVDIFONTS_H */
