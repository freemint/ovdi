#include <osbind.h>

#include "libkern.h"
#include "display.h"
#include "file.h"
#include "vdi_defs.h"
#include "fonts.h"
#include "gdf_defs.h"
#include "memory.h"
#include "vdi_font.h"
#include "vdi_globals.h"

long
load_font( char *fn, long *size, long *loc, struct gdf_membuff *m)
{
	long fs, lbytes, used;
	int i;
	char *b = m->free;
	XGDF_HEAD *xf;

	used = (long)m->free - (long)m->base;
	fs = get_file_size(fn);

	if ( (used + fs + sizeof(XGDF_HEAD)) > m->size)
		return -1;

	if (fs < 0)
		return fs;

	xf = (XGDF_HEAD *)b;
	bzero(xf, sizeof(XGDF_HEAD));
	b += sizeof(XGDF_HEAD);

	lbytes = load_file( fn, fs, b);
	if (lbytes < 0)
		return lbytes;
	else if (lbytes != fs)
		return -1;

	xf->font_head = (FONT_HEAD *)b;

	m->free  += (lbytes + sizeof(XGDF_HEAD) + 3) & ~3;
	
	//scrnlog("Font '%s' loaded into %lx. fonthead %lx, Size %ld, nxtfree %lx\n", fn, xf, xf->font_head, lbytes + sizeof(XGDF_HEAD), (long)xf + lbytes + sizeof(XGDF_HEAD) );
	//scrnlog("mbase %lx, mfree %lx, msize %ld\n\n", m->base, m->free, m->size);
	
	for (i = 200, b = (char *)&xf->filename[0]; i > 0 && (*b++ = *fn++); i--){}
	*b = 0;

	if (size)
		*size = lbytes;
	if (loc)
		*loc = (long)xf;

	return 0;
}

void
fixup_font( FONT_HEAD *font )
{
	if (!(font->flags & F_STDFORM))
	{
		int i;
		short flags;
		unsigned short tmp;
		unsigned short *tmp2;
		
		font->id	= ((font->id << 8) | (font->id >> 8));
		font->point	= ((font->point << 8) | (font->point >> 8));

		tmp2 = (unsigned short *)&font->first_ade;

		for (i=0;i<24; i++)
		{
			tmp = *tmp2;
			*tmp2++ = ((tmp << 8) | (tmp >> 8));
		}

		(long)font->hor_table = ( ((long)font->hor_table << 16) | ((long)font->hor_table>>16) );
		(long)font->off_table = ( ((long)font->off_table << 16) | ((long)font->off_table>>16) );
		(long)font->dat_table = ( ((long)font->dat_table << 16) | ((long)font->dat_table>>16) );
	
		flags = font->flags;

		{
			unsigned short tmp3;

			tmp2 = (short *)((long)font + (long)font->off_table);
			for (i = 0; i < (font->last_ade - font->first_ade) + 2; i ++)
			{
				tmp3	= *tmp2;
				*tmp2++ = ( (*tmp2)<<8 | (*tmp2)>>8 );
				if (*tmp2 != tmp)
					flags &= ~F_MONOSPACE;
			}
		}
			

		if (flags & F_HORZ_OFF)
		{
			tmp2 = (short *)((long)font + (long)font->hor_table);
			for (i = 0 ; i < (font->last_ade - font->first_ade) ; i++ )
 				*tmp2++ = ( (*tmp2)<<8 | (*tmp2)>>8 );
		}
		flags |= F_STDFORM;

		font->flags = flags;
	}

	if (font->flags & F_HORZ_OFF)
		(long)font->hor_table += (long)font;
	else
		font->hor_table = 0;

	(long)font->dat_table += (long)font;
	(long)font->off_table += (long)font;

	if (font->top + font->bottom > font->form_height - 1)
		font->bottom = font->form_height - font->top - 1;

	font->next = 0;
	font->font_seg = 0;
}

