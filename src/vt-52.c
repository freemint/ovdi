#include <osbind.h>

#include "console.h"
#include "draw.h"
#include "display.h"
#include "fonts.h"
#include "file.h"
#include "gdf_defs.h"
#include "gdf_text.h"
#include "libkern.h"
#include "memory.h"
#include "ovdi_defs.h"
#include "ovdi_rasters.h"
#include "ovdi_types.h"
#include "rasters.h"
#include "vbiapi.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "../../sys/mint/arch/asm_spl.h"

#define UP	0
#define DOWN	1

/* bconout_vec = 0x57e */
#define	con_state	*(long *)(0x4a8)
#define xconout_console	*(long *)(0x586)
#define xconout_raw	*(long *)(0x592)
#define bell_hook	*(long *)(0x5ac)
#define conterm		*(char *)(0x484)

static void set_vectors(void);
static void recalc_columnrows(CONSOLE *c);
static void new_font(CONSOLE *c, FONT_HEAD *f);

static EscFunc ctrl_codes[] =
{
	bell,
	Esc_D, /*backspace*/
	tab,
	LineFeed, LineFeed, LineFeed,
	CarrigeReturn,
};

static EscFunc	UC_escapes[] =
{
	Esc_A,
	Esc_B,
	Esc_C,
	Esc_D,
	Esc_E,
	Esc_nosys,
	Esc_nosys,
	Esc_H,
	Esc_I,
	Esc_J,
	Esc_K,
	Esc_L,
	Esc_M,
	Esc_nosys, Esc_nosys, Esc_nosys, Esc_nosys,
	Esc_nosys, Esc_nosys, Esc_nosys, Esc_nosys,
	Esc_nosys, Esc_nosys, Esc_nosys,
	Esc_Y,
	Esc_nosys
};

static EscFunc LC_escapes[] =
{
	Esc_nosys,
	Esc_b,
	Esc_c,
	Esc_d,
	Esc_e,
	Esc_f,
	Esc_nosys, Esc_nosys, Esc_nosys,
	Esc_j,
	Esc_k,
	Esc_l,
	Esc_nosys, Esc_nosys,
	Esc_o,
	Esc_p,
	Esc_q,
	Esc_nosys, Esc_nosys, Esc_nosys, Esc_nosys,
	Esc_v,
	Esc_w,
	Esc_nosys, Esc_nosys, Esc_nosys
};

static char UC_escapeflags[] =
{
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 0
};
static char LC_escapeflags[] =
{
	0, 1, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0
};

static unsigned short consfill = 0xffff;

static CONSOLE console;

static COLINF colinf;
static short vdi2hw[256];
static short hw2vdi[256];
static RGB_LIST request_rgb[256];
static RGB_LIST actual_rgb[256];
static unsigned long pixelvalues[256];

/* init console structure. All console functions will reference the
 * VIRTUAL structure passed here.
*/
//void
//init_console(VIRTUAL *v, LINEA_VARTAB *la)
CONSOLE *
init_console(OVDI_HWAPI *hw, RASTER *r, LINEA_VARTAB *la)
{
	CONSOLE *c = &console;

	bzero(&console, sizeof(CONSOLE));

	c->r	= r;
	c->drv	= hw->driver;
	c->la	= la;

	c->curs_hide_ct = 1;
	c->tps = (*hw->vbi->get_tics)();

	/*
	 * console driver
	*/
	c->enter_console	= &console_enter;
	c->exit_console		= &console_exit;
	c->draw_text_cursor	= &draw_text_cursor;
	c->undraw_text_cursor	= &undraw_text_cursor;
	c->textcursor_blink	= &do_txtcurs_blnk;
	c->scroll_lines		= &scroll_lines;
	c->erase_lines		= &erase_lines;
	c->draw_character	= &draw_character;
	c->csout_char		= &cs_output_character;
	
	la->v_period = 1 * 66;
	la->v_cur_ct = 1 * 66;

	la->v_cur_flag	= V_LINEWRAP;

	c->tc_type = TCT_LEFT_SL;
	new_font(c, sysfnt10p->font_head);
	change_console_resolution(c, r);

	return c;
}

