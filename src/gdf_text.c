#include "display.h"
#include "fonts.h"
#include "gdf_text.h"
#include "ovdi_defs.h"
#include "rasters.h"
#include "vdi_defs.h"

void
output_gdftext( VIRTUAL *v, POINT *xy, short *text, short textlen, short jlen, short wf, short cf)
{
	int i;
	short chr, strwidth, width, words, spaces, direction, wordx, charx, rmwordx, rmcharx;
	short x1, y1, x2, y2, tmp, left_offset, right_offset, nxt_x1;
	long sc, sw, sca, swa, scs, sws;
	register VDIRECT clp, src, dst;
	MFDB fontd, screen;
	MFDB *fmfdb;
	short colors[2];
	short coords[8];

	FONT_HEAD *f;

	fmfdb = &fontd;

	f = v->font.header;
	x1 = xy->x;
	y1 = xy->y;

	strwidth = gdf_string_width(f, text, textlen);

	if (jlen && (cf || wf))
	{
		register short *p = text;
		short dwx, dcx;

		spaces = 0;
		words = 1;
		for (i = textlen; i > 0; i--)
		{
			if (*p++ == 0x20 || *p == 0)
			{
				spaces++;
				if ( i > 1 && *p != 0x20 && *p != 0)
					words++;
			}
		}

		if (cf && textlen > 1)
		{
			dcx = jlen - strwidth;
			charx = dcx / textlen;
			rmcharx = dcx % textlen;

			if (dcx < 0)
			{
				direction = -1;
				rmcharx = 0 - rmcharx;
			}
			else
				direction = 1;

			wordx = charx;
			rmwordx = rmcharx;
		}
		else if (wf && spaces)
		{
			dwx = jlen - strwidth;
			wordx = dwx / spaces;
			rmwordx = dwx % spaces;

			if (dwx < 0)
			{
				direction = -1;
				rmwordx = 0 - rmwordx;
			}
			else
				direction = 1;

			charx = 0;
			rmcharx = 0;
		}
		width = jlen;
	}
	else
	{
		jlen = 0;
		width = strwidth;
	}
		
	if (v->font.style & F_SKEW)
	{
		left_offset = f->left_offset;
		right_offset = f->right_offset;
	}
	else
	{
		left_offset = 0;
		right_offset = 0;
	}

	switch(v->font.halign)
	{
		case 0:		/* Left */
		{
			break;
		}
		case 1:		/* Centered */
		{
			x1 -= width >> 1;
			break;
		}
		case 2:		/* right */
		{
			x1 -= width;
			break;
		}
	}

	switch( v->font.valign )
	{
		case 0:		/* Base line */
		{
			y1 -= f->top;
			//x1 -= left_offset;
			break;
		}
		case 1:		/* Half line */
		{
			y1 -= f->top - f->half;
			//x1 -= (f->half * right_offset) / f->top;
			break;
		}
		case 2:		/* Ascent line */
		{
			y1 -= f->top - f->ascent;
			//x1 -= (f->ascent * right_offset) / f->top;
			break;
		}
		case 3:		/* Bottom */
		{
			y1 -= f->top + f->bottom;
			break;
		}
		case 4:		/* Descent */
		{
			y1 -= f->top + f->descent;
			//x1 -= (f->descent * left_offset) / f->bottom;
			break;
		}
		case 5:		/* Top */
		{
			//x1 -= left_offset + right_offset;
			break;
		}
	}

	x2 = x1 + (width - 1);
	y2 = y1 + (f->form_height - 1);

	dst.x1 = x1;
	dst.y1 = y1;
	dst.x2 = x2;
	dst.y2 = y2;

	if (x1 < v->clip.x1)
	{
		if (x2 < v->clip.x1)
			return;
		x1 = v->clip.x1;
	}
	if (x2 > v->clip.x2)
	{
		if (x1 > v->clip.x2)
			return;
		x2 = v->clip.x2;
	}
	if (y1 < v->clip.y1)
	{
		if (y2 < v->clip.y1)
			return;
		y1 = v->clip.y1;
	}
	if (y2 > v->clip.y2)
	{
		if (y1 > v->clip.y2)
			return;
		y2 = v->clip.y2;
	}

	clp.x1 = x1;
	clp.y1 = y1;
	clp.x2 = x2;
	clp.y2 = y2;

	src.y1 = clp.y1 - dst.y1;
	tmp = dst.y1 + (f->form_height - 1);
	src.y2 = tmp < clp.y2 ? f->form_height - 1 : ((f->form_height - 1) - (tmp - clp.y2));

	screen.fd_addr	= 0;
	colors[0] = 1, colors[1] = 0;

	coords[1] = src.y1, coords[3] = src.y2;
	coords[5] = clp.y1, coords[7] = clp.y2;

	expand_gdf_font( f, &fontd, *text++ & 0xff, (long)0);
	nxt_x1 = dst.x1;

	if (nxt_x1 > v->clip.x2)
		return;
	
	tmp = nxt_x1 + (fmfdb->fd_w - 1);
	if (tmp >= v->clip.x1)
	{
		coords[4] = nxt_x1 < v->clip.x1 ? v->clip.x1 : nxt_x1;
		coords[6] = tmp > v->clip.x2 ? v->clip.x2 : tmp;
		coords[0] = coords[4] - nxt_x1;
		coords[2] = coords[6] - nxt_x1;
		rt_cpyfm( v, &fontd, &screen, coords, v->font.color, v->font.bgcol, v->font.wrmode);
	}

	if (!(textlen--))
		return;

	if (jlen)
	{
		sca = (long)textlen << 16;
		scs = (long)rmcharx << 16;
		swa = (long)spaces << 16;
		sws = (long)rmwordx << 16;
		sc = sca;
		sw = swa;
	}

	for (i = 0; i < textlen; i++)
	{
		chr = *text++ & 0xff;
		nxt_x1 += fmfdb->fd_w;

		if (jlen)
		{
			if (cf)
			{
				nxt_x1 += charx;

				if (rmcharx && ((sc -= scs) < 0))
				{
					nxt_x1 += direction;
					sc += sca;
				}
			}
			else if (wf && (chr == 0x20 || chr == 0))
			{
				nxt_x1 += wordx;

				if (rmwordx && ((sw -= sws) < 0))
				{
					nxt_x1 += direction;
					sw += swa;
				}
			}
		}

		if (nxt_x1 > v->clip.x2)
			return;

		expand_gdf_font( f, &fontd, chr, (long)0);
		tmp = nxt_x1 + (fmfdb->fd_w - 1);

		if (tmp >= v->clip.x1)
		{
			coords[6] = tmp > v->clip.x2 ? v->clip.x2 : tmp;
			coords[4] = nxt_x1 < v->clip.x1 ? v->clip.x1 : nxt_x1;
			coords[0] = coords[4] - nxt_x1;
			coords[2] = coords[6] - nxt_x1;
			rt_cpyfm( v, (MFDB *)&fontd, (MFDB *)&screen, (short *)coords, v->font.color, v->font.bgcol, v->font.wrmode);
		}
	}
	return;
}