/* Set up the system font */
/* User selected system-fonts or NULL */
static XGDF_HEAD *
load_sysfont(char *path, char *file, struct gdf_membuff *m)
{
	XGDF_HEAD *xf = 0;
	char *fname;
	char fqpn[128];

	fname	= (char *)&fqpn;
	while (*path) *fname++ = *path++;
	while (*file) *fname++ = *file++;
	*fname = 0;

	if ( !load_font((char *)fqpn, 0, (long *)&xf, m) )
	{
		fixup_font(xf->font_head);
	}
	return xf;
}
/* This is called !ONCE! upon boot to initialize the default system fonts!
 * Because of this, we dont keep the gdf_membuff for the systemfonts,
 * as this memory will never be released
*/
void
init_systemfonts(SIZ_TAB *st, DEV_TAB *dt)
{

	if (!sysfnt08p)
	{
		int i;
		long fs, size = 0;
		char *buff;
		FONT_HEAD *f1;
		XGDF_HEAD *xf;
		struct gdf_membuff m;
		char savpath[128];

		Dgetpath( (char *)&savpath, 0);
		Dsetpath( (char *)gdf_path );

		sysfnt_minwchar = sysfnt_minhchar = 0x7fff;
		sysfnt_maxwchar = sysfnt_maxhchar = 0;

		sysfnt_faces	= 1;

	/* Figure out size of buffer needed to hold loaded system-fonts */
		if (sysf08_name[0])
		{
			fs = get_file_size((char *)sysf08_name);
			if (fs > 0)
				size =  (fs + sizeof(XGDF_HEAD) + 3) & ~3;
			else
				sysf08_name[0] = 0;
		}
		if (sysf09_name[0])
		{
			fs = get_file_size((char *)sysf09_name);
			if (fs > 0)
				size += (fs + sizeof(XGDF_HEAD) + 3) & ~3;
			else
				sysf09_name[0] = 0;
		}
		if (sysf10_name[0])
		{
			fs = get_file_size((char *)sysf10_name);
			if (fs > 0)
				size += (fs + sizeof(XGDF_HEAD) + 3) & ~3;
			else
				sysf09_name[0] = 0;
		}
		if (size)
		{
			buff = (char *)omalloc(size, 0);
			if (!buff) /* If no buffer obtainable, use internal systemfonts */
				sysf08_name[0] = sysf09_name[0] = sysf10_name[0] = 0;
			else
			{
				m.base = buff;
				m.free = buff;
				m.size = size;
			}
		}

	/* system font 08 points */
		xf = 0;
		if (sysf08_name[0])
			xf = load_sysfont((char *)gdf_path, (char *)sysf08_name, &m);
		if (xf)
			f1 = xf->font_head;
		else
		{
			f1 = (FONT_HEAD *)&systemfont08;
			xf = &xsystemfont08;
			xf->font_head = f1;
			for (i = 0; i < 256; i++)
				xf->cache[i] = 0;
			fixup_font(f1);
		}
		xf->links = 1;
		sysfnt08p = xf;
		f1->id = 1;

	/* system font 09 points */
		xf = 0;
		if (sysf09_name[0])
			xf = load_sysfont((char *)gdf_path, (char *)sysf09_name, &m);
		if (xf)
			f1 = xf->font_head;
		else
		{
			f1 = (FONT_HEAD *)&systemfont09;
			xf = &xsystemfont09;
			xf->font_head = f1;
			for (i = 0; i < 256; i++)
				xf->cache[i] = 0;
			fixup_font(f1);
		}
		xf->links = 1;
		sysfnt09p = xf;
		f1->id = 1;
		if ((add_font(sysfnt08p, xf)) == 1)
			sysfnt_faces++;

	/* system font 10 points */
		xf = 0;
		if (sysf10_name[0])
			xf = load_sysfont((char *)gdf_path, (char *)sysf10_name, &m);
		if (xf)
			f1 = xf->font_head;
		else
		{
			f1 = (FONT_HEAD *)&systemfont10;
			xf = &xsystemfont10;
			xf->font_head = f1;
			for (i = 0; i < 256; i++)
				xf->cache[i] = 0;
			fixup_font(f1);
		}
		xf->links = 1;
		sysfnt10p = xf;
		f1->id = 1;
		if ((add_font(sysfnt08p, xf)) == 1)
			sysfnt_faces++;

	/* Fill in SIZ tab with max/min char with/height */
		xf = sysfnt08p;
		while(xf)
		{
			f1 = xf->font_head;

			if (f1->max_char_width < sysfnt_minwchar)
				sysfnt_minwchar = f1->max_char_width;
			if (f1->max_char_width > sysfnt_maxwchar)
				sysfnt_maxwchar = f1->max_char_width;

			if (f1->top < sysfnt_minhchar)
				sysfnt_minhchar = f1->top;
			if (f1->top > sysfnt_maxhchar)
				sysfnt_maxhchar = f1->top;

			xf = xf->next;
		}

		dt->faces	= sysfnt_faces;
		st->minwchar	= sysfnt_minwchar;
		st->maxwchar	= sysfnt_maxwchar;
		st->minhchar	= sysfnt_minhchar;
		st->maxhchar	= sysfnt_maxhchar;

		Dsetpath( (char *)&savpath );
	}
}