void
install_console_handlers(CONSOLE *c)
{
	set_constate(c, (long)&con_state_handler);
	set_xconout_raw(c, (long)&rawcon_output);

	(void)Supexec(set_vectors);
}

static void
set_vectors(void)
{
	short sr;

	sr = spl7();
	old_con_state	= con_state;
	con_state 	= (long)&do_con_state;

	old_xconout_console = xconout_console;
	xconout_console = (long)&do_xconout_console;

	old_xconout_raw = xconout_raw;
	xconout_raw = (long)&do_xconout_raw;
	spl(sr);
}

static void
recalc_columnrows(CONSOLE *c)
{
	LINEA_VARTAB *la = c->la;
	FONT_HEAD *f	= c->f;
	RASTER *r	= c->r;

	la->v_cur_x	= la->v_cur_y = la->v_sav_x = la->v_sav_y = 0;
	la->v_cel_ht	= f->top + f->bottom + 1;
	la->v_cel_wr	= 0;	/* Not used!!! */
	la->v_cel_mx	= ((r->x2 + 1) / f->max_cell_width) - 1;
	la->v_cel_my	= ((r->y2 + 1) / (f->top + f->bottom + 1)) - 1;
}

static void
new_font(CONSOLE *c, FONT_HEAD *f)
{
	LINEA_VARTAB *la = c->la;

	c->f = f;

	la->v_fnt_ad = f->dat_table;
	la->v_fnt_nd = f->last_ade;
	la->v_fnt_st = f->first_ade;
	la->v_fnt_wd = f->form_width;
	la->v_off_ad = f->off_table;

	la->fbase = f->dat_table;
	la->fwidth = f->form_width;
	la->style = 0;
	la->litemask = f->lighten;
	la->weight = f->thicken;
	la->roff = f->right_offset;
	la->loff = f->left_offset;
	la->scale = 0;
	la->chup = 0;

	la->v_cur_ad	= (unsigned char *)c->r->base;
	la->v_cur_of	= 0;
}

void
change_console_resolution(CONSOLE *c, RASTER *r)
{
	int i, hwpen;
	//FONT_HEAD *f = c->f;
	LINEA_VARTAB *la = c->la;
	COLINF *cinf;

	c->r = r;

	if (c->colinf)
		cinf = c->colinf;
	else
	{
		cinf = &colinf;
		cinf->color_vdi2hw = (short *)&vdi2hw;
		cinf->color_hw2vdi = (short *)&hw2vdi;
		cinf->pixelvalues = (unsigned long *)&pixelvalues;
		cinf->request_rgb = (RGB_LIST *)&request_rgb;
		cinf->actual_rgb = (RGB_LIST *)&actual_rgb;

		c->colinf = cinf;
	}
	
	init_colinf(r, cinf);

	if (r->res.clut)
	{
		for (i = 0; i < cinf->pens; i++)
		{
			hwpen = cinf->color_vdi2hw[i];
			(*c->drv->dev->setcol)(c->drv, hwpen, &cinf->actual_rgb[hwpen]);
		}
	}

	c->pattern.expanded = 0;
	c->pattern.color[0] = c->pattern.color[1] = cinf->color_vdi2hw[0];
	c->pattern.color[2] = c->pattern.color[3] = r->res.planes > 8 ? 0 : 0xff;
	c->pattern.bgcol[0] = c->pattern.bgcol[1] = cinf->color_vdi2hw[1];
	c->pattern.bgcol[2] = c->pattern.bgcol[3] = r->res.planes > 8 ? 0xff : 0x0;

	c->pattern.width	= 16;
	c->pattern.height	= 1;
	c->pattern.wwidth	= 1;
	c->pattern.planes	= 1;
	c->pattern.wrmode	= 2;
	c->pattern.data		= &consfill;
	c->pattern.exp_data	= (unsigned short *)&c->pd.edata;
	c->pattern.mask		= (unsigned short *)&c->pd.mask;

	/* Initialize the Line A variables used by the console/vt52 emulator */
	la->v_col_fg	= cinf->color_vdi2hw[1];
	la->v_col_bg	= cinf->color_vdi2hw[0];

	la->textfg	= cinf->color_vdi2hw[1];
	la->textbg	= cinf->color_vdi2hw[0];

	recalc_columnrows(c);
}

