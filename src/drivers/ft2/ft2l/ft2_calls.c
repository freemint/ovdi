#include <ft2build.h>
#include FT_FREETYPE_H

#include "ft2_calls.h"
#include "vdi_font.h"

int
main(void)
{
	return 0;
}

/*
*/ 
long
open(void)
{
	long error;
	FT_Library lib_handle;

	error = FT_Init_FreeType( &lib_handle );

	if (error)
		return 0;
	else
		return (long)lib_handle;
}

long
open_face(long lib_handle, char *filename, long ind, struct vf_face *vff)
{
	long error;
	FT_Face face;

	error	= FT_New_Face( (FT_Library)lib_handle, filename, ind, &face);

	if (error)
	{
		return -1;
	}
	else
	{
		vff->face_handle = (long)face;
		get_face_info(vff);
		return 0;
	}
}
static void
set_active_vdi_face_size(struct vf_face *vff)
{
	FT_Face face = (FT_Face)vff->face_handle;

	vff->size.x_ppem	= face->size->x_ppem;
	vff->size.y_ppem	= face->size->y_ppem;
	vff->size.x_scale	= face->size->x_scale;
	vff->size.y_scale	= face->size->y_scale;
	vff->size.ascender	= face->size->ascender;
	vff->size.descender	= face->size->descender;
	vff->size.height	= face->size->height;
	vff->size.max_advance	= face->size->max_advance;		
}

static void
set_vdi_face_metrics(struct vf_face *vff)
{
	FT_Face face = (FT_Face)vff->face_handle;

	if (face->face_flags & FT_FACE_FLAG_SCALABLE)
	{
		vff->vf_bbox		= face->bbox;
		vff->units_per_em	= face->units_per_em;
		vff->ascender		= face->ascender;
		vff->descender		= face->descender;
		vff->height		= face->height;
		vff->max_advance_width	= face->max_advance_width;
		vff->max_advance_height	= face->max_advance_height;
		vff->underline_position	= face->underline_position;
		vff->underline_thickness = face->underline_thickness;
	}
	set_active_vdi_face_size(vff);
}

void
get_face_info(struct vf_face *vff)
{
	FT_Face face = (FT_Face)vff->face_handle;
	int i;
	char *s;

	vff->num_faces		= face->num_faces;
	vff->face_index		= face->face_index;
	vff->face_flags		= face->face_flags;
	vff->style_flags	= face->style_flags;
	vff->num_glyphs		= face->num_glyphs;

	for (i = 0, *s = face->family_name;  i < 200 && (vff->family_name[i] = *s++); i++){}
	for (i = 0, *s = face->family_style; i < 200 && (vff->family_style[i] = *s++); i++){}
	
	vff->num_fixed_sizes	= face->num_fixed_sizes;
	vff->num_charmaps	= face->num_charmaps;

	set_vdi_face_metrics(face, vff);
}

void
set_char_size(struct vf_face *vff, long width, long height, long horz_res, long vert_res)
{
	FT_Set_Char_Size((FT_Face)vff->face_handle, width, height, horz_res, vert_res);
	set_active_vdi_face_size(vff);
}
	
void
set_pixel_size(struct vf_face *vff, long width, long height)
{
	FT_Set_Pixel_Sizes( (FT_Face)vff->face_handle, width, height);
	set_active_vdi_face_size(vff);
}

void
get_glyph_bitmap(struct vf_face *vff, long charcode)
{
	long gi, error;
	FT_Face face = (FT_Face)vff->face_handle;

	gi	= FT_Get_Char_Index(face, charcode);
	error	= FT_Load_Glyph(face, gi, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);	
}