unsigned short fontdatabuff[1024 * 10];
unsigned short *nxtfdb = fontdatabuff;

void
expand_gdf_font( FONT_HEAD *f, MFDB *font, short chr, long *ret)
{
	int i, j;
	MFDB *fmfdb;
	unsigned short *fdatptr, *edatptr;
	short x1, x2, woffset, cwidth;
	short strtbits, groups, endbits, spans, spanm, ebm;
	unsigned short fdat;

	if (font)
		fmfdb = font;
	else
	{
		fmfdb = (MFDB *)nxtfdb;
		nxtfdb += sizeof(MFDB) >> 1;
	}

	fdatptr = (unsigned short *)f->dat_table;
	edatptr = (unsigned short *)nxtfdb;

	if (chr < f->first_ade || chr > f->last_ade)
		chr = 0x3f;

	chr -= f->first_ade;
	x1 = f->off_table[chr];
	x2 = f->off_table[chr + 1];
	cwidth = x2 - x1;
	x2--;

	fmfdb->fd_addr = &fontdatabuff;
	fmfdb->fd_w = cwidth;
	fmfdb->fd_h = f->form_height;
	fmfdb->fd_wdwidth = (cwidth + 15) >> 4;
	fmfdb->fd_stand = 0;
	fmfdb->fd_nplanes = 1;
	fmfdb->fd_r1 = 0;
	fmfdb->fd_r2 = 0;
	fmfdb->fd_r3 = 0;

	woffset = x1 >> 4;
	x1 &= 15;
	strtbits = (16 - x1) & 0xf;
	cwidth -= strtbits;

	if (cwidth <= 0)
	{
		strtbits = cwidth + strtbits;
		groups = endbits = 0;
	}
	else if (cwidth > 15)
	{
		endbits = (x2 + 1) & 0xf;
		groups = (cwidth - endbits) >> 4;
	}
	else
	{
		groups = 0;
		endbits = cwidth;
	}

	spans = (endbits + strtbits) - 16;
	if (spans <= 0)
		spans = 0;
	else
		spanm = 0xffff << spans;	/* spans mask */

	ebm = 0xffff << (16 - endbits);		/* End bits mask */

	for (j = f->form_height; j > 0; j--)
	{
		short woff = woffset;

		if (x1 || strtbits)
		{

			fdat = fdatptr[woff++] << x1;

			if (groups)
			{
				for ( i = groups; i > 0; i--)
				{
					fdat |= fdatptr[woff] >> strtbits;
					*edatptr++ = fdat;
					fdat = fdatptr[woff++] << x1;
				}
			}
			if (endbits)
				fdat |= (fdatptr[woff] & ebm) >> strtbits;

			*edatptr++ = fdat;
			if (spans)
				*edatptr++ = (fdatptr[woff] << x1) & spanm;
		}
		else
		{
			if (groups)
			{
				for (i = groups; i > 0; i--)
					*edatptr++ = fdatptr[woff++];
			}
			if (endbits)
				*edatptr++ = fdatptr[woff] & ebm;
		}
		fdatptr += f->form_width >> 1;
	}

	if (ret)
		*ret = (long)fmfdb;
	return;
}