void
set_console_font(CONSOLE *c, const char *path, char *fname)
{
	FONT_HEAD *f;
	char *dst, *src;
	long fs;
	char fqpn[128+32];
	
 /* Create Fully Qualified Path Name and get size of fontfile */
	dst = (char *)&fqpn;
	if (path)
		(const char *)src = path;
	else if (c->fontpath)
		(const char *)src = c->fontpath;
	else
		src = 0;

	if (src)
		while (*src) *dst++ = *src++;
	src = fname;
	while (*src) *dst++ = *src++;
	*src = 0;

	src = (char *)&fqpn;
	fs = get_file_size( src );

	if (fs > 0)
	{
		f = (FONT_HEAD *)omalloc(fs, 0);
		if (f)
		{
			if ( load_file( src, fs, (char *)f) != fs)
			{
				free_mem(f);
				return;
			}
			/* Font sucessfully loaded, now we can point things to the new font */
			if (c->loaded_font)
				free_mem(c->loaded_font);
			fixup_font(f);
			new_font(c, f);
			recalc_columnrows(c);
			c->loaded_font = f;
			c->f = f;
			if (path)
				c->fontpath = path;
			dst = c->fontfile;
			while (*fname) *dst++ = *fname++;
			*dst = 0;
		}
	}
}

void
enter_console(CONSOLE *c)
{
	(*c->enter_console)(c);
	Esc_E(c);
	text_cursor_on(c);
	return;
};

void
exit_console(CONSOLE *c)
{
	text_cursor_off(c);
	Esc_E(c);
	(*c->exit_console)(c);
	return;
};

short
conf_textcursor_blink(CONSOLE *c, short mode, short rate)
{
	int oldrate;

	oldrate = c->blinkrate;

	if (rate != 0xffff)
		c->blinkrate = rate;

	if (mode != 0xffff)
	{
		if (mode == 0)
			c->tc_flags &= ~BLINK_ON;
		else if (mode == 1)
			c->tc_flags |= BLINK_ON;
	}
		
	return oldrate;
}

void
textcursor_blink(CONSOLE *c)
{
	if (!c)
		return;

	if (!c->curs_hide_ct && (c->la->v_cur_flag & V_CURSON)) // && c->tc_flags & BLINK_ON)
	{
		if (c->la->v_cur_flag & V_CURSDRAWN)
		{
			c->la->v_cur_flag &= ~V_CURSDRAWN;
			(*c->undraw_text_cursor)(c);
		}
		else
		{
			c->la->v_cur_flag |= V_CURSDRAWN;
			(*c->draw_text_cursor)(c);
		}
	}
	return;
}

void
con_state_handler(CONSOLE *c, short character)
{
	register short chr = character & 0xff;

	if (chr > 31)
	{
		rawcon_output(c, chr);
		return;
	}

	if (chr == 27)
	{
		set_constate(c, (long)&VT52_handler);
		return;
	}

	if (chr > 6 && chr < 14)
	{
		EscFunc f;

		f = ctrl_codes[chr - 7];
		hide_text_cursor(c);
		(*f)(c);
		show_text_cursor(c);
		return;
	}
	return;
}

void
bell(CONSOLE *c)
{
	if ((conterm & 4) && bell_hook)
		call_bellhook();

	return;
}

void
tab(CONSOLE *c)
{
	register int column, columns;

	columns = c->la->v_cel_mx;
	column = (c->la->v_cur_x & ~7) + 8;

	if (column > columns)
		column = columns;
	move_text_cursor(c, column, c->la->v_cur_y);
	return;
}
void
LineFeed(CONSOLE *c)
{
	register int row = c->la->v_cur_y + 1;

	if (row > c->la->v_cel_my)
	{
		(*c->scroll_lines)(c, 0, 1, UP);
		(*c->erase_lines)(c, 0, c->la->v_cel_my, c->la->v_cel_mx, c->la->v_cel_my);
		row = c->la->v_cel_my;
	}

	move_text_cursor(c, c->la->v_cur_x, row);
	return;
}

void
CarrigeReturn(CONSOLE *c)
{
	move_text_cursor(c, 0, c->la->v_cur_y);
	return;
}

	
void			
rawcon_output(CONSOLE *c, short character)
{
	register int column = c->la->v_cur_x + 1;
	register int row = c->la->v_cur_y;
	register int chr = character & 0xff;

	hide_text_cursor(c);
	(*c->draw_character)(c, chr);

	if (column > c->la->v_cel_mx)
	{
		if (c->la->v_cur_flag & V_LINEWRAP)
		{
			
			row++;
			column = 0;
			if (row > c->la->v_cel_my)
			{
				row = c->la->v_cel_my;
				(*c->scroll_lines)(c, 0, 1, UP);
				(*c->erase_lines)(c, 0, c->la->v_cel_my,
						 c->la->v_cel_mx, c->la->v_cel_my);
			}
		}
		else
			column = c->la->v_cel_mx;

	}
	move_text_cursor(c, column, row);
	show_text_cursor(c);
	return;
}

void
VT52_handler(CONSOLE *c, short character)
{
	register int chr;
	register EscFunc ef;
	register char flag;

	chr = character & 0xff;

	if (chr > 0x40 && chr < 0x5b)
	{
		chr -= 0x41;
		ef = UC_escapes[chr];
		flag = UC_escapeflags[chr];
	}
	else if (chr > 0x60 && chr < 0x7b)
	{
		chr -= 0x61;
		ef = LC_escapes[chr];
		flag = LC_escapeflags[chr];
	}
	else
	{
		set_constate(c, (long)&con_state_handler);
		return;
	}

	(*ef)(c);

	if (!flag)
		set_constate(c, (long)&con_state_handler);

	return;
}

/*
* Move the cursor up one line. If the cursor is on the top
* line this does nothing.
*/
void
Esc_nosys(CONSOLE *c)
{
	set_constate(c, (long)&con_state_handler);
	return;
}

void
Esc_A(CONSOLE *c)
{
	register int row;

	row = c->la->v_cur_y - 1;

	if (row < 0)
		return;

	move_text_cursor(c, c->la->v_cur_x, row);
	return;
}

/*
* Move the cursor down one line. If the cursor is on the
* bottom line this does nothing.
*/
void
Esc_B(CONSOLE *c)
{
	register int row = c->la->v_cur_y + 1;

	if (row > c->la->v_cel_my)
		return;

	move_text_cursor(c, c->la->v_cur_x, row);
	return;
}

/* 
* Move the cursor right one character. If the cursor is on the
* far right of the screen this does nothing.
*/
void
Esc_C(CONSOLE *c)
{
	register int column = c->la->v_cur_x + 1;

	if (column > c->la->v_cel_mx)
		return;

	move_text_cursor(c, column, c->la->v_cur_y);
	return;
}

/*
* Move the cursor left one character. If the cursor is on the far
* left of the screen this does nothing.
*/
void
Esc_D(CONSOLE *c)
{
	register int column = c->la->v_cur_x - 1;

	if (column < 0)
		return;

	move_text_cursor(c, column, c->la->v_cur_y);
	return;
}

/*
* Clear the screen and place the cursor at the upper-left
* corner
*/
void
Esc_E(CONSOLE *c)
{
	hide_text_cursor(c);
	(*c->erase_lines)(c, 0, 0, c->la->v_cel_mx, c->la->v_cel_my);
	Esc_H(c);
	show_text_cursor(c);
	return;
}

/*
* Move the cursor to the upper-left corner of the screen.
*/
void
Esc_H(CONSOLE *c)
{
	move_text_cursor(c, 0, 0);
	return;
}

