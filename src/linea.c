#include <mintbind.h>

#include "display.h"
#include "draw.h"
#include "line.h"
#include "libkern.h"
#include "mouse.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "ovdi_types.h"
#include "vdi_defs.h"
#include "linea.h"
#include "polygon.h"
#include "vdi_globals.h"
#include "../../sys/mint/arch/asm_spl.h"

#define	VEC_LINEA	0x28

extern short logit;

//unsigned long	old_la_vect = 0;

extern long old_LineA_Handler;

extern void LineA_Initialize(void);
extern void LineA_PlotPixel(void);
extern void LineA_GetPixel(void);
extern void LineA_ArbLine(void);
extern void LineA_HorLine(void);
extern void LineA_FilledRect(void);
extern void LineA_FilledPoly(void);
extern void LineA_BitBlt(void);
extern void LineA_TextBlt(void);
extern void LineA_ShowMouse(void);
extern void LineA_HideMouse(void);
extern void LineA_TransformMouse(void);
extern void LineA_UndrawSprite(void);
extern void LineA_DrawSprite(void);
extern void LineA_CopyRaster(void);
extern void LineA_SeedFill(void);

extern void LineA_Handler(void);

extern short systemfont08[];
extern short systemfont09[];
extern short systemfont10[];

void linea_handler(short);
void linea_plot_pixel(void);
long linea_get_pixel(void);
void linea_arb_line(void);
void linea_hor_line(void);
void linea_filled_rect(void);
void linea_filled_poly(void);
void linea_bitblt(BITBLT *b);
void linea_textblt(void);
void linea_showmouse(void);
void linea_hidemouse(void);
void linea_transformmouse(void);
void linea_undrawsprite(MSAVE *ms);
void linea_drawsprite(MFORM *mf, short x, short y, MSAVE *ms);
void linea_copyraster(void);
void linea_seedfill(void);

void *linea_functab[] = 
{
	LineA_Initialize,
	LineA_PlotPixel,
	LineA_GetPixel,
	LineA_ArbLine,
	LineA_FilledRect,
	LineA_FilledPoly,
	LineA_BitBlt,
	LineA_TextBlt,
	LineA_ShowMouse,
	LineA_HideMouse,
	LineA_TransformMouse,
	LineA_UndrawSprite,
	LineA_DrawSprite,
	LineA_CopyRaster,
	LineA_SeedFill,
	0, 0, 0
};

FONT_HEAD *linea_fonts[] =
{
	(FONT_HEAD *)&systemfont08,
	(FONT_HEAD *)&systemfont09,
	(FONT_HEAD *)&systemfont10,
	(FONT_HEAD *)0
};
/* This is the remaining LineA variable table initialization. */
void
init_linea_vartab(VIRTUAL *v, LINEA_VARTAB *la)
{

	la->cur_font = sysfnt10p->font_head; //v->fring->;
	la->cur_work = v;
	la->def_font = la->cur_font = sysfnt10p->font_head;
	la->font_ring[0] = sysfnt08p->font_head;
	la->font_ring[1] = sysfnt10p->font_head;
	la->font_ring[2] = 0;
	la->font_ring[3] = 0;
	la->font_count = 3;

	linea_reschange(la, v->raster, v->colinf);

	return;
}

void
linea_reschange(LINEA_VARTAB *la, RASTER *r, COLINF *c)
{
	register short i;
	register short *dst;

	memcpy(&la->inq, &INQ_TAB_rom, sizeof(INQ_TAB));
	memcpy(&la->dev, &DEV_TAB_rom, sizeof(DEV_TAB));

	dst = (short *)&la->req_col;
	for (i = 0; i < 16; i++)
	{
		*dst++ = c->request_rgb[i].red;
		*dst++ = c->request_rgb[i].green;
		*dst++ = c->request_rgb[i].blue;
	}

	memcpy(&la->siz, &SIZ_TAB_rom, sizeof(SIZ_TAB));

	la->v_rez_hz	= r->w;
	la->v_rez_vt	= r->h;
	la->bytes_lin	= r->bypl;
	la->planes	= r->planes;
	la->width	= r->bypl;
	
}

void
set_linea_vector(void)
{
#ifndef PRG_TEST
	short sr;

	if (old_LineA_Handler)
		return;

	sr = spl7();
	old_LineA_Handler = *(long *)VEC_LINEA;
	*(long *)VEC_LINEA = (long)&LineA_Handler;
	spl(sr);
#endif
	return;
}
	
void
linea_handler(short func)
{
#if 0
	short pid = Pgetpid();
	if (logit)
		log("(%d), calls liena func %d\n\n", pid, func);
#endif
	return;
}
		
void
linea_plot_pixel(void)
{
	register LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;
	RASTER *r = v->raster;


	(*r->drawers->put_pixel)(r->base, r->bypl,
				  la->ptsin[0], la->ptsin[1], (unsigned long)la->intin[0]);
	return;
}

long
linea_get_pixel(void)
{
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;
	RASTER *r = v->raster;
	unsigned long pixel;
	short planes = v->raster->planes;

	pixel = (*r->drawers->get_pixel)(r->base, r->bypl,
					 la->ptsin[0], la->ptsin[1]);

	if (planes > 8)
	{
		int i;
		unsigned long *pv = v->colinf->pixelvalues;

		for (i = 0; i < 256; i++)
		{
			if (*pv++ == pixel)
				break;
		}

		if (i == 256)
			return 0;

		return (long)i;
	}
	else
		return pixel;
}

