/*
 * This file contains the built-in GDF font-driver of oVDI
*/

#include "file.h"
#include "display.h"
#include "vdi_defs.h"
#include "vdi_font.h"
#include "fonts.h"
#include "gdf_defs.h"
#include "memory.h"

#include "fontdrv_api.h"
#include "ovdi_lib.h"
#include "modinf.h"
#include "ovdi_types.h"

#include "gdf_driver.h"


/****************************************************************************/
/* BEGIN external references */

extern short systemfont08[];
extern short systemfont09[];
extern short systemfont10[];

/* END external references */
/****************************************************************************/

/****************************************************************************/
/* BEGIN definition part */

#define FEF_WEOWN	1
#define FEF_FE_MALLOC	2
#define FEF_FH_MALLOC	4

struct fntent
{
	struct fntent		*nxt_face;
	struct vf_face		*vff;
	
	struct fntent		*nxt_font;
	long			flags;
	char			*fnt_name;
	FONT_HEAD		*fnt_head;
};
typedef struct fntent FNTENT;

struct vff_attach
{
	struct vf_face		*vff;
	struct fntent		*face;
	struct fntent		*font;
};
typedef struct vff_attach VFF_ATTACH;

struct libdesc;
struct libdesc
{
	struct libdesc	*next;

	long		handle;
	OVDI_LIB	*l;
	struct fontapi	*api;

	long		num_faces;
	long		num_loaded;

	FNTENT		*system_fonts;
	FNTENT		*loaded_fonts;

	FNTENT		sysfnt_08;
	FNTENT		sysfnt_09;
	FNTENT		sysfnt_10;
};

/* End definition part */
/****************************************************************************/
/* BEGIN API function definitions */

static long	load_fonts		(long handle, char *path, char *names);
static void	unload_fonts		(long handle);

static long	open 			(void);
static long	open_face		(long handle, char *fn, long ind, VF_FACE **vff);
static long	get_facebyidx		(long handle, long idx, VF_FACE **vff);
static long	get_facebyid		(long handle, long id, VF_FACE **vff);

static void	set_char_pointsize	(VF_FACE *vff, long point);
static void	set_charsize		(VF_FACE *vff, long width, long height);
static void	set_devsize		(VF_FACE *vff, long horirez, long vertrez);
static void	set_pixelsize		(VF_FACE *vff, long width, long height);

/* END API function definitions */
/****************************************************************************/

/****************************************************************************/
/* BEGIN local function definitions */

static void	setup_sysfont		(struct libdesc *ldesc);
static void	get_family_name		(VF_FACE *vff, FONT_HEAD *fh);
static void	get_style_name		(VF_FACE *vff, FONT_HEAD *fh);
static void	get_size_metrics	(FONT_HEAD *f, VF_SIZE_METRICS *m );
static int	add_loaded		(FNTENT **start, FNTENT *new);

/* END local function definitions */
/****************************************************************************/
/* BEGIN global data definition and access part */

static OVDI_LIB *l;

static char sname[] = "GDOS FNT driver";
static char lname[] = "GDOS FNT driver for oVDI";

static char pn[128] = { 0 };
static char fn[64] = { 0 };

static struct fontapi fapi =
{
	0,
	0x00000001,
	sname,
	lname,
	pn,
	fn,

	FONTTYPE_GEMDOS,

	open,
	load_fonts,
	unload_fonts,

	open_face,
	get_facebyidx,
	get_facebyid,

	set_char_pointsize,
	set_charsize,
	set_devsize,
	set_pixelsize,
};

static struct libdesc *ldlist;
static struct libdesc root_ld;

#if 0
static FNTENT *system_fonts = NULL;
static FNTENT *loaded_fonts = NULL;

static FNTENT sysfnt_08;
static FNTENT sysfnt_09;
static FNTENT sysfnt_10;
#endif
static char sysfont_fname[] = "Built in\0";