/*
* Move the cursor up one line. If the cursor is on the top
* line, the screen scrolls down one line.
*/
void
Esc_I(CONSOLE *c)
{
	register int row = c->la->v_cur_y - 1;

	hide_text_cursor(c);

	if (row < 0)
	{
		(*c->scroll_lines)(c, 0, 1, DOWN);
		(*c->erase_lines)(c, 0, 0, c->la->v_cel_mx, 0);
	}
	else
		move_text_cursor(c, c->la->v_cur_x, row);

	show_text_cursor(c);

	return;
}

/*
* Erase the screen downwards from the current position of
* the cursor.
*/
void
Esc_J(CONSOLE *c)
{
	hide_text_cursor(c);
	(*c->erase_lines)(c,  c->la->v_cur_x, c->la->v_cur_y,
			c->la->v_cel_mx, c->la->v_cel_my);
	show_text_cursor(c);
	return;
}

/*
* Clear the current line to the right from the cursor
* position.
*/
void
Esc_K(CONSOLE *c)
{
	hide_text_cursor(c);
	(*c->erase_lines)(c,  c->la->v_cur_x, c->la->v_cur_y,
			c->la->v_cel_mx, c->la->v_cur_y);
	show_text_cursor(c);
	return;
}

/*
* Insert a line by scrolling all lines at the cursor position
* down one line.
*/
void
Esc_L(CONSOLE *c)
{
	hide_text_cursor(c);
	(*c->scroll_lines)(c, c->la->v_cur_y, 1, DOWN);
	(*c->erase_lines)(c, 0, c->la->v_cur_y,
			 c->la->v_cel_mx, c->la->v_cur_y);
	show_text_cursor(c);
	return;
}

/*
* Delete the current line and scroll lines below the cursor
* position up one line.
*/
void
Esc_M(CONSOLE *c)
{
	hide_text_cursor(c);
	(*c->scroll_lines)(c, c->la->v_cur_y + 1, 1, UP);
	(*c->erase_lines)(c, 0, c->la->v_cel_my, c->la->v_cel_mx, c->la->v_cel_my);
	show_text_cursor(c);
	return;
}

/*
* Position the cursor at the coordinates given by the
* following two codes. The screen starts with coordinates
* (32, 32) at the upper-left of the screen. Coordinates
* should be presented in reverse order, Y and then X.
*/
void
Esc_Y(CONSOLE *c)
{
	set_constate(c, (long)&Esc_Y_save_row);
	return;
}
void
Esc_Y_save_row(CONSOLE *c, short row)
{
	c->save_row = (row & 0xff) - 32;
	set_constate(c, (long)&Esc_Y_save_column);
	return;
}
void
Esc_Y_save_column( CONSOLE *c, short column)
{
	move_text_cursor(c, (column & 0xff) - 32, c->save_row);
	set_constate(c, (long)&con_state_handler);
	return;
}

/*
* This code is followed by a character from which the lowest
* four bits determine a new text foreground color.
*/
void
Esc_b(CONSOLE *c)
{
	set_constate(c, (long)&Esc_b_collect);
	return;
}
void
Esc_b_collect(CONSOLE *c, short color)
{
	c->la->v_col_fg = c->colinf->color_vdi2hw[color & 0xf];
	set_constate(c, (long)&con_state_handler);
	return;
}

/*
* This code is followed by a character from which the lowest
* four bits determine a new text background color.
*/
void
Esc_c(CONSOLE *c)
{
	set_constate(c, (long)&Esc_c_collect);
	return;
}
void
Esc_c_collect(CONSOLE *c, short color)
{
	c->la->v_col_bg = c->colinf->color_vdi2hw[color & 0xf];
	set_constate(c, (long)&con_state_handler);
	return;
}

/*
* Earse the screen from the upper-left to the current cursor
* position.
*/
void
Esc_d(CONSOLE *c)
{
	hide_text_cursor(c);
	(*c->erase_lines)(c, 0, 0, c->la->v_cur_x, c->la->v_cur_y);
	show_text_cursor(c);
	return;
}

/*
* Enable the cursor
*/
void
Esc_e(CONSOLE *c)
{
	text_cursor_on(c);
	return;
}

/*
* Disable the cursor
*/
void
Esc_f(CONSOLE *c)
{
	text_cursor_off(c);
	return;
}

