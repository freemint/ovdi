#include "display.h"
#include "gdf_text.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "fonts.h"
#include "vdi_font.h"
#include "v_text.h"
#include "v_attribs.h"

/*
 * Release resources related to fonts...
*/
void
lvst_exit(VIRTUAL *v)
{
	XGDF_HEAD *xf = v->font.current;

	if (xf)
	{
		xf->links--;
	}
}

void
vst_load_fonts( VDIPB *pb, VIRTUAL *v)
{
	if (v->font.loaded)
		pb->intout[0] = v->font.lcount;
	else if (v->root)
	{
		v->font.loaded = v->root->font.loaded;
		v->font.lcount = v->root->font.lcount;
		pb->intout[0] = v->font.lcount;
	}

	pb->contrl[N_INTOUT] = 1;
}

short
lvst_load_fonts( VIRTUAL *v)
{
	if (!v->font.loaded)
	{
		v->font.loaded = v->root->font.loaded;
		v->font.lcount = v->root->font.lcount;
	}
	return v->font.lcount;
}

void
vst_unload_fonts( VDIPB *pb, VIRTUAL *v)
{
	if (v->root)
	{
		v->font.loaded = 0;
		v->font.lcount = 0;
	}
}

void
lvst_wrmode( VIRTUAL *v, short wrmode)
{
	set_writingmode( wrmode, &v->font.wrmode);
}

void
vst_alignment( VDIPB *pb, VIRTUAL *v)
{
	lvst_alignment( v, pb->intin[0], pb->intin[1]);

	pb->intout[0] = v->font.halign;
	pb->intout[1] = v->font.valign;

	pb->contrl[N_INTOUT] = 2;
}

void
lvst_alignment( VIRTUAL *v, short halign, short valign)
{
	v->font.halign = halign > MAX_HALIGN ? MAX_HALIGN : halign;
	v->font.valign = valign > MAX_VALIGN ? MAX_VALIGN : valign;
}

void
vst_color( VDIPB *pb, VIRTUAL *v)
{
	lvst_color( v, pb->intin[0]);
	pb->intout[0] = v->colinf->color_hw2vdi[v->font.color];
	pb->contrl[N_INTOUT] = 1;
}

void
lvst_color( VIRTUAL *v, short color)
{
	short maxcolor;

	maxcolor = v->colinf->pens;
	if (color < 0)
		color = 0;
	else if (color >= maxcolor)
		color = maxcolor - 1;

	v->font.color = v->colinf->color_vdi2hw[color];
}

void
vst_effects( VDIPB *pb, VIRTUAL *v)
{
	lvst_effects( v, pb->intin[0] );
	pb->intout[0] = v->font.style;

	pb->contrl[N_INTOUT] = 1;
}

void
lvst_effects( VIRTUAL *v, short style)
{
	v->font.style = style & F_SUPPORTED;
	get_font_chrfx_ofst(v->font.header, v->font.style, &v->font.fx_x_ofst, &v->font.fx_y_ofst);
}

void
vst_font( VDIPB *pb, VIRTUAL *v)
{
	if ( (lvst_font( v, pb->intin[0])) )
		pb->intout[0] = v->font.header->id;
	else
		pb->intout[0] = 0;

	pb->contrl[N_INTOUT] = 1;
}

static void
setfont(VIRTUAL *v, XGDF_HEAD *xf)
{
	XGDF_HEAD *cf = v->font.current;

	if (cf)
	{
		if (cf != xf)
			cf->links--, xf->links++;
	}
	else
		xf->links++;

	v->font.header	= xf->font_head;
	v->font.current	= xf;
	v->font.scaled	= 0;
	v->font.sclsts	= 0;
	get_font_chrfx_ofst(xf->font_head, v->font.style, &v->font.fx_x_ofst, &v->font.fx_y_ofst);
}


short
lvst_font( VIRTUAL *v, short id)
{
	XGDF_HEAD *sf, *lf, *cf;
	int sr, lr;

	lr = -1;
	cf = v->font.current;

	/* fint_fontbyxxx() returns 1 if perfect match found,	*/
	/* 2 if ID found, but scaling necessary (closest sized	*/
	/* font returned), or 0 if font with ID not found	*/
	if ( v->font.pts )
		sr = find_fontbypoint( v->fring, id, v->font.size, (long *)&sf);
	else
		sr = find_fontbyheight( v->fring, id, v->font.size, (long *)&sf);

	if (sr == 1)
	{
		setfont(v, sf);
		return 1;
	}

	if (v->font.loaded)
	{
		if (v->font.pts)
			lr = find_fontbypoint( v->font.loaded, id, v->font.size, (long *)&lf);
		else
			lr = find_fontbyheight( v->font.loaded, id, v->font.size, (long *)&lf);

		if (lr == 1)
		{
			setfont(v, lf);
			return 1;
		}
	}
	
	if (sr != 2 && lr == 2)
	{
		sr = lr;
		sf = lf;
	}

	if (sr == 2)
	{
	/* scale font here */
		v->font.header	= sf->font_head;
		v->font.current	= sf;
		if (cf)
		{
			if (cf != sf)
				cf->links--, sf->links++;
		}
		else
			sf->links++;
		if (v->font.pts)
			(void)lvst_point( v, v->font.size );
		else
			(void)lvst_height( v, v->font.size );
		return 1;
	}
	return 0;
}

void
vst_point( VDIPB *pb, VIRTUAL *v )
{
	if ( (lvst_point( v, pb->intin[0] )) )
	{
		int hcell;

		pb->intout[0] = v->font.size;
		pb->ptsout[0] = v->font.header->max_char_width;
		pb->ptsout[1] = hcell = v->font.header->top;
		pb->ptsout[2] = v->font.header->max_cell_width;
		pb->ptsout[3] = hcell + v->font.header->bottom + 1;
	}
	else
	{
		pb->intout[0] = 0;
	}

	pb->contrl[N_INTOUT] = 1;
	pb->contrl[N_PTSOUT] = 2;
}

short
lvst_point ( VIRTUAL *v, short point )
{
	XGDF_HEAD *sf, *lf, *cf;
	//FONT_HEAD *fnth;
	int sr, lr;

	cf = v->font.current;
	lr = 0;
	sr = find_fontbypoint(v->fring, v->font.header->id, point, (long *)&sf );
	if (sr == 1)
	{
		
		setfont(v, sf);
		v->font.size = v->font.header->point;
		return 1;
	}

	if (v->font.loaded)
		lr = find_fontbypoint(v->font.loaded, v->font.header->id, point, (long *)&lf);

	if (lr == 1)
	{
		setfont(v, lf);
		v->font.size = v->font.header->point;
		return 1;
	}

	if (sr != 2 && lr == 2)
	{
		sr = lr;
		sf = lf;
	}

	if (sr ==  2)
	{
		setfont(v, sf);
		v->font.size = v->font.header->point;
		return 1;
	}

	return 0;
}

void
vst_height( VDIPB *pb, VIRTUAL *v )
{

	if ( (lvst_height( v, pb->ptsin[1])) )
	{
		int hcell;

		pb->ptsout[0] = v->font.header->max_char_width;
		pb->ptsout[1] = hcell = v->font.header->top;
		pb->ptsout[2] = v->font.header->max_cell_width;
		pb->ptsout[3] = hcell + v->font.header->bottom + 1;
	}
	else
	{
		display("vst_height: This should never happen!");
	}

	pb->contrl[N_INTOUT] = 0;
	pb->contrl[N_PTSOUT] = 2;
}

short
lvst_height ( VIRTUAL *v, short height )
{
	XGDF_HEAD *sf, *lf, *cf;
	//FONT_HEAD *fnth;
	int sr, lr;

	lr = 0;
	cf = v->font.current;

	sr = find_fontbyheight(v->fring, v->font.header->id, height, (long *)&sf );
	if (sr == 1)
	{
		setfont(v, sf);
		v->font.size = v->font.header->top;
		return 1;
	}

	if (v->font.loaded)
		lr = find_fontbyheight(v->font.loaded, v->font.header->id, height, (long *)&lf);

	if (lr == 1)
	{
		setfont(v, lf);
		v->font.size = v->font.header->top;
		return 1;
	}

	if (sr != 2 && lr == 2)
	{
		sr = lr;
		sf = lf;
	}

	if (sr ==  2)
	{
		setfont(v, sf);
		v->font.size = v->font.header->top;
		return 1;
	}

	return 0;
}

void
vst_rotation( VDIPB *pb, VIRTUAL *v)
{
	lvst_rotation( v, pb->intin[0] );
	pb->intout[0] = v->font.angle;

	pb->contrl[N_INTOUT] = 1;
}

void
lvst_rotation( VIRTUAL *v, short angle)
{

	v->font.angle = 0;
#if 0
	if (angle < 450)
		v->font.angle = 0;
	else if (angle < 900 + 450)
		v->font.angle = 900;
	else if (angle < 1800 + 450)
		v->font.angle = 1800;
	else if (angle < 2700 + 450)
		v->font.angle = 2700;
	else
		v->font.angle = 3600;

#endif
}

void
v_gtext( VDIPB *pb, VIRTUAL *v)
{
	int slen;

	if (!(slen = pb->contrl[N_INTIN]))
		return;

	output_gdftext( v, (POINT *)&pb->ptsin[0], (short *)&pb->intin[0], slen,
			0, 0, 0);
}

void
vqt_attributes( VDIPB *pb, VIRTUAL *v)
{
	FONT_HEAD *f;
	int hcell;

	f = v->font.header;

	pb->intout[0] = f->id;
	pb->intout[1] = v->colinf->color_hw2vdi[v->font.color];
	pb->intout[2] = v->font.angle;
	pb->intout[3] = v->font.halign;
	pb->intout[4] = v->font.valign;
	pb->intout[5] = v->wrmode + 1;	/* BUGCOMPATIBILITY! Original VDI returns wrmode -1 here .. so we do that as well */

	pb->ptsout[0] = f->max_char_width;
	pb->ptsout[1] = hcell = f->top;
	pb->ptsout[2] = f->max_cell_width;
	pb->ptsout[3] = hcell + f->bottom + 1;

	pb->contrl[N_INTOUT] = 6;
	pb->contrl[N_PTSOUT] = 2;
}

void
vqt_extent( VDIPB *pb, VIRTUAL *v)
{
	int angle, width, height;
	short *points;
	FONT_HEAD *f;

	f = v->font.header;
	width = gdf_string_width(f, (short *)&pb->intin[0], pb->contrl[N_INTIN]);
	height = f->top + f->bottom + 1;
	angle = v->font.angle;
	points = (short *)&pb->ptsout[0];

	switch (angle)
	{
		case 0:
		{
			*points++ = 0;
			*points++ = 0;

			*points++ = width;
			*points++ = 0;

			*points++ = width;
			*points++ = height;

			*points++ = 0;
			*points	= height;
			break;
		}
		case 900:
		{
			*points++ = height;
			*points++ = 0;

			*points++ = height;
			*points++ = width;

			*points++ = 0;
			*points++ = width;

			*points++ = 0;
			*points	= 0;
			break;
		}
		case 1800:
		{
			*points++ = width;
			*points++ = height;

			*points++ = 0;
			*points++ = height;

			*points++ = 0;
			*points++ = 0;

			*points++ = width;
			*points	= 0;
			break;
		}
		case 2700:
		{
			*points++ = 0;
			*points++ = height;

			*points++ = 0;
			*points++ = 0;

			*points++ = height;
			*points++ = 0;

			*points++ = width;
			*points	= height;
			break;
		}
	}
	pb->contrl[N_PTSOUT] = 4;
}

void
vqt_fontinfo( VDIPB *pb, VIRTUAL *v)
{
	FONT_HEAD *f = v->font.header;

	pb->intout[0] = f->first_ade;
	pb->intout[1] = f->last_ade;

	pb->ptsout[0] = f->max_cell_width;
	pb->ptsout[1] = f->bottom;			/* Dist 0 */
	pb->ptsout[2] = v->font.style & F_THICKEN ? f->thicken : 0;	/* Effx 0 */
	pb->ptsout[3] = f->descent;			/* Dist 1 */

	if (v->font.style & F_SKEW)
	{
		pb->ptsout[4] = f->left_offset;		/* Effx 1 */
		pb->ptsout[6] = f->right_offset;	/* Effx 2 */
	}
	else
		pb->ptsout[4] = pb->ptsout[6] = 0;	/* Effx 1 & Effx 2 */

	pb->ptsout[5] = f->half;			/* Dist 2 */
	pb->ptsout[7] = f->ascent;			/* Dist 3 */
	pb->ptsout[8] = 0;
	pb->ptsout[9] = f->top;				/* Dist 4 */

	pb->contrl[N_PTSOUT] = 5;
	pb->contrl[N_INTOUT] = 2;
}

