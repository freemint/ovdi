#include "v_attribs.h"
#include "patattr.h"
#include "vdi_globals.h"

/*
 * Set foreground color
*/
void
set_pa_fgcolor(PatAttr *p, COLINF *c, short color)
{
	int maxcolor;

	maxcolor = c->pens;

	if (color < 0)
		color = 0;
	else if (color >= maxcolor)
		color = maxcolor - 1;

	color = c->color_vdi2hw[color];

	if (p->color[0] != color)
	{
		p->color[0] = p->color[1] = color;
		p->color[2] = p->color[3] = c->planes > 8 ? 0x0 : 0xff;
		p->expanded = 0;
	}	
}
/*
 * Set background color
*/
void
set_pa_bgcolor(PatAttr *p, COLINF *c, short color)
{
	int maxcolor;

	maxcolor = c->pens;

	if (color < 0)
		color = 0;
	else if (color >= maxcolor)
		color = maxcolor - 1;

	color = c->color_vdi2hw[color];

	if (p->bgcol[0] != color)
	{
		p->bgcol[0] = p->bgcol[1] = color;
		p->bgcol[2] = p->bgcol[3] = c->planes > 8 ? 0xff : 0x0;
		p->expanded = 0;
	}
}
/*
 * Set VDI drawing mode
*/
void
set_pa_writemode(PatAttr *p, short wrm)
{
	int w;

	set_writingmode(wrm, (short *)&w);

	if (p->wrmode != w)
	{
		p->wrmode = w;
		p->expanded = 0;
	}
}
/****************************/
/*   Line specific settings */
/****************************/

/*
 * Set line ends for lines
*/
void
set_pa_lineends(PatAttr *p, short beg, short end)
{
	p->t.l.beg = beg <= MAX_LN_ENDS ? beg : MAX_LN_ENDS;
	p->t.l.end = end <= MAX_LN_ENDS ? end : MAX_LN_ENDS;
}
/*
 * Set line index for lines
*/
void
set_pa_lineindex(PatAttr *p, short index)
{
	if (index < 1)
		index = 1;
	else if (index > MAX_LN_STYLE)
		index = 1;

	index--;
	if (p->t.l.index != index)
	{
		if (index == LI_USER-1)
		{
			p->data = &p->ud;
			p->interior = FIS_PATTERN;
		}
		else
		{
			p->data = (unsigned short *)&LINE_STYLE[index];
			if (!index)
				p->interior = FIS_SOLID;
			else
				p->interior = FIS_PATTERN;
		}
		p->t.l.index = index;
		p->expanded = 0;
	}
}
/*
 * Set user defined line-style for lines
*/
void
set_pa_udline(PatAttr *p, unsigned short udline)
{
	p->ud = udline;
	if (p->t.l.index == LI_USER-1)
		p->expanded = 0;
}
/*
 * Set linewidth for lines
*/
void
set_pa_linewidth(PatAttr *p, short width)
{
	if (width < 1)
		p->t.l.width = 1;
	else if (width > MAX_L_WIDTH)
		p->t.l.width = MAX_L_WIDTH | 1;
	else
		p->t.l.width = width | 1;
}
/**********************************/
/*  Polymarker specific settings  */
/**********************************/

/*
 * Set line index for polymarkers
*/
void
set_pa_pmrk_lineindex(PatAttr *p, short index)
{
	if (index < 1 || index > MAX_LN_STYLE)
		index = 1;

	index--;
	if (p->t.p.index != index)
	{
		if (index == LI_USER-1)
		{
			p->data = &p->ud;
			p->interior = FIS_PATTERN;
		}
		else
		{
			p->data = (unsigned short *)&LINE_STYLE[index];
			if (!index)
				p->interior = FIS_SOLID;
			else
				p->interior = FIS_PATTERN;
		}
		p->t.p.index = index;
		p->expanded = 0;
	}
}
/*
 * Set polymarker width, height
*/
void
set_pa_pmrk_size(PatAttr *p, short width, short height)
{
	p->t.p.height = height;
	if (width)
		p->t.p.width = width;
	else
		p->t.p.width = height;
}
/*
 * Set polymarker type
*/
void
set_pa_pmrk_type(PatAttr *p, short type)
{
	if (type < MIN_PMARKERTYPE)
		type = MIN_PMARKERTYPE;
	else if (type > MAX_PMARKERTYPE)
		type = MAX_PMARKERTYPE;

	p->t.p.type = type - 1;
}