/*
* Save the current cursor position. (only implemented as of
* TOS 1.02)
*/
void
Esc_j(CONSOLE *c)
{
	c->la->v_sav_x = c->la->v_cur_x;
	c->la->v_sav_y = c->la->v_cur_y;
	return;
}

/*
* Restore the current cursor position. (only implemented as of
* TOS 1.02)
*/
void
Esc_k(CONSOLE *c)
{
	move_text_cursor(c, c->la->v_sav_x, c->la->v_sav_y);
	return;
}

/*
* Erase the current line and place the cursor at the far left.
*/
void
Esc_l(CONSOLE *c)
{
	hide_text_cursor(c);
	(*c->erase_lines)(c, 0, c->la->v_cur_y, c->la->v_cel_mx, c->la->v_cur_y);
	move_text_cursor(c, 0, c->la->v_cur_y);
	show_text_cursor(c);
	return;
}

/*
* Erase the current line from the far left to the current
* cursor position.
*/
void
Esc_o(CONSOLE *c)
{
	hide_text_cursor(c);
	(*c->erase_lines)(c, 0, c->la->v_cur_y, c->la->v_cur_x, c->la->v_cur_y);
	show_text_cursor(c);
	return;
}

/*
* Enable inverse video.
*/ 
void
Esc_p(CONSOLE *c)
{
	c->la->v_cur_flag |= V_INVERSED;
	return;
}

/*
* Disable inverse video.
*/
void
Esc_q(CONSOLE *c)
{
	c->la->v_cur_flag &= ~V_INVERSED;
	return;
}

/*
* Enable line wrap.
*/
void
Esc_v(CONSOLE *c)
{
	c->la->v_cur_flag |= V_LINEWRAP;
	return;
}

/*
* Disable line wrap.
*/
void
Esc_w(CONSOLE *c)
{
	c->la->v_cur_flag &= ~V_LINEWRAP;
	return;
}

void
reset_text_cursor(CONSOLE *c)
{
	c->curs_hide_ct = 1;
	show_text_cursor(c);
	return;
}

void
move_text_cursor(CONSOLE *c, short x, short y)
{
	register int mx, my;

	mx = c->la->v_cel_mx;
	my = c->la->v_cel_my;

	if (x < 0)
		x = 0;
	else if (x > mx)
		x = mx;

	if (y < 0)
		y = 0;
	else if (y > my)
		y = my;

	hide_text_cursor(c);
	c->la->v_cur_x = x;
	c->la->v_cur_y = y;
	show_text_cursor(c);
	return;
}

void
text_cursor_off( CONSOLE *c)
{
	hide_text_cursor(c);
	c->la->v_cur_flag &= ~V_CURSON;
	return;
}
void
text_cursor_on( CONSOLE *c)
{
	c->la->v_cur_flag |= V_CURSON;
	reset_text_cursor(c);
	return;
}

void
show_text_cursor( CONSOLE *c)
{

	c->curs_hide_ct--;

	if (c->curs_hide_ct < 0)
		c->curs_hide_ct = 0;

	if (!c->curs_hide_ct && (c->la->v_cur_flag & (V_CURSDRAWN|V_CURSON)) == V_CURSON)
	{
		(*c->draw_text_cursor)(c);
		c->la->v_cur_flag |= V_CURSDRAWN;
	}
	return;
}

void
hide_text_cursor( CONSOLE *c)
{

	c->curs_hide_ct++;

	if ((c->la->v_cur_flag & (V_CURSDRAWN | V_CURSON)) == (V_CURSDRAWN | V_CURSON) )
	{
		(*c->undraw_text_cursor)(c);
		c->la->v_cur_flag &= ~V_CURSDRAWN;
	}
	return;
}

/* =============================================================== */
/* --------------------------------------------------------------- */
/* The stuff below here is the console 'driver'!                   */
/* --------------------------------------------------------------- */
/* =============================================================== */

void
console_enter(CONSOLE *c)
{
	/* Nothing to do. This is where drivers setup the video card
	*  to text mode, or does things needed to enter console.
	*  Virtual consoles, anyone?
	*/
}

void
console_exit(CONSOLE *c)
{
	/* Nothing to do. This is where drivers go back to the normal
	*  mode, which will show the AES, for example.
	*/
}
/* Erase nlines from cursor x and y.
* If nlines == -1, erase from x, y to end of screen.
* If nlines ==  0, erase from x to start/end of line only.
* if nlines == **, erase that many lines.
* If starting x and y above cursors x and y, erasing
* goes from start x, y to cursor x, y.
*/
void
erase_lines( CONSOLE *c, short x1, short y1, short x2, short y2)
{
	FONT_HEAD *f;
	RASTER *r;
	COLINF *ci;
	Ffilled_rect fr;

	int cwidth, cheight, lines;
	short coords[4];

	f = c->f;
	r = c->r;
	ci = c->colinf;
	fr = DRAW_FILLEDRECT_PTR(r);	//r->drawers->draw_filledrect;


	cwidth = f->max_cell_width;
	cheight = f->top + f->bottom + 1;
	lines = y2 - y1 + 1;
	c->pattern.wrmode = MD_REPLACE - 1;
	c->pattern.interior = FIS_SOLID;

	if (lines == 1)
	{
		coords[0] = x1 * cwidth;
		coords[1] = y1 * cheight;
		coords[2] = ((x2 + 1) * cwidth) - 1;
		coords[3] = ((y1 + 1) * cheight) - 1;
		(*fr)(r, ci, (VDIRECT *)&coords, (VDIRECT *)&r->x1, &c->pattern);
	}
	else if (lines == 2)
	{
		coords[0] = x1 * cwidth;
		coords[1] = y1 * cheight;
		coords[2] = ((c->la->v_cel_mx + 1) * cwidth) - 1;
		coords[3] = ((y1 + 1) * cheight) - 1;
		(*fr)(r, ci, (VDIRECT *)&coords, (VDIRECT *)&r->x1, &c->pattern);

		coords[0] = 0;
		coords[1] = (y1 + 1) * cheight;
		coords[2] = (x2 * cwidth) - 1;
		coords[3] = ((y2 + 1) * cheight) - 1;
		(*fr)(r, ci, (VDIRECT *)&coords, (VDIRECT *)&r->x1, &c->pattern);
	}
	else
	{
		coords[0] = x1 * cwidth;
		coords[1] = y1 * cheight;
		coords[2] = ((c->la->v_cel_mx + 1) * cwidth) - 1;
		coords[3] = ((y1 + 1) * cheight) - 1;
		(*fr)(r, ci, (VDIRECT *)&coords, (VDIRECT *)&r->x1, &c->pattern);

		coords[0] = 0;
		coords[1] = (y1 + 1) * cheight;
		coords[2] = ((c->la->v_cel_mx + 1) * cwidth) - 1;
		coords[3] = (y2 * cheight) - 1;
		(*fr)(r, ci, (VDIRECT *)&coords, (VDIRECT *)&r->x1, &c->pattern);

		coords[0] = 0;
		coords[1] = y2 * cheight;
		coords[2] = (x2 * cwidth) - 1;
		coords[3] = ((y2 + 1) * cheight) - 1;
		(*fr)(r, ci, (VDIRECT *)&coords, (VDIRECT *)&r->x1, &c->pattern);
	}
}