/* END global data definition and access part */
/****************************************************************************/
static void
print_faces(FNTENT **start)
{
	FNTENT *face = *start;
	return;
	
	if (face)
	{
		do
		{
			FNTENT *font = face;
			scrnlog("Face %s\n", face->fnt_head->name);
			do
			{
				scrnlog("file %s", face->fnt_name);
				scrnlog("     %s, id=%d, pnts=%d\n", font->fnt_head->name, font->fnt_head->id, font->fnt_head->point);

			} while ( (font = font->nxt_font) );
		} while ( (face = face->nxt_face) );
	}
	else
		scrnlog("No faces here!\n");
}

static void
print_vff(VF_FACE *vff)
{
	int i;
	if (!vff)
	{
		scrnlog("print_vff: Nothing to do!\n");
		return;
	}
	scrnlog("\n\n");

	scrnlog("lib_handle       %lx\n", vff->lib_handle);
	scrnlog("face_handle      %lx\n", vff->face_handle);
	scrnlog("api              %lx\n", vff->api);

	scrnlog("ID               %lx\n", vff->id);
	scrnlog("num faces        %ld\n", vff->num_faces);
	scrnlog("face index       %ld\n", vff->face_index);
	scrnlog("face flags       %lx\n", vff->face_flags);
	scrnlog("style flags      %lx\n", vff->style_flags);
	scrnlog("num glyphs       %ld\n", vff->num_glyphs);
	scrnlog("family name      %s\n",  vff->family_name ? vff->family_name : "None set");
	scrnlog("style name       %s\n",  vff->style_name ? vff->style_name : "None set");
	scrnlog("num fixed sizes  %ld\n", vff->num_fixed_sizes);
	scrnlog("num charmaps     %ld\n", vff->num_charmaps);

	scrnlog("-> size metrics\n");
	scrnlog("   x ppem   %d\n", vff->size.metrics.x_ppem);
	scrnlog("   y ppem   %d\n", vff->size.metrics.y_ppem);
	scrnlog("   x_scale  %ld.%d\n", vff->size.metrics.x_scale >> 6, vff->size.metrics.x_scale & 0x3f);
	scrnlog("   y_scale  %ld.%d\n", vff->size.metrics.y_scale >> 6, vff->size.metrics.y_scale & 0x3f);
	
	scrnlog("   top         %ld.%d\n", vff->size.metrics.top >> 6, vff->size.metrics.top & 0x3f);
	scrnlog("   ascent      %ld.%d\n", vff->size.metrics.ascent >> 6, vff->size.metrics.ascent & 0x3f);
	scrnlog("   half        %ld.%d\n", vff->size.metrics.half >> 6, vff->size.metrics.half & 0x3f);
	scrnlog("   descent     %ld.%d\n", vff->size.metrics.descent >> 6, vff->size.metrics.descent & 0x3f);
	scrnlog("   bottom      %ld.%d\n", vff->size.metrics.bottom >> 6, vff->size.metrics.bottom & 0x3f);
	scrnlog("   height      %ld.%d\n", vff->size.metrics.height >> 6, vff->size.metrics.height & 0x3f);
	scrnlog("   left of     %ld.%d\n", vff->size.metrics.left_offset >> 6, vff->size.metrics.left_offset & 0x3f);
	scrnlog("   right of    %ld.%d\n", vff->size.metrics.right_offset >> 6, vff->size.metrics.right_offset & 0x3f);
	scrnlog("   thicken     %ld.%d\n", vff->size.metrics.thicken >> 6, vff->size.metrics.thicken & 0x3f);
	scrnlog("   max advance %ld.%d\n", vff->size.metrics.max_advance >> 6, vff->size.metrics.max_advance & 0x3f);

	for (i = 0; i < vff->num_fixed_sizes; i++)
	{
		VF_BITMAP_SIZE *bms = vff->available_sizes;

		scrnlog("--> bitmap size %d\n", i);
		scrnlog("    heigh   %d\n", bms[i].height);
		scrnlog("    width   %d\n", bms[i].width);
		scrnlog("    size    %ld.%d\n", bms[i].size >> 6, bms[i].size & 0x3f);
		scrnlog("    x_ppem  %ld.%d\n", bms[i].x_ppem >> 6, bms[i].x_ppem & 0x3f);
		scrnlog("    y_ppem  %ld.%d\n", bms[i].y_ppem >> 6, bms[i].y_ppem & 0x3f);
	}

	scrnlog("-> bounding box\n");
	scrnlog("   xmin  %ld.%d\n", vff->bbox.xmin >> 6, vff->bbox.xmin & 0x3f);
	scrnlog("   ymin  %ld.%d\n", vff->bbox.ymin >> 6, vff->bbox.ymin & 0x3f);
	scrnlog("   xmax  %ld.%d\n", vff->bbox.xmax >> 6, vff->bbox.xmax & 0x3f);
	scrnlog("   ymax  %ld.%d\n", vff->bbox.ymax >> 6, vff->bbox.ymax & 0x3f);

	scrnlog("units per EM  %d\n", vff->units_per_em);
	scrnlog("ascender      %d\n", vff->ascender);
	scrnlog("descender     %d\n", vff->descender);
	scrnlog("height        %d\n", vff->height);
	scrnlog("mx adv width  %d\n", vff->max_advance_width);
	scrnlog("mx adv height %d\n", vff->max_advance_height);
	scrnlog("uline pos     %d\n", vff->underline_position);
	scrnlog("uline thick   %d\n", vff->underline_thickness);
	
	
	
}
/****************************************************************************/
/* BEGIN initialization code */

