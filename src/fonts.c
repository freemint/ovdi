#include "display.h"
#include "file.h"
#include "fonts.h"
#include "gdf_defs.h"
#include "vdi_defs.h"

char fonts_buffer[];
char *nextfont = (char *)&fonts_buffer;
long total_fsize = 0;
char fonts_buffer[1024UL*1024UL];

/* This stupid use of a fixed buffer is only temporary! */
long
load_font( char *fn, long *size, long *loc)
{
	long fs, lbytes;
	char *b = nextfont;
	XGDF_HEAD *xf;

	fs = get_file_size(fn);

	if (total_fsize + fs + sizeof(XGDF_HEAD) > sizeof(fonts_buffer))
	{
		log("fontbuffer exhausted!\n");
		return -1;
	}

	if (fs < 0)
		return fs;

	xf = (XGDF_HEAD *)b;
	b += sizeof(XGDF_HEAD);

	lbytes = load_file( fn, fs, b );

	if (lbytes < 0)
		return lbytes;

	xf->font_head = (FONT_HEAD *)b;

	nextfont += lbytes+3 + sizeof(XGDF_HEAD);
	(long)nextfont &= (long)0xfffffffcUL;

	total_fsize += lbytes + sizeof(XGDF_HEAD);;

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
	

		tmp2 = (short *)((long)font + (long)font->off_table);
		
		for (i = 0; i < (font->last_ade - font->first_ade) + 2; i ++)
			*tmp2++ = ( (*tmp2)<<8 | (*tmp2)>>8 );

		if (font->flags & F_HORZ_OFF)
		{
			tmp2 = (short *)((long)font + (long)font->hor_table);
			for (i = 0 ; i < (font->last_ade - font->first_ade) ; i++ )
 				*tmp2++ = ( (*tmp2)<<8 | (*tmp2)>>8 );
		}
		font->flags |= F_STDFORM;
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

	return;
}

/* Returns 0 if added font was not a new face, only new size */
/* Returns 1 if font added was a new face */
/* Returns -1 if font already existed in chain */
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

short
find_fontbyindex ( XGDF_HEAD *start, short index, long *ret)
{
	register short previd;
	register XGDF_HEAD *f, *p;

	f = p = start;
	previd = 0xffff;

	//index++;
	while (index)
	{
		p = f;

		if (previd != f->font_head->id)
		{
			previd = f->font_head->id;
			index--;
		}
		else

		if (!(f = p->next))
			break;
	}

	if (ret)
		*ret = (long)p;

	return index;	/* Will return 0 if successful */
}

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

/* returns 0 if not found, 1 if ideal font found, or 2 if font ID, but not size */
/* found. If it returns 2, the font whose size was closest to the request is	*/
/* is returned. */
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

void
get_font_chrfx_ofst( FONT_HEAD *f, short style, short *x_ofst, short *y_ofst )
{
	register short x, y;

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
	register short *offtab;
	register char *hortab;
	register short fade, lade, chr, width;

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
	int i;
	short chr, coff, fade, lade, cwidth;
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