/*
 * vqt_name()		function 130, subfunction 0	TOS
 * vqt_ext_name()	function 130, subfunction 1	NVDI 3.00
*/
void
vqt_name( VDIPB *pb, VIRTUAL *v)
{
	int i;
	unsigned char *sfname;
	short *dfname;
	int index;
	FONT_HEAD *f;
	XGDF_HEAD *xf;

	index = find_fontbyindex( v->fring, pb->intin[0], (long *)&xf);

	if (index)
	{
		if (v->font.loaded)
		{
			index = find_fontbyindex( v->font.loaded, index, (long *)&xf);
			if (index)
				xf = v->fring;
		}
		else
			xf = v->fring;
	}

	f = xf->font_head;
	dfname = (short *)&pb->intout[1];
	for (i = 0, sfname = (unsigned char *)&f->name, dfname = (short *)&pb->intout[1]; (*dfname++ = *sfname++); i++){}
	while ( i < 32 ) { *dfname++ = 0; i++; }

	if (pb->contrl[SUBFUNCTION] == 1)
	{
		int flags = 0, ff = f->flags;

		pb->intout[33] = 0;
		if (ff & F_MONOSPACE)
			flags |= FF_MONOSPACED << 8;

		pb->intout[34] = flags;

		pb->contrl[N_INTOUT] = 35;
	}
	else
	{
		pb->contrl[N_INTOUT] = 33;
	}
	pb->intout[0] = f->id;
}
/*
 * vqt_width()		function 117, subfunction x	TOS
*/
void
vqt_width( VDIPB *pb, VIRTUAL *v)
{
	unsigned short chr;
	short *offtab;
	char *hortab;
	FONT_HEAD *f = v->font.header;

	chr = (pb->intin[0] & 0x00ff);

	if (chr > 253 || chr < f->first_ade || chr > f->last_ade)
		pb->intout[0] = -1;
	else
	{
		pb->intout[0] = chr;

		offtab = f->off_table;
		hortab = f->flags & F_HORZ_OFF ? f->hor_table : 0;

		chr -= f->first_ade;
		pb->ptsout[0] = offtab[chr + 1] - offtab[chr];
		
		if (hortab)
		{
			pb->ptsout[1] = hortab[chr];
			pb->ptsout[2] = hortab[chr + 1];
		}
		else
			pb->ptsout[1] =	pb->ptsout[2] = 0;
	}

	pb->contrl[N_INTOUT] = 1;
	pb->contrl[N_PTSOUT] = 3;
}
/*
 * vqt_xfntinfo()	function 299, subfunction x	NVDI 3.02
*/
void
vqt_xfntinfo( VDIPB *pb, VIRTUAL *v)
{
	struct in
	{
		short flags, id, index;
		XFNT_INFO *info;
	};
	struct out
	{
		short fmt, id, index;
	};
	XGDF_HEAD *xf = 0;
	FONT_HEAD *f;
	struct in *i = (struct in *)&pb->intin[0];
	struct out *o = (struct out *)&pb->intout[0];
	short flags, index;
	XFNT_INFO *xfi = i->info;

	if (xfi->size != sizeof(XFNT_INFO))
		return;
	
	if (i->index)
	{
		index = find_fontbyindex( v->fring, i->index, (long *)&xf);
		if (index)
		{
			if (v->font.loaded)
				index = find_fontbyindex( v->font.loaded, index, (long *)&xf);
			if (index)
			{
				xf = v->fring;
				index = 1;
			}
		}
		if (!index)
			index = i->index;
	}
	else if (i->id)
	{
		index = find_fontbyid( v->fring, i->id, (long *)&xf);
		if (!index)
		{
			if (v->font.loaded)
				index = find_fontbyid( v->font.loaded, i->id, (long *)&xf);

			if (!index)
			{
				xf = v->fring;
				index = 1;
			}
		}
	}
	else
	{
		index = 1;
		xf = v->fring;
	}

	flags = i->flags;
	f = xf->font_head;
	xfi = i->info;
	if (flags & XFNT_NAME)
		get_font_fullname(xf, (char *)&xfi->font_name);
	if (flags & XFNT_FAMILYNAME)
		get_font_familyname(xf, (char *)&xfi->family_name);
	if (flags & XFNT_STYLENAME)
		get_font_stylename(xf, (char *)&xfi->style_name);
	if (flags & XFNT_FILENAME1)
		get_font_filename(xf, (char *)&xfi->file_name1);
	if (flags & XFNT_FILENAME2)
		get_font_filename(xf, (char *)&xfi->file_name2);
	if (flags & XFNT_FILENAME2)
		get_font_filename(xf, (char *)&xfi->file_name2);

	if (flags & XFNT_HEIGHT)
	{
		XGDF_HEAD *xfnt;
		int cnt, id = f->id;
		
		for (cnt = 0;cnt < 64; cnt++)
		{
			if ( find_fontbyidindex(v->fring, id, cnt + 1, (long *)&xfnt) )
				xfi->pt_sizes[cnt++] = xfnt->font_head->point;
			else
				break;
		}
		xfi->pt_cnt = cnt;
	}
	else if (flags & XFNT_DHEIGHT)
	{
		xfi->pt_cnt = 0;
	}

	xfi->format = FT_BITMAP;

	o->fmt = 1;
	o->id = xf->font_head->id;
	o->index = index;
}

/*
 * Helper function
*/
static short
get_fontname(short *src, char *dst, short len)
{
	int chr, lchr = 0;
	char *d = dst;

	while (*src == 32 && len){src++; len--;}

	while (len > 0)
	{
		chr = *src++;
		*dst++ = chr;
		if (!chr)
		{
			if (lchr == 32)
				*--dst = 0;
			break;
		}
		len--;
		if (chr == 32)
		{
			while (*src == 32 && len){src++; len--;}
		}
		lchr = chr;
	}
	return (short)(long)dst - (long)d;
}

/*
 * Helper function
*/
static short
put_fontname(char *src, unsigned short *dst, short len)
{
	unsigned short chr, lchr = 0;
	unsigned short *d = dst;

	while (*src == 32 && len){src++; len--;}

	while (len > 0)
	{
		chr = *src++;
		*dst++ = chr;
		if (!chr)
		{
			if (lchr == 32)
				*--dst = 0;
			break;
		}
		len--;
		if (chr == 32)
		{
			while (*src == 32 && len){src++; len--;}
		}
		lchr = chr;
	}
	return (short)((long)dst - (long)d) >> 1;
}

/*
 * vst_name()		function 230, subfunction 0	NVDI 3.02
 * vqt_name_and_id()	function 230, subfunction 100	NVDI 3.02
*/
void
vst_name( VDIPB *pb, VIRTUAL *v)
{

	int format = pb->intin[0], found = 0, nlen = pb->contrl[N_INTIN - 1];
	char nameb[200];
	char *name = (char *)&nameb[0];
	short *dname = (short *)&pb->intin[1];
	XGDF_HEAD *xf;

	(void)get_fontname(dname, name, nlen);

	if (format == FT_BITMAP)
	{
		if ( find_fontbyname(v->fring, name, (long *)&xf) )
			xf = v->fring;
		else
			found = 1;
	}
	else if (format == FT_SPEEDO)
		xf = v->fring;
	else if (format == FT_TRUETYPE)
		xf = v->fring;
	else if (format == FT_TYPE1)
		xf = v->fring;
	else
		xf = v->fring;

	if (pb->contrl[SUBFUNCTION] == 100)
	{
		if (found)
		{
			pb->intout[0] = xf->font_head->id;
			pb->contrl[N_INTOUT] = put_fontname((char *)&xf->font_head->name, (unsigned short *)dname, 200) + 1;
		}
		else
			pb->intout[0] = 0;
	}
	else
	{
		lvst_font( v, xf->font_head->id);
		pb->intout[0] = xf->font_head->id;
		pb->contrl[N_INTOUT] = put_fontname((char *)&xf->font_head->name, (unsigned short *)dname, 200) + 1;
	}
}

/*
 * vst_width()		function 231, subfunction x	NVDI 3.00
*/
void
vst_width( VDIPB *pb, VIRTUAL *v )
{

	if ( (lvst_width( v, pb->ptsin[0])) )
	{
		int hcell;

		pb->ptsout[0] = v->font.header->max_char_width;
		pb->ptsout[1] = hcell = v->font.header->top;
		pb->ptsout[2] = v->font.header->max_cell_width;
		pb->ptsout[3] = hcell + v->font.header->bottom + 1;
	}
	else
	{
		scrnlog("vst_width: This should never happen!");
	}

	pb->contrl[N_INTOUT] = 0;
	pb->contrl[N_PTSOUT] = 2;
}

short
lvst_width ( VIRTUAL *v, short width )
{
	XGDF_HEAD *sf, *lf, *cf;
	int sr, lr;

	lr = 0;
	cf = v->font.current;

	sr = find_fontbycharwidth(v->fring, v->font.header->id, width, (long *)&sf );
	if (sr == 1)
	{
		setfont(v, sf);
		v->font.size = v->font.header->top;
		return 1;
	}

	if (v->font.loaded)
		lr = find_fontbycharwidth(v->font.loaded, v->font.header->id, width, (long *)&lf);

	if (lr == 1)
	{
		setfont(v, lf);
		v->font.size = v->font.header->top;
		return 1;
	}

	if (sr != 2 && lr == 2)
	{
		sr = lr;
		sf = lf;
	}

	if (sr ==  2)
	{
		setfont(v, sf);
		v->font.size = v->font.header->top;
		return 1;
	}

	return 0;
}