void
init_gdfdrv (OVDI_LIB *lib, struct module_desc *ret, char *p, char *f)
{
	l = lib;

	ret->types = D_FNT;
	ret->fnt = &fapi;

#if 0
	setup_sysfont();

	print_faces(&system_fonts);
	print_faces(&loaded_fonts);
#endif
}
/* END initialization code */
/****************************************************************************/

/****************************************************************************/
/* BEGIN local functions */
static void
setup_sysfont(struct libdesc *ldesc)
{
	FNTENT *fe;
	FONT_HEAD *f;

	fe = &ldesc->sysfnt_08;
	f = (FONT_HEAD *)systemfont08;
	fixup_font(f);
	f->id	= 1;
	fe->nxt_face = fe->nxt_font = NULL;
	fe->flags = 0;
	fe->fnt_head = f;
	fe->fnt_name = sysfont_fname;
	add_loaded(&ldesc->system_fonts, fe);	

	fe = &ldesc->sysfnt_09;
	f = (FONT_HEAD *)&systemfont09;
	fixup_font(f);
	f->id = 1;
	fe->nxt_face = fe->nxt_font = 0;
	fe->flags = 0;
	fe->fnt_head = f;
	fe->fnt_name = sysfont_fname;
	add_loaded(&ldesc->system_fonts, fe);

	fe = &ldesc->sysfnt_10;
	f = (FONT_HEAD *)&systemfont10;
	fixup_font(f);
	f->id = 1;
	fe->nxt_face = fe->nxt_font = 0;
	fe->flags = 0;
	fe->fnt_head = f;
	fe->fnt_name = sysfont_fname;
	add_loaded(&ldesc->system_fonts, fe);
}

static void
get_family_name(VF_FACE *vff, FONT_HEAD *fh)
{
	int i;
	char *s, *d;

	s = (char *)&fh->name;
	d = (char *)&vff->family_name;

	for (i = 0;i < 32 && (*d++ = *s++); i++)
		;

	*d = 0;
}
static void
get_style_name(VF_FACE *vff, FONT_HEAD *fh)
{
	vff->style_name[0] = 0;
}