/* This cache stuff is very, very simple and preliminary.... */
long		gdf_cache_size = 0;
static char	*font_cache = 0;
static char	*next_free = 0;
GDF_CACHED *
gdf_get_cachemem(XGDF_HEAD *xf, short chr)
{
	int width;
	long fdatlen, cachentlen, usedcache;
	FONT_HEAD *f = xf->font_head;
	GDF_CACHED *cachent = 0;
	char *nextfree = next_free;

	if (!(nextfree = next_free))
	{
		if (!gdf_cache_size)
			return 0;
		font_cache = (char *)omalloc(gdf_cache_size + 4, MX_PREFTTRAM | MX_SUPER);
		if (!font_cache)
			return 0;
		next_free = nextfree = font_cache;
	}
	
	if (chr < f->first_ade || chr > f->last_ade)
		chr = 0x3f;

	chr	-= f->first_ade;
	width	 = f->off_table[chr +  1] - f->off_table[chr];
	
	usedcache	= (long)nextfree - (long)font_cache;
	fdatlen		= ((long)width * f->form_height) << 1;
	cachentlen	= fdatlen + sizeof(GDF_CACHED);

	if ( (cachentlen + usedcache) <= gdf_cache_size)
	{
		cachent			=  (GDF_CACHED *)nextfree;
		cachent->mfdb.fd_addr	=  (char *)cachent + sizeof(GDF_CACHED);
		cachent->font		=  xf;
		cachent->chridx		=  chr;
		next_free		+= cachentlen;
		xf->cache[chr + f->first_ade] = cachent;
	}
	//else
		//scrnlog("fontcache exhausted!\n");

	return cachent;
}