/* Scroll nlines lines starting at y direction way ... yeah. */
void
scroll_lines( CONSOLE *c, short y, short nlines, short direction)
{
	int cheight, cwidth;
	FONT_HEAD *f;
	RASTER *r;
	short pts[8];
	register short *srcpts, *dstpts;
	MFDB src, dst;

	f = c->f;
	r = c->r;

	cheight = f->top + f->bottom + 1;
	cwidth = f->max_cell_width;

	srcpts = (short *)&pts[0];
	dstpts = (short *)&pts[4];

	srcpts[0] = dstpts[0] = 0;
	srcpts[2] = dstpts[2] = ((c->la->v_cel_mx + 1) * cwidth) - 1;

	if (direction == UP)
	{

		if (y >= c->la->v_cel_my)
			return;

		srcpts[1] = (y + 1) * cheight;
		dstpts[1] = y * cheight;

		srcpts[3] = ((c->la->v_cel_my + 1) * cheight) - 1;
		dstpts[3] = (c->la->v_cel_my * cheight) - 1;
	}
	else /* direction == DOWN */
	{
		if (y >= c->la->v_cel_my)
			return;

		srcpts[1] = y * cheight;
		dstpts[1] = (y + 1) * cheight;

		srcpts[3] = (c->la->v_cel_my * cheight) - 1;
		dstpts[3] = ((c->la->v_cel_my + 1) * cheight) - 1;
	}

	src.fd_addr = dst.fd_addr = 0;
	RO_CPYFM( r, &src, &dst, pts, (VDIRECT *)&r->x1, 3);
}

void
draw_text_cursor(CONSOLE *c)
{
	RASTER *r;
	register int cwidth, cheight;
	short coords[4];

	r = c->r;

	cwidth = c->f->max_cell_width;
	cheight = c->f->top + c->f->bottom + 1;

	switch (c->tc_type)
	{
		case TCT_BOTTOM_SL:
		{	/* horizontal one-pixel line */
			coords[0] = c->la->v_cur_x * cwidth;
			coords[1] = coords[3] = ((c->la->v_cur_y + 1) * cheight) - 1;
			coords[2] = coords[0] + cwidth - 1;
			break;
		}
		case TCT_LEFT_SL:
		{	/* vertical, left oriented, one-pixel line */
			coords[0] = c->la->v_cur_x * cwidth;
			coords[1] = c->la->v_cur_y * cheight;
			coords[2] = coords[0];
			coords[3] = coords[1] + cheight - 1;
			break;
		}
		case TCT_BLOCK:
		default:
		{	/* block */
			coords[0] = c->la->v_cur_x * cwidth;
			coords[1] = c->la->v_cur_y * cheight;
			coords[2] = coords[0] + cwidth - 1;
			coords[3] = coords[1] + cheight - 1;
			break;
		}
	}
	c->pattern.wrmode = MD_XOR - 1;
	c->pattern.interior = FIS_SOLID;
	DRAW_FILLEDRECT( r, c->colinf, (VDIRECT *)&coords, (VDIRECT *)&r->x1, &c->pattern);
}

void
undraw_text_cursor(CONSOLE *c)
{
	(*c->draw_text_cursor)(c);
}

void
draw_character(CONSOLE *c, short chr)
{
	MFDB dst, fontd;
	FONT_HEAD *f;
	RASTER *r;
	register int cwidth, cheight, fc, bc;
	short coords[8];

	f = c->f;
	r = c->r;

	cwidth		= f->max_cell_width;
	cheight		= f->top + f->bottom + 1;
	fontd.fd_addr	= NULL;
	expand_gdf_font( f, &fontd, chr);

	coords[4] = c->la->v_cur_x * cwidth;
	coords[5] = c->la->v_cur_y * cheight;
	coords[6] = coords[4] + (cwidth - 1);
	coords[7] = coords[5] + (cheight - 1);

	coords[0] = coords[1] = 0;
	coords[2] = fontd.fd_w - 1;
	coords[3] = fontd.fd_h - 1;

	dst.fd_addr = NULL;

	if (r->res.planes == 1)
	{
		if (c->la->v_cur_flag & V_INVERSED)
			RO_CPYFM(r, &fontd, &dst, (short *)coords, (VDIRECT *)&r->x1, 12);
		else
			RO_CPYFM(r, &fontd, &dst, (short *)coords, (VDIRECT *)&r->x1, 3);
	}
	else
	{
		if (c->la->v_cur_flag & V_INVERSED)
		{
			fc = c->la->v_col_bg;
			bc = c->la->v_col_fg;
		}
		else
		{
			fc = c->la->v_col_fg;
			bc = c->la->v_col_bg;
		}

		RT_CPYFM( r, c->colinf, &fontd, &dst, (short *)coords, (VDIRECT *)&r->x1, fc, bc, 0);
	}
}