void
linea_arb_line(void)
{
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;
	PatAttr ptrn;
	short color = 0;

	if (la->colbit0)
		color |= 1;
	if (la->colbit1)
		color |= 2;
	if (la->colbit2)
		color |= 4;
	if (la->colbit3)
		color |= 8;

	ptrn.expanded = 0;
	ptrn.color[0] = v->colinf->color_hw2vdi[color];
	ptrn.bgcol[0] = v->colinf->color_hw2vdi[0];
	ptrn.width = 16;
	ptrn.height = 1;
	ptrn.planes = 1;
	ptrn.wrmode = la->wrmode;
	ptrn.data = (short *)&la->lnmask;

	abline(v->raster, v->colinf, (VDIRECT *)&la->x1, &ptrn);

	return;
}

void
linea_hor_line(void)
{
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;
	PatAttr ptrn;
	short color = 0;
	unsigned short *pdata;

	if (la->colbit0)
		color |= 1;
	if (la->colbit1)
		color |= 2;
	if (la->colbit2)
		color |= 4;
	if (la->colbit3)
		color |= 8;

	ptrn.expanded = 0;
	ptrn.color[0] = v->colinf->color_hw2vdi[color];
	ptrn.bgcol[0] = v->colinf->color_hw2vdi[0];
	ptrn.width = 16;
	ptrn.height = 1;
	ptrn.planes = 1;
	ptrn.wrmode = la->wrmode;
	pdata = la->patptr;
	ptrn.data = &pdata[la->patmsk];

	habline(v->raster, v->colinf, la->x1, la->x2, la->y1, &ptrn);

	return;
}

void
linea_filled_rect(void)
{
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;
	RASTER *r = v->raster;
	PatAttr ptrn;
	short color = 0;
	short *pdata;

	if (la->colbit0)
		color |= 1;
	if (la->colbit1)
		color |= 2;
	if (la->colbit2)
		color |= 4;
	if (la->colbit3)
		color |= 8;

	ptrn.expanded = 0;
	ptrn.color[0] = v->colinf->color_hw2vdi[color];
	ptrn.bgcol[0] = v->colinf->color_hw2vdi[0];
	ptrn.width = 16;
	ptrn.height = 1;
	ptrn.planes = 1;
	ptrn.wrmode = la->wrmode;
	pdata = la->patptr;
	ptrn.data = &pdata[la->patmsk];

	if (la->clip)
		rectfill(r, v->colinf, (VDIRECT *)&la->x1, (VDIRECT *)&la->xmincl, &ptrn, FIS_PATTERN);
	else
		rectfill(r, v->colinf, (VDIRECT *)&la->x1, (VDIRECT *)&r->x1, &ptrn, FIS_PATTERN);

	return;
}

void
linea_filled_poly(void)
{
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;
	RASTER *r = v->raster;
	VDIRECT *clip;
	PatAttr ptrn;
	short color = 0;
	short *pdata;
	short spanbuff[100];

	if (la->colbit0)
		color |= 1;
	if (la->colbit1)
		color |= 2;
	if (la->colbit2)
		color |= 4;
	if (la->colbit3)
		color |= 8;

	ptrn.expanded = 0;
	ptrn.color[0] = v->colinf->color_hw2vdi[color];
	ptrn.bgcol[0] = v->colinf->color_hw2vdi[0];
	ptrn.width = 16;
	ptrn.height = 1;
	ptrn.planes = 1;
	ptrn.wrmode = la->wrmode;
	pdata = la->patptr;
	ptrn.data = &pdata[la->patmsk];

	clip = la->clip ? (VDIRECT *)&la->xmincl : (VDIRECT *)&r->x1;
	filled_poly(r, v->colinf, (short *)&la->ptsin[0], la->contrl[1], clip, (short *)&spanbuff, sizeof(spanbuff), &ptrn);

	return;
}

void
linea_bitblt(BITBLT *b)
{
#if 0
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;
	short wrmode = 0;
	MFDB src, dst;

	src.fd_addr = b->s_form;
	dst.fd_addr = b->d_form;

	src.fd_w = dst.fd_w = b->width;
	src.fd_h = dst.fd_h = b->height;
	src.fd_wdwidth = b->s_nxln << 1;
	dst.fd_wdwidth = b->d_nxln << 1;
	src.fd_stand = dst.fd_stand = 1;
	src.fd_nplanes = b->planes;
#endif
	return;

}

void
linea_textblt(void)
{
#if 0
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;
	short wrmode = 0;
	MFDB fmfdb, dst;
#endif

	return;
}

void
linea_showmouse(void)
{
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;

	(*v->mouseapi->showmcurs)(0);

	return;
}

void
linea_hidemouse(void)
{
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;

	(*v->mouseapi->hidemcurs)();

	return;
}

void
linea_transformmouse(void)
{
	LINEA_VARTAB *la = linea_vars;
	VIRTUAL *v = la->cur_work;

	(*v->mouseapi->setnewmform)((MFORM *)(((long)la->intin[0] << 16) | (unsigned short)la->intin[1]));

	return;
}

void
linea_undrawsprite(MSAVE *ms)
{
	return;
}

void
linea_drawsprite(MFORM *mf, short x, short y, MSAVE *ms)
{
	return;
}

void
linea_copyraster(void)
{
	return;
}

void
linea_seedfill(void)
{
	return;
}
	