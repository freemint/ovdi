#ifndef _TABLES_H
#define _TABLES_H

#include "gdf_defs.h"
#include "libkern.h"
#include "ovdi.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "v_attribs.h"
#include "v_fill.h"
#include "v_gdp.h"
#include "v_input.h"
#include "v_line.h"
#include "v_mouse.h"
#include "v_opcode_5.h"
#include "v_opcode_neg_1.h"
#include "v_pmarker.h"
#include "v_raster.h"
#include "v_text.h"
#include "v_time.h"
#include "workstation.h"


OVDI_VTAB v_vtab[MAX_VIRTUALS];

void (*v_njmptab[])() = {

	v_opcode_neg_1,
	v_nosys,
	v_nosys,
	v_nosys,
};

void (*v_jmptab[])() = {
	v_nosys,	/*   00 */
	v_opnwk,
	v_clswk,
	v_clrwk,
	v_updwk,
	v_opcode_5,
	v_pline,
	v_pmarker,
	v_gtext,
	v_fillarea,

	v_nosys,	/*   10 */	/* v_cellarray */
	v_gdp,
	vst_height,
	vst_rotation,
	vs_color,
	vsl_type,
	vsl_width,
	vsl_color,
	vsm_type,
	vsm_height,

	vsm_color,	/*   20 */
	vst_font,
	vst_color,
	vsf_interior,
	vsf_style,
	vsf_color,
	vq_color,
	v_nosys,			/* vq_cellarray */
	vxx_locator,			/* vrq_locator, vsm_locator */
	vxx_valuator,			/* vrq_valuator, vsm_valuator */

	vxx_choice,	/*   30 */	/* vrq_choice, vsm_choise */
	vxx_string,			/* vrq_string, vsm_string */
	vswr_mode,
	vsin_mode,			/* vsin_mode */
	v_nosys,			/* NOSYS */
	vql_attributes,
	vqm_attributes,
	vqf_attributes,
	vqt_attributes,
	vst_alignment,

 /* All opcodes 40 to 99 are NOSYS */
	v_nosys,	/*   40 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/*   50 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/*   60 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/*   70 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/*   80 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/*   90 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_opnvwk,	/*  100 */
	v_clsvwk,
	vq_extnd,			/* vq_extend */
	v_nosys,			/* v_contourfill */
	vsf_perimeter,
	v_get_pixel,
	vst_effects,
	vst_point,
	vsl_ends,
	vro_cpyfm,

	vr_trnfm,	/*  110 */	/* vr_trnfm */
	vsc_form,
	vsf_udpat,
	vsl_udsty,
	vr_recfl,
	vqin_mode,			/* vqin_mode */
	vqt_extent,
	vqt_width,
	vex_timv,
	vst_load_fonts,

	vst_unload_fonts,	/*  120 */
	vrt_cpyfm,
	v_show_c,
	v_hide_c,
	vq_mouse,
	vex_butv,
	vex_motv,
	vex_curv,
	vq_key_s,			/* vq_key_s */
	vs_clip,

	vqt_name,	/* 130 */
	vqt_fontinfo,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 140 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 150 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 160 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 170 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 180 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 190 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 200 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 210 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 220 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	vqt_xfntinfo,

	vst_name,	/* 230 */
	vst_width,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 240 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

	v_nosys,	/* 250 */
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,
	v_nosys,

};

DEV_TAB DEV_TAB_rom = 
{
	639,			/* 0	x resolution		*/
	399,			/* 1	y resolution		*/
	0,			/* 2	device precision 0=exact,1=not exact */
	372,			/* 3	width of pixel		*/
	372,			/* 4	heigth of pixel		*/
	1,			/* 5	character sizes		*/
	MAX_LN_STYLE,		/* 6	linestyles		*/
	0,			/* 7	linewidth		*/
	6,			/* 8	marker types		*/
	0,			/* 9	marker size		*/
	1,			/* 10	text font		*/
	MAX_FIL_PAT_INDEX,	/* 11	area patterns		*/
	MAX_FIL_HAT_INDEX,	/* 12	crosshatch patterns	*/
	2,			/* 13	colors at one time	*/
	10,			/* 14	number of GDP's		*/
	{
		1,		/* 15	GDP bar			*/
		2,		/* 16	GDP arc			*/
		3,		/* 17	GDP pic			*/
		4,		/* 18	GDP circle		*/
		5,		/* 19	GDP ellipse		*/
		6,		/* 20	GDP elliptical arc	*/
		7,		/* 21	GDP elliptical pie	*/
		8,		/* 22	GDP rounded rectangle	*/
		9,		/* 23	GDP filled rounded rectangle */
		10		/* 24	GDP #justified text	*/
	},
	{
		3,		/* 25	GDP #1	*/
		0,		/* 26	GDP #2	*/
		3,		/* 27	GDP #3	*/
		3,		/* 28	GDP #4	*/
		3,		/* 29	GDP #5	*/
		0,		/* 30	GDP #6	*/
		3,		/* 31	GDP #7	*/
		0,		/* 32	GDP #8	*/
		3,		/* 33	GDP #9	*/
		2		/* 34	GDP #10	*/
	},
	0,			/* 35	Color capability	*/
	1,			/* 36	Text Rotation		*/
	1,			/* 37	Polygonfill		*/
	0,			/* 38	Cell Array		*/
	2,			/* 39	Pallette size		*/
	2,			/* 40	# of locator devices 1 = mouse */
	1,			/* 41	# of valuator devices	*/
	1,			/* 42	# of choice devices	*/
	1,			/* 43	# of string devices	*/
	2			/* 44	Workstation Type 2 = out/in */
};