/*
 * Fetch metrics from fontheader and place into a VF_SIZE_METRICS structure
*/
static void
get_size_metrics( FONT_HEAD *f, VF_SIZE_METRICS *m )
{
	/*
	 * Size of the EM, expressed in integer pixels
	 */
	m->x_ppem	= 1;
	m->y_ppem	= 1;
	/*
	 * 16.16 fixed fload scales used to scale directly from
	 * design space to 1/64th of a device space pixel
	 */
	m->x_scale	= 1 << 16;
	m->y_scale	= 1 << 16;
	
	m->top		= (long)f->top << 6;
	m->ascent	= (long)f->ascent << 6;
	m->descent	= (long)f->descent << 6;
	m->height	= (long)f->top << 6;

	m->left_offset	= (long)f->left_offset << 6;
	m->right_offset	= (long)f->right_offset << 6;
	m->thicken	= (long)f->thicken << 6;

	m->max_advance	= (long)f->max_char_width << 6;
}

/*
 * Add a new font to our list of faces-oriented fontlist.
 * Faces are identified using the 'id' field in the font header
 * and togheter with 'point' identifies a unique font.
 * When the new font has a 'id' and 'point' equal to a font
 * already in this fontlist, add_loaded return -1, which means
 * font already exists.
 * A NULL is returned when the font belonged to one of the faces
 * already registered, or a 1 when the new font was a new face.
*/
static int
add_loaded(FNTENT **start, FNTENT *new)
{
	FNTENT *f, *closest, *prev, *this_face, *prev_face;
	unsigned int diff, got;

	this_face = *start;

	if (!this_face)
	{
		*start = new;
		new->nxt_face = NULL;
		new->nxt_font = NULL;
		return 0;
	}

	prev_face = closest = NULL;
	got = 0;
	diff = 0xffff;

	/*
	 * Check if the new font's ID exists in our faces list.
	 * This loop continues until nxt_face is NULL or 'closest' gets
	 * set because a placement for the new font was found.
	*/
	while (this_face)
	{
		if (this_face->fnt_head->id == new->fnt_head->id)
		{
			/*
			 * Found face, now look for the best placement for the new font.
			 * 'closest' will contain the best place to insert the new font
			 * when the loop terminates.
			 * The loop terminates when the nxt_font field tells us there
			 * are no more fonts in this face.
			*/
			f = this_face;
			prev = NULL;
			while (f)
			{
				if (f->fnt_head->point == new->fnt_head->point)
				{
					return -1;		/* This font is already loaded */
				}
				if (got)
				{
					unsigned int d;

					if (f->fnt_head->point > new->fnt_head->point)
					{
						d = f->fnt_head->point - new->fnt_head->point;
						if (d < diff)
						{
							closest = prev;
							diff = d;
							got = 1;
						}
					}
					else
					{
						d = new->fnt_head->point - f->fnt_head->point;
						if (d < diff)
						{
							closest = f;
							diff = d;
							got = 1;
						}
					}
				}
				else
				{
					if (f->fnt_head->point > new->fnt_head->point)
					{
						closest = prev;
						diff = f->fnt_head->point - new->fnt_head->point;
						got = 1;
					}
					else
					{
						closest = f;
						diff = new->fnt_head->point - f->fnt_head->point;
						got = 1;
					}
				}
				prev = f;
				f = f->nxt_font;
			}
		}

		if (got)
			break;

		prev_face = this_face;
		this_face = this_face->nxt_face;
	}
	
	if (got)
	{
		if (!closest)
		{
			new->nxt_face	= this_face->nxt_face;
			new->vff	= this_face->vff;
			new->nxt_font	= this_face;
			this_face->nxt_face = NULL;
			this_face->vff = NULL;
			if (!prev_face)
				*start = new;
			else
				prev_face->nxt_face = new;
		}
		else
		{
			new->nxt_font = closest->nxt_font;
			closest->nxt_font = new;
		}
		return 0;
	}
	else
	{
		new->nxt_face = *start;
		*start = new;
		return 1;
	}	
}

static long
count_face_sizes(FNTENT *face)
{
	long cnt = 0;

	do { cnt++; } while ( (face = face->nxt_font) );

	return cnt;
}
#if 0
static void
attach_face2vff(FNTENT *face, VF_FACE *vff)
{
	FONT_HEAD *f;

	f = face->fnt_head;

	//vff->internal	= face;

	vff->num_faces		= 1;
	vff->face_index		= 1;
	vff->face_flags		= 0;
	vff->style_flags	= 0;
	vff->num_glyphs		= f->last_ade - f->first_ade;

	get_family_name(vff, f);
	get_style_name(vff, f);

	vff->num_fixed_sizes	= count_face_sizes(face);

	vff->num_charmaps	= 1;

	get_size_metrics (f, &vff->size );
}
#endif

/*
 * Find face by index
 */
static FNTENT *
ffbidx(FNTENT *s, long i, long *ret)
{
	while (i > 1 && s)
	{
		s = s->nxt_face;
		i--;
	}
	
	if (ret)
		*ret = i;
	return s;
}		
	
static FNTENT *
find_facebyidx(struct libdesc *ldesc, long idx, long *ridx)
{
	FNTENT *found;

	if (!(found = ffbidx(ldesc->system_fonts, idx, &idx)))
		found = ffbidx(ldesc->loaded_fonts, idx, &idx);
	
	if (ridx)
		*ridx = idx;
	return found;
}

/*
 * Find face by ID
 */
static FNTENT *
ffbid(FNTENT *s, long id)
{
	while (s)
	{
		if (s->fnt_head->id == id)
			break;
		s = s->nxt_face;
	}
	return s;
}

static FNTENT *
find_facebyid(struct libdesc *ldesc, long id)
{
	FNTENT *found;

	if (!(found = ffbid(ldesc->system_fonts, id)))
		found = ffbid(ldesc->loaded_fonts, id);
	return found;
}


static void
get_bitmap_sizes(VF_FACE *vff)
{
	long sizes;
	FNTENT *face = (FNTENT *)vff->face_handle;
	VF_BITMAP_SIZE *pt_sizes;

	sizes = count_face_sizes(face);
	
	if (vff->available_sizes)
	{
		vff->num_fixed_sizes = 0;
		(*l->mfree)(vff->available_sizes);
		vff->available_sizes = NULL;
	}
	
	pt_sizes = (*l->malloc)((sizeof(*pt_sizes)) * sizes, 0);
	vff->available_sizes = pt_sizes;
	
	if (pt_sizes)
	{
		vff->num_fixed_sizes = sizes;
		while (face)
		{
			FONT_HEAD *fh = face->fnt_head;
			
			pt_sizes->height = fh->top;
			pt_sizes->width	 = fh->max_char_width;
			pt_sizes->size	 = (long)fh->point << 6;
			pt_sizes->x_ppem = 1 << 6; //(long)fh->max_char_width << 6;
			pt_sizes->y_ppem = 1 << 6; //(long)fh->top << 6;
			
			pt_sizes++;
			face = face->nxt_font;
		}
	}
	else
		vff->num_fixed_sizes = 0;
}

static long
new_vdi_face(struct libdesc *ldesc, FNTENT *face, VF_FACE **ret)
{

	VF_FACE *new = NULL;

	if (face)
	{
		if (!(new = face->vff))
		{
			new = (*l->malloc)(sizeof(*new), 0);
			if (new)
			{
				FONT_HEAD *fh;

				(*l->bzero)(new, sizeof(*new));

				fh = face->fnt_head;
				face->vff = new;
				
				new->lib_handle			= (long)ldesc;
				new->face_handle		= (long)face;
				new->api			= &fapi;
				new->id				= fh->id;
				new->num_faces			= 1;
				new->face_index			= 1;
				new->face_flags			= 0;
				new->style_flags		= 0;
				new->num_glyphs			= fh->last_ade - fh->first_ade;
				get_family_name(new, fh);
				get_style_name(new, fh);
				new->num_charmaps		= 1;
				
				new->size.parent		= new;
				new->size.generic.data		= NULL;
				new->size.generic.finalizer	= NULL;
				new->size.internal		= NULL;
				get_size_metrics(fh, &new->size.metrics);

				get_bitmap_sizes(new);
				new->internal	= face;
				
				new->units_per_em = 1; /* Set to unrealistic 1 for fixed sized fonts */
				new->ascender = fh->top;
				new->descender = -fh->bottom;
				new->height = fh->top + fh->bottom;
				new->max_advance_width = fh->max_char_width;
				new->max_advance_height = fh->top + fh->bottom;
				new->underline_position = -1;
				new->underline_thickness = 1;
			}
		}
	}
	*ret = new;
	return 0;
}

static FNTENT *
load_a_font(struct libdesc *ldesc, char *fullname, char *buff, long fs, long nlen)
{
	FNTENT *fntent = NULL;
	char *mem = NULL;

	if (fs > 0)
		mem = buff;

	if (mem && (*l->load_file)(fullname, fs, mem + sizeof(FNTENT) + nlen) == fs)
	{
		FONT_HEAD *fnt;
		char *name;
		int i;

		fntent		= (FNTENT *)mem;
		name		= mem + sizeof(FNTENT);
		fnt		= (FONT_HEAD *)(name + nlen);

		(*l->bzero)(fntent, sizeof(*fntent));

		fntent->fnt_name	= name;
		fntent->fnt_head	= fnt;
		
		for (i = 0; (*name++ = fullname[i]); i++)
			;

		fixup_font(fnt);

		/*
		 * add_loaded returns -1 if this font have already been loaded.
		 * 0 if this font belonged to an already registered face,
		 * or 1 if this font was a new face.
		*/
		i = add_loaded(&ldesc->loaded_fonts, fntent);

		if (i == -1)
		{
			/*
			 * If this font already exist, just skip it
			*/
			fntent = NULL;
		}
		else
			ldesc->num_loaded++; /* Counts number of loaded fontfiles */

		if (i == 1)
			ldesc->num_faces++; /* Counts number of faces */
	}
	//else
	//	(*l->scrnlog)("Could not load %s\n", fullname);

	return fntent;
}

/* END local functions */
/****************************************************************************/

/****************************************************************************/
/* Begin API functions */

/*
 * Open a new library.
 */
static long
open(void)
{
	struct libdesc **ld_list = &ldlist, *ld;

	if (!(ld = *ld_list))
	{
		ld = &root_ld;
	}
	else
	{
		while ((*ld_list)->next)
			*ld_list = (*ld_list)->next;
		
		ld = (*l->malloc)(sizeof(*ld), 0);
		*ld_list = ld;
	}
	(*l->bzero)(ld, sizeof(*ld));

	ld->handle	= (long)ld;
	ld->l		= l;
	ld->api		= &fapi;

	setup_sysfont(ld);

	print_faces(&ld->system_fonts);
	print_faces(&ld->loaded_fonts);
	
	return (long)ld;
}

#if 0
static long
new_face(VF_FACE *ret)
{
	VF_FACE *vff;
	VFF_ATTACH *attach;

	vff = (VF_FACE *)omalloc(sizeof(VF_FACE) + sizeof(VFF_ATTACH), MX_PREFTTRAM | MX_PRIVATE);
	attach = (VFF_ATTACH *)((long)vff + sizeof(VF_FACE));

	vff->face_handle = attach;
#endif
	
	
static long
open_face(long handle, char *fname, long ind, VF_FACE **vff)
{
	struct libdesc *ldesc = (struct libdesc *)handle;
	long fs, nlen, ret = 0;
	char *mem;
	FNTENT *face;
	VF_FACE *new_vff = NULL;

	nlen = ((*l->strlen)(fname) + 2) & -2;
	
	fs = get_file_size( fname );

	if (fs < 0)
		return -1;

	mem = (*l->malloc)( fs + sizeof(FNTENT) + nlen, MX_PREFTTRAM | MX_SUPER );

	if (!mem)
		return -1;

	if ((face = load_a_font(ldesc, fname, mem, fs, nlen)))
	{
		face->flags |= FEF_FE_MALLOC;
		new_vdi_face(ldesc, face, &new_vff);
	}
	else
		ret = -1;
	
	*vff = new_vff;

	return ret;
}

static long
get_facebyidx(long handle, long idx, VF_FACE **vff)
{
	struct libdesc *ldesc = (struct libdesc *)handle;
	FNTENT *face;
	VF_FACE *new = NULL;
	long ret = -1;
	long index;

	face = find_facebyidx(ldesc, idx, &index);
	
	if (face)
		ret = new_vdi_face(ldesc, face, &new);
	
	*vff = new;

	print_vff(*vff);
	
	return ret;
}

static long
get_facebyid(long handle, long id, VF_FACE **vff)
{
	long ret = -1;
	struct libdesc *ldesc = (struct libdesc *)handle;
	VF_FACE *new = NULL;
	FNTENT *face;

	face = find_facebyid(ldesc, id);
	
	if (face)
		ret = new_vdi_face(ldesc, face, &new);
	
	*vff = new;
	
	print_vff(*vff);
	
	return ret;
}

/*
 * Takes a pointer to a pathname and a list of filenames of font-files to load.
*/
static long
load_fonts ( long handle, char *path, char *names )
{
	struct libdesc *ldesc = (struct libdesc *)handle;
	long fs;
	char *fname, *fullname, *n, *nlist;
	char pfbuff[128+64];

	//scrnlog("\nLoad fonts: path %s, names=%lx\n", path, names);

	if (!*names)
		return 0;

	fname = fullname = (char *)pfbuff;

	/*
	 * Copy path into local buffer, after which fname will
	 * point to where filename starts
	*/
	while (*path){*fname++ = *path++;}

	/*
	 * Check if pathname ends with a '\' or a '/'
	*/
	fname--;
	if (*fname == 0x5c || *fname == '/')
	{
		fname++;
	}
	else
	{
		fname++;
		*fname++ = 0x5c;
	}

	nlist = names;
	while (*nlist)
	{
		FNTENT *fent;
		char *mem;
		long nlen;
		
		n = fname;
		while ((*n++ = *nlist++))
			;

		fs = get_file_size(fullname);
		if (fs > 0)
		{
			nlen = ((*l->strlen)(fullname) + 2) & -2;
			mem = (*l->malloc)(fs + nlen + sizeof(*fent), 0);
			if ((fent = load_a_font(ldesc, fullname, mem, fs, nlen)))
				fent->flags |= FEF_FE_MALLOC;
		}
	}
	print_faces(&ldesc->loaded_fonts);
	return 0;
}

static void
unload_fonts(long handle)
{
	struct libdesc *ldesc = (struct libdesc *)handle;
	FNTENT *fent, *fe;

	fent = ldesc->loaded_fonts;
	
	while (fent)
	{
		fe = fent->nxt_font;
		fent->nxt_font = NULL;

		while (fe)
		{
			FNTENT *f = fe->nxt_font;
			if (fe->flags & FEF_FH_MALLOC)
			{
				(*l->mfree)(fe->fnt_head);
			}
			if (fe->flags & FEF_FE_MALLOC)
			{
				(*l->mfree)(fe);
			}
			fe = f;
		}
		
		fe = fent;
		fent = fent->nxt_face;

		if (fe->flags & FEF_FH_MALLOC)
		{
			(*l->mfree)(fe->fnt_head);
		}
		if (fe->flags & FEF_FE_MALLOC)
		{
			(*l->mfree)(fe);
		}
	}
}
static void
set_char_pointsize(VF_FACE *vff, long point)
{
	
}
static void
set_charsize(VF_FACE *vff, long width, long height)
{
}
static void
set_devsize(VF_FACE *vff, long horirez, long vertrez)
{
}
static void
set_pixelsize(VF_FACE *vff, long width, long height)
{
}

/* END API functions */
/****************************************************************************/