void
gdf_free_cache(void)
{
	int i;

	if (font_cache)
	{
		/* For now we need to clear the cache-table for the systemfonts */
		for (i = 0; i < 256; i++)
		{
			sysfnt08p->cache[i] = 0;
			sysfnt09p->cache[i] = 0;
			sysfnt10p->cache[i] = 0;
		}
		free_mem(font_cache);
	}
	font_cache = next_free = 0;
}
/*
 * Add a new font to the font-ring starting at 'start'. 
 * Returns 0 if added font was not a new face, only new size.
 * Returns 1 if added font was a new face (first font with this fonts ID)
 * Returns -1 if font with same ID and size already existed in chain.
*/
short
add_font( XGDF_HEAD *start, XGDF_HEAD *new) //FONT_HEAD *start, FONT_HEAD *new)
{
	XGDF_HEAD *f, *closest, *prev;
	unsigned short diff;

	f = start;
	closest = 0;
	prev = start;
	diff = 0xffff;

	do
	{
		if (f->font_head->id == new->font_head->id)
		{
			if (f->font_head->point == new->font_head->point)
				return -1;

			if (closest)
			{
				unsigned short d;

				if (f->font_head->point > new->font_head->point)
				{
					d = f->font_head->point - new->font_head->point;
					if (d < diff)
					{
						closest = prev;
						diff = d;
					}
				}
				else
				{
					d = new->font_head->point - f->font_head->point;
					if (d < diff)
					{
						closest = f;
						diff = d;
					}
				}
			}
			else
			{
				if (f->font_head->point > new->font_head->point)
				{
					closest = prev;
					diff = f->font_head->point - new->font_head->point;
				}
				else
				{
					closest = f;
					diff = new->font_head->point - f->font_head->point;
				}
			}
		}
		prev = f;
	} 
	while ( (f = f->next) );

	if (closest)
	{
		new->next = closest->next;
		closest->next = new;
		return 0;
	}
	else
	{
		new->next = prev->next;
		prev->next = new;
		return 1;
	}
}
/*
 * Find font by index. Returns 0 if successful, or the index at which
 * search stopped (end of fonts in ring). 'ret' is filled with pointer
 * to the XGDF_HEAD found on success, or to the last XGDF_HEAD in the
 * ring if not successful.
*/
short
find_fontbyindex ( XGDF_HEAD *start, short index, long *ret)
{
	register short previd;
	register XGDF_HEAD *f, *p;

	f = start;
	previd = 0xffff;

	do
	{
		p = f;

		if (previd != p->font_head->id)
		{
			previd = p->font_head->id;
			index--;
		}
	} while (index && (f = p->next) );

	if (ret)
		*ret = (long)p;

	return index;	/* Will return 0 if successful */
}
/*
 * Find font by ID. Returns the index of the font in the ring
 * when successful, else NULL. 'ret' is filled with the address
 * of the found XGDF_HEAD on success, else it is not modified.
*/
short
find_fontbyid ( XGDF_HEAD *start, short id, long *ret)
{
	short previd, i, index;
	XGDF_HEAD *f, *p;

	f = start;
	previd = start->font_head->id;
	index = 0;
	i = 1;

	do
	{
		p = f;
		if (p->font_head->id == id)
		{
			index = i;
			break;
		}
		else if (p->font_head->id != previd)
		{
			i++;
			previd = p->font_head->id;
		} 
	} while ((f = p->next));

	if (ret && index)
		*ret = (long)p;

	return index;	/* Will return 0 if unsuccessful */
}
/*
 * Find font ID by index. Used to look for different fonts having
 * the same ID. Returns index on success, or NULL if not successful.
 * 'ret' is filled in with the address of the found XGDF_HEAD on
 * success, not modified on failure.
*/
short
find_fontbyidindex(XGDF_HEAD *start, short id, short index, long *ret)
{
	int i;
	XGDF_HEAD *f, *p;

	f = start;
	i = 1;

	do
	{
		p = f;
		if (p->font_head->id == id)
		{
			if (i == index)
			{
				if (ret)
					*ret = (long)p;
				return i;
			}
			else
				i++;
		}
	} while ( (f = p->next) );

	return 0;	/* Will return 0 if unsuccessful */
}
/*
 * Find font by id and point.
 * Returns 0 (unsuccessful) if no font with ID 'id' was located.
 * Returns 1 if a perfect match was found (id and point)
 * Returns 2 if a font with ID 'id', but not 'point' was found.
 * 'ret' is filled in with address of XGDF_HEAD, or not modified
 * when unsuccessful.
*/
short
find_fontbypoint ( XGDF_HEAD *start, short id, short point, long *ret )
{
	XGDF_HEAD *f, *closest, *cu, *cd;
	unsigned short d, du, dd;

	f = start;
	closest = cu = cd = 0;
	du = dd = 0xffff;

	do
	{
		if (f->font_head->id == id)
		{
			if (f->font_head->point == point)
			{
				if (ret)
					*ret = (long)f;
				return 1;
			}

			if (f->font_head->point > point)
			{
				d = f->font_head->point - point;
				if (d < du)
				{
					du = d;
					cu = f;
				}
			}
			else
			{
				d = point - f->font_head->point;
				if (d < dd)
				{
					dd = d;
					cd = f;
				}
			}
		}
	}  while ( (f = f->next) );

	if (cu && cd)
		closest = cu; //dd < du ? cd : cu;
	else if (cu)
		closest = cu;
	else if (cd)
		closest = cd;

	if (closest)
	{
		if (ret)
			*ret = (long)closest;
		return 2;
	}
	else
		return 0;
}
/*
 * Find font by id and heigh.
 * Returns 0 (unsuccessful) if no font with ID 'id' was located.
 * Returns 1 if a perfect match was found (id and height)
 * Returns 2 if a font with ID 'id', but not 'height' was found.
 * 'ret' is filled in with address of XGDF_HEAD, or not modified
 * when unsuccessful.
*/
short
find_fontbyheight ( XGDF_HEAD *start, short id, short height, long *ret )
{
	XGDF_HEAD *f, *closest, *cu, *cd;
	unsigned short d, du, dd;

	f = start;
	closest = cu = cd = 0;
	du = dd = 0xffff;

	do
	{
		if (f->font_head->id == id)
		{
			if (f->font_head->top == height)
			{
				if (ret)
					*ret = (long)f;
				return 1;
			}

			if (f->font_head->top > height)
			{
				d = f->font_head->top - height;
				if (d < du)
				{
					du = d;
					cu = f;
				}
			}
			else
			{
				d = height - f->font_head->top;
				if (d < dd)
				{
					dd = d;
					cd = f;
				}
			}
		}
	} 
	while ( (f = f->next) );

	if (cu && cd)
		closest = cu; //dd < du ? cd : cu;
	else if (cu)
		closest = cu;
	else if (cd)
		closest = cd;

	if (closest)
	{
		if (ret)
			*ret = (long)closest;
		return 2;
	}
	else
		return 0;
}
/*
 * Find font by id and char width.
 * Returns 0 (unsuccessful) if no font with ID 'id' was located.
 * Returns 1 if a perfect match was found (id and width)
 * Returns 2 if a font with ID 'id', but not 'width' was found.
 * 'ret' is filled in with address of XGDF_HEAD, or not modified
 * when unsuccessful.
*/
short
find_fontbycharwidth ( XGDF_HEAD *start, short id, short width, long *ret )
{
	XGDF_HEAD *f, *closest, *cu, *cd;
	unsigned short d, du, dd;

	f = start;
	closest = cu = cd = 0;
	du = dd = 0xffff;

	do
	{
		if (f->font_head->id == id)
		{
			if (f->font_head->max_char_width == width)
			{
				if (ret)
					*ret = (long)f;
				return 1;
			}

			if (f->font_head->max_char_width > width)
			{
				d = f->font_head->max_char_width - width;
				if (d < du)
				{
					du = d;
					cu = f;
				}
			}
			else
			{
				d = width - f->font_head->max_char_width;
				if (d < dd)
				{
					dd = d;
					cd = f;
				}
			}
		}
	} 
	while ( (f = f->next) );

	if (cu && cd)
		closest = cu; //dd < du ? cd : cu;
	else if (cu)
		closest = cu;
	else if (cd)
		closest = cd;

	if (closest)
	{
		if (ret)
			*ret = (long)closest;
		return 2;
	}
	else
		return 0;
}

