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
struct fntent
{
	struct fntent		*nxt_face;
	struct fntent		*nxt_font;
	long			flags;
	char			*fn;
	FONT_HEAD		*f;
};
typedef struct fntent FNTENT;

struct vff_attach
{
	struct vf_face		*vff;
	struct fntent		*face;
	struct fntent		*font;
};
typedef struct vff_attach VFF_ATTACH;

/* End definition part */
/****************************************************************************/
/* BEGIN API function definitions */

static long	load_fonts(char *path, char *names);
static void	unload_fonts(void);

static long	open (void);
static long	open_face(long hl, char *fn, long ind, VF_FACE *vff);
static long	get_facebyidx(long handle, long idx, VF_FACE *vff);
static long	get_facebyid(long handle, long id, VF_FACE *vff);

static void	set_char_pointsize(VF_FACE *vff, long point);
static void	set_charsize(VF_FACE *vff, long width, long height);
static void	set_devsize(VF_FACE *vff, long horirez, long vertrez);
static void	set_pixelsize(VF_FACE *vff, long width, long height);

/* END API function definitions */
/****************************************************************************/

/****************************************************************************/
/* BEGIN local function definitions */

static void	setup_sysfont(void);
static void	get_family_name(VF_FACE *vff, FONT_HEAD *fh);
static void	get_style_name(VF_FACE *vff, FONT_HEAD *fh);
static void	get_size_metrics(FONT_HEAD *f, VF_SIZE_METRICS *m );
static int	add_loaded(FNTENT **start, FNTENT *new);

/* END local function definitions */
/****************************************************************************/
/* BEGIN global data definition and access part */

static OVDI_LIB *l;

static long libhandle = 0;

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

	load_fonts,
	unload_fonts,

	open,
	open_face,
	get_facebyidx,
	get_facebyid,

	set_char_pointsize,
	set_charsize,
	set_devsize,
	set_pixelsize,
};

static FNTENT *system_fonts = 0;
static FNTENT *loaded_fonts = 0;

static FNTENT sysfnt_08;
static FNTENT sysfnt_09;
static FNTENT sysfnt_10;

static char sysfont_fname[] = "Built in\0";