/*
 * SIZ_TAB - Returns text, line and marker sizes in device coordinates
 */

SIZ_TAB SIZ_TAB_rom =
{
	0,			/* 0	min char width		*/
	7,			/* 1	min char height		*/
	0,			/* 2	max char width		*/
	7,			/* 3	max char height		*/
	1,			/* 4	min line width		*/
	0,			/* 5	reserved 0		*/
	MAX_LN_WIDTH,		/* 6	max line width		*/
	0,			/* 7	reserved 0		*/
	15,			/* 8	min marker width	*/
	11,			/* 9	min marker height	*/
	120,			/* 10	max marker width	*/
	88			/* 11	max marker height	*/
};


/* Here's the template INQ_TAB, see lineavars.S for the normal INQ_TAB */
INQ_TAB INQ_TAB_rom =
{
	4,			/* 0  type of alpha/graphic controllers */
	1,			/* 1  number of background colors	*/
	0x1F,			/* 2  text styles supported		*/
	0,			/* 3  scale rasters = false		*/
	1,			/* 4  number of planes			*/
	0,			/* 5  video lookup table		*/
	50,			/* 6  performance factor????		*/
	0, /* update me */	/* 7  contour fill capability		*/
	0, /* update me */	/* 8  character rotation capability	*/
	4,			/* 9  number of writing modes		*/
	2,			/* 10 highest input mode		*/
	1,			/* 11 text alignment flag		*/
	0,			/* 12 Inking capability			*/
	0,			/* 13 rubber banding			*/
	128,			/* 14 maximum vertices - must agree with entry.s */
	-1,			/* 15 maximum intin			*/
	1,			/* 16 number of buttons on MOUSE	*/
	0,			/* 17 styles for wide lines		*/
	0,			/* 18 writing modes for wide lines	*/
	0,			/* 19 filled in with clipping flag	*/
	{
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0,
	},
};

/* Some color tables */
//static short Col2VDI[MAX_COLOR] =
// { 0, 15, 1, 2, 4, 6, 3, 5, 7, 8, 9, 10, 12, 14, 11, 13 };

//static short VDI2Col[MAX_COLOR] =
// { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };

short Planes2Pens[] =
 {   0, 2, 4, 0, 16, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 256,
   256, 0, 0, 0,  0, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0,   0, 256  };

short Planes2xinc[] = 
 { 1, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4 };

const short oVDI2HW_colorindex[] =
{
	0x00, 0xFF, 0x01, 0x02, 0x04, 0x06, 0x03, 0x05,
	0x07, 0x08, 0x09, 0x0A, 0x0C, 0x0E, 0x0B, 0x0D
};

short VDI2HW_colorindex[256] =
{
	0x00, 0xFF, 0x01, 0x02, 0x04, 0x06, 0x03, 0x05,
	0x07, 0x08, 0x09, 0x0A, 0x0C, 0x0E, 0x0B, 0x0D,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,		
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
	0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
	0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
	0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
	0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
	0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
	0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x0f

};

const short oHW2VDI_colorindex[] =
{
	0x00, 0x02, 0x03, 0x06, 0x04, 0x07, 0x05, 0x08,
	0x09, 0x0A, 0x0B, 0x0E, 0x0C, 0x0F, 0x0D, 0xFF
};

short HW2VDI_colorindex[256] =
{
	0x00, 0x02, 0x03, 0x06, 0x04, 0x07, 0x05, 0x08,
	0x09, 0x0A, 0x0B, 0x0E, 0x0C, 0x0F, 0x0D, 0xFF,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
	0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
	0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
	0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
	0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
	0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
	0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x01
};

#if 0
char ICB_MouseAccTab[12] =
 { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 255 };

short CalibColTab[18] =
{
	  0, 255, 255,
	255,   0, 255,
	255, 255,   0,

	255,   0,   0,
	0,   255,   0,
	0,     0, 255
};
#endif

/* pmaker definitions taken from fVDI (line.c) */
/* m_dot, m_plus, m_star, m_square, m_cross, m_dmnd */
static signed char row1[] = { 1, 2, 0, 0, 0, 0 };
static signed char row2[] = { 2, 2, 0, -3, 0, 3, 2, -4, 0, 4, 0 };
static signed char row3[] = { 3, 2, 0, -3, 0, 3, 2, 3, 2, -3, -2, 2, 3, -2, -3, 2};
static signed char row4[] = { 1, 5, -4, -3, 4, -3, 4, 3, -4, 3, -4, -3}; 
static signed char row5[] = { 2, 2, -4, -3, 4, 3, 2, -4, 3, 4, -3 };
static signed char row6[] = { 1, 5, -4, 0, 0, -3, 4, 0, 0, 3, -4, 0 };
signed char *marker[] = {row1, row2, row3, row4, row5, row6};


#endif	/* _TABLES_H */