static short
fntncmp(char *s, char *d)
{
	char ch1, ch2;

	while (*s == 32){s++;}
	while (*d == 32){d++;}

	for (;;)
	{
		ch1 = *s++;
		if (ch1 == 32)
			while (*s == 32){s++;}
		ch2 = *d++;
		if (ch2 == 32)
			while (*d == 32){d++;}
		if (ch1 != ch2)
		{
			if (!ch1)
				return 0;
			else
				return 1;
		}
		else if (!ch1)
			return 0;
	}
}	
		
		
/*
 * Find a font by its name.
 * Returns  0 if successful, and 'ret' is filled in with address of XGDF_HEAD.
 * Returns -1 if not successful, and 'ret' is not modified.
*/
short
find_fontbyname( XGDF_HEAD *start, char *name, long *ret)
{
	XGDF_HEAD *p, *f;

	f = start;

	do
	{
		p = f;
		if ( !fntncmp(name, (char *)&p->font_head->name) )
			break;
	}
	while ((f = p->next));

	if (f && ret)
	{
		*ret = (long)p;
		return 0;
	}
	else
		return 1;
}

/*
 * Copy the full name of the font to buffer 'n'
*/
void
get_font_fullname(XGDF_HEAD *f, char *n)
{
	char *s;
	int i;

	s = (char *)&f->font_head->name;
	for (i = 32; i > 0 && (*n++ = *s++); i--){}
	*n = 0;
}
void
get_font_familyname(XGDF_HEAD *f, char *n)
{
	*n = 0;
}
void
get_font_stylename(XGDF_HEAD *f, char *n)
{
	*n = 0;
}
void
get_font_filename(XGDF_HEAD *f, char *n)
{
	char *s;
	int i;
	for (i = 200, s = (char *)&f->filename[0]; i > 0 && (*n++ = *s++); i--){}
	*n = 0;
}

void
get_font_chrfx_ofst( FONT_HEAD *f, short style, short *x_ofst, short *y_ofst )
{
	short x, y;

	x = y = 0;

	if (style & F_THICKEN)
		x += f->thicken;

	if (style & F_OUTLINE)
	{
		x += 2;
		y += 2;
	}

	*x_ofst = 0; //x;
	*y_ofst = 0; //y;

	return;
}
/* if len < 0, it treats the string as a null-terminated string. Else it returns
 * after len characters.
*/
short
gdf_string_width(FONT_HEAD *f, short *str, short len)
{
	int i;
	short *offtab;
	char *hortab;
	short fade, lade, chr, width;

	offtab = f->off_table;
	hortab = f->hor_table;

	fade = f->first_ade;
	lade = f->last_ade;
	width = 0;

	if (len < 0)
	{
		for (;;)
		{
			chr = *str++ & 0xff;

			if (!chr)
				break;

			if (chr < fade || chr > lade)
				chr = 0x3f;	/* '?' character */

			chr -= fade;
			width += offtab[chr + 1] - offtab[chr];

			if (hortab)
				width += (short)(hortab[chr] + hortab[chr + 1]);
		}
	}
	else
	{
		for (i = 0; i < len; i++)
		{
			chr = *str++ & 0xff;
			if (chr < fade || chr > lade)
				chr = 0x3f;	/* '?' character */

			chr -= fade;
			width += offtab[chr + 1] - offtab[chr];

			if (hortab)
				width += (short)(hortab[chr] + hortab[chr + 1]);
		}
	}

	return width;
}

short
gdf_get_so( FONT_HEAD *f, short *string, short len, short pixeloffset, short chrfx, GDFSO *inf)
{
	short i, chr, coff, fade, lade;
	short cwidth;
	short *offtable;
	char *hortable;

	inf->str_coff = 0;

	if (len <= 0)
		return 0;

	offtable = f->off_table;
	hortable = f->hor_table;
	coff = cwidth = 0;
	fade = f->first_ade;
	lade = f->last_ade;

	for (i = 0; i < len; i++)
	{

		chr = *string++ & 0xff;
		if (chr < fade || chr > lade)
			chr = 0x3f;

		chr -= fade;
		cwidth = offtable[chr + 1] - offtable[chr];

		if (hortable)
		{
			cwidth += hortable[chr] + hortable[chr + 1];
		}

		pixeloffset -= cwidth;

		if (pixeloffset <= 0)
			break;

		inf->str_coff++;

		coff += cwidth;
	}

	if (pixeloffset > 0)
		return 0;	/* Reached end of string before starting X .. */

	pixeloffset += cwidth;

	inf->width = cwidth;
	inf->str_poff = coff;
	inf->chr_poff = pixeloffset;

	return 1;
}