/* END global data definition and access part */
/****************************************************************************/
static void
print_faces(FNTENT **start)
{
	FNTENT *face = *start;

	if (face)
	{
		do
		{
			FNTENT *font = face;
			scrnlog("Face %s\n", face->f->name);
			do
			{
				scrnlog("  Font %s\n", font->f->name);
			} while ( (font = font->nxt_font) );
		} while ( (face = face->nxt_face) );
	}
	else
		scrnlog("No faces here!\n");
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
setup_sysfont(void)
{
	FNTENT *fe;
	FONT_HEAD *f;

	fe = &sysfnt_08;
	f = (FONT_HEAD *)&systemfont08;
	fixup_font(f);
	f->id	= 1;
	fe->nxt_face = fe->nxt_font = 0;
	fe->flags = 0;
	fe->f = f;
	fe->fn = sysfont_fname;
	add_loaded(&system_fonts, fe);	

	fe = &sysfnt_09;
	f = (FONT_HEAD *)&systemfont09;
	fixup_font(f);
	f->id = 1;
	fe->nxt_face = fe->nxt_font = 0;
	fe->flags = 0;
	fe->f = f;
	fe->fn = sysfont_fname;
	add_loaded(&system_fonts, fe);

	fe = &sysfnt_10;
	f = (FONT_HEAD *)&systemfont10;
	fixup_font(f);
	f->id = 1;
	fe->nxt_face = fe->nxt_font = 0;
	fe->flags = 0;
	fe->f = f;
	fe->fn = sysfont_fname;
	add_loaded(&system_fonts, fe);
}

static void
get_family_name(VF_FACE *vff, FONT_HEAD *fh)
{
	int i;
	char *s, *d;

	s = (char *)&fh->name;
	d = (char *)&vff->family_name;

	for (i = 0;i < 32 && (*d++ = *s++); i++){}

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
	m->x_ppem	= f->point;
	m->y_ppem	= f->point;
	m->x_scale	= 0;
	m->y_scale	= 0;
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
	unsigned int diff;

	this_face = *start; //loaded_fonts;

	if (!this_face)
	{
		*start = new; //loaded_fonts = new;
		new->nxt_face = 0;
		new->nxt_font = 0;
		return 0;
	}

	prev_face = this_face;
	closest = 0;
	diff = 0xffff;

	/*
	 * Check if the new font's ID exists in our faces list.
	 * This loop continues until nxt_face is NULL or 'closest' gets
	 * set because a placement for the new font was found.
	*/
	do
	{
		if (this_face->f->id == new->f->id)
		{
			/*
			 * Found face, now look for the best placement for the new font.
			 * 'closest' will contain the best place to insert the new font
			 * when the loop terminates.
			 * The loop terminates when the nxt_font field tells us there
			 * are no more fonts in this face.
			*/
			f = this_face;
			prev = this_face;
			do
			{
				if (f->f->point == new->f->point)
				{
					return -1;
				}
				if (closest)
				{
					unsigned int d;

					if (f->f->point > new->f->point)
					{
						d = f->f->point - new->f->point;
						if (d < diff)
						{
							closest = prev;
							diff = d;
						}
					}
					else
					{
						d = new->f->point - f->f->point;
						if (d < diff)
						{
							closest = f;
							diff = d;
						}
					}
				}
				else
				{
					if (f->f->point > new->f->point)
					{
						closest = prev;
						diff = f->f->point - new->f->point;
					}
					else
					{
						closest = f;
						diff = new->f->point - f->f->point;
					}
				}
				prev = f;
			} while ( (f = f->nxt_font) );
		}
		prev_face = this_face;
	} while ( (this_face = this_face->nxt_face) && !closest );
	
	if (closest)
	{
		new->nxt_font = closest->nxt_font;
		closest->nxt_font = new;
		return 0;
	}
	else
	{
		/*
		 * This is a new face, just add it
		*/
		new->nxt_face = loaded_fonts;
		loaded_fonts = new;
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

static void
attach_face2vff(FNTENT *face, VF_FACE *vff)
{
	FONT_HEAD *f;

	f = face->f;

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

static FNTENT *
find_facebyidx(long idx)
{
	FNTENT *fe = system_fonts, *found;

	if (fe)
	{
		while ( --idx && (found = fe->nxt_face) ) { fe = found; }
	}
	if (idx)
	{
		if ( (fe = loaded_fonts) )
		{
			while ( --idx && (found = fe->nxt_face) ) { fe = found; }
		}
	}

	if (!idx)
		return fe;
	else
		return 0;
}

static FNTENT *
find_facebyid(long id)
{
	FNTENT *fe = system_fonts;

	if (fe)
	{
		do
		{
			if (fe->f->id == id)
				return fe;
		} while ( (fe = fe->nxt_face) );
	}
	if ( (fe = loaded_fonts) )
	{
		do
		{
			if (fe->f->id == id)
				return fe;
		} while ( (fe = fe->nxt_face) );
	}
	return 0;
}

/* END local functions */
/****************************************************************************/

/****************************************************************************/
/* Begin API functions */

static long
open(void)
{
	libhandle++;
	return libhandle;
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
open_face(long hl, char *fname, long ind, VF_FACE *vff)
{
	long fs, mem;
	FONT_HEAD *fhead;

	fs = get_file_size( fname );

	if (fs < 0)
		return -1;

	mem = (long)omalloc( fs, MX_PREFTTRAM | MX_SUPER );

	if (!mem)
		return -1;

	if ( load_file(fname, fs, (char *)mem) != fs)
		return -1;

	fhead = (FONT_HEAD *)mem;
	vff->face_handle = mem;

	fixup_font(fhead);

	vff->num_faces		= 1;
	vff->face_index		= 1;
	vff->face_flags		= 0;
	vff->style_flags	= 0;
	vff->num_glyphs		= fhead->last_ade - fhead->first_ade;

	get_family_name(vff, fhead);
	get_style_name(vff, fhead);

	vff->num_fixed_sizes	= 1;

	vff->num_charmaps	= 1;

	get_size_metrics(fhead, &vff->size );

	return 0;
}

static long
get_facebyidx(long handle, long idx, VF_FACE *vff)
{
	FNTENT *face;
	long ret = -1;

	face = find_facebyidx(idx);

	if (face)
	{

	}
	return ret;
}

static long
get_facebyid(long handle, long id, VF_FACE *vff)
{
	return 0;
}

/*
 * Takes a pointer to a pathname and a list of filenames of font-files to load.
*/
static long
load_fonts ( char *path, char *names )
{
	long fs;
	int num_loaded = 0, faces = 0;
	char *fname, *fullname, *n, *nlist;
	long size = 0;
	char pfbuff[128+64];

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

	/*
	 * Gather the size of each file, so we know the amount
	 * of ram needed to load these files.
	*/
	nlist = names;
	while (*nlist)
	{
		n = fname;
		while ( (*n++ = *nlist++) ){}
		fs = get_file_size(fullname);
		if (fs > 0)
		{
			size += fs;
			size += sizeof(FNTENT);
			size += (((n - fullname) + 1) & -2);
		}
		//else
		//	scrnlog("could not locate %s\n", fullname);
	}

	/*
	 * Now we load all the files, and store them in a face-oriented fashion.
	 * Faces are identified by the 'id' field in the font header.
	*/
	if (size)
	{
		char *mem, *tmp;
		int i;
		FONT_HEAD *fnt;
		FNTENT *fntent;

		/*
		 * Get RAM for the files..
		*/
		mem = (char *)omalloc(size, MX_PREFTTRAM | MX_READABLE);
		if (!mem)
		{
			return 0;
		}

		/*
		 * Load each file, try to add them to our ring, based on faces
		*/
		nlist = names;
		while (*nlist)
		{
			n = fname;
			while ( (*n++ = *nlist++) ){}
			fs = get_file_size(fullname);
			if (fs > 0)
			{
				if ( load_file(fullname, fs, mem + sizeof(FNTENT)) == fs)
				{
					tmp = mem;
					fntent		 = (FNTENT *)mem;
					mem		+= sizeof(FNTENT);
					fnt		 = (FONT_HEAD *)mem;
					mem		+= fs;

					fntent->nxt_face	= 0;
					fntent->nxt_font	= 0;
					fntent->flags		= FEF_WEOWN;
					fntent->fn		= mem;
					fntent->f		= fnt;
					for (i = 0; (*mem++ = fullname[i]); i++){}

					/*
					 * word-align
					*/
					if ((long)mem & 1)
						mem++;

					fixup_font(fnt);

					/*
					 * add_loaded returns -1 if this font have already been loaded.
					 * 0 if this font belonged to an already registered face,
					 * or 1 if this font was a new face.
					*/
					i = add_loaded(&loaded_fonts, fntent);

					if (i == -1)
					{
						/*
						 * If this font already exist, just skip it
						*/
						mem = tmp;	/* Back to start of next font to load */
					}
					else
						num_loaded++;	/* Counts number of loaded fontfiles */

					if (i == 1)
						faces++;	/* Counts number of faces */
					
				}
				//else
				//	scrnlog("Could not load %s\n", fullname);
			}
		}

		/*
		 * If no fonts was loaded, release resources and return.
		*/
		if (!num_loaded)
		{
			free_mem(loaded_fonts);
			loaded_fonts = 0;
			return 0;
		}
	}
	print_faces(&loaded_fonts);
	return 0;
}

static void
unload_fonts(void)
{
	if (loaded_fonts)
	{
		free_mem(loaded_fonts);
		loaded_fonts = 0;
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
