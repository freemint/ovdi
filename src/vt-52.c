
#include "console.h"
#include "draw.h"
#include "display.h"
#include "gdf_text.h"
#include "libkern.h"
#include "ovdi_defs.h"
#include "ovdi_types.h"
#include "rasters.h"
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

EscFunc ctrl_codes[] =
{
	bell,
	Esc_D, /*backspace*/
	tab,
	LineFeed, LineFeed, LineFeed,
	CarrigeReturn,
};

EscFunc	UC_escapes[] =
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

EscFunc LC_escapes[] =
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

char UC_escapeflags[] =
{
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 0
};
char LC_escapeflags[] =
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

CONSOLE console;

/* init console structure. All console functions will reference the
 * VIRTUAL structure passed here.
*/
void
init_console(VIRTUAL *v, LINEA_VARTAB *la)
{
	CONSOLE *c = &console;
	FONT_HEAD *f;

	bzero(&console, sizeof(CONSOLE));

	f = sysfnt10p;
	c->f = f;
	c->v = v;
	c->la = la;

	c->col_vdi2hw = v->color_vdi2hw; //VDI2HW_colorindex;
	c->col_hw2vdi = v->color_hw2vdi; //HW2VDI_colorindex;

	c->pattern.expanded = 0;
	c->pattern.color[0] = c->pattern.color[1] = c->col_vdi2hw[0];
	c->pattern.color[2] = c->pattern.color[3] = v->raster->planes > 8 ? 0 : 0xff;
	c->pattern.bgcol[0] = c->pattern.bgcol[1] = c->col_vdi2hw[1];
	c->pattern.bgcol[2] = c->pattern.bgcol[3] = v->raster->planes > 8 ? 0xff : 0x0;

	c->pattern.width = 16;
	c->pattern.height = 1;
	c->pattern.wwidth = 1;
	c->pattern.planes = 1;
	c->pattern.wrmode = 2;
	c->pattern.mask = 0xffff;
	c->pattern.data = &consfill;

	c->curs_hide_ct = 1;
	c->tps = (*v->driver->get_vbitics)();

	/* console driver */
	c->enter_console = &console_enter;
	c->exit_console = &console_exit;
	c->draw_text_cursor = &draw_text_cursor;
	c->undraw_text_cursor = &undraw_text_cursor;
	c->textcursor_blink = &do_txtcurs_blnk;
	c->scroll_lines = &scroll_lines;
	c->erase_lines = &erase_lines;
	c->draw_character = &draw_character;
	c->csout_char = &cs_output_character;
	

	/* Initialize the Line A variables used by the console/vt52 emulator */
	la->v_cur_x = la->v_cur_y = la->v_sav_x = la->v_sav_y = 0;
	la->v_cel_ht = f->top + f->bottom + 1;
	la->v_cel_wr = 0;	/* Not used!!! */
	la->v_cel_mx = (v->raster->w / f->max_cell_width) - 1;
	la->v_cel_my = (v->raster->h / (f->top + f->bottom + 1)) - 1;
	la->v_cur_flag = 0;
	la->v_col_fg = c->col_vdi2hw[1];
	la->v_col_bg = c->col_vdi2hw[0];

	la->v_cur_ad = (unsigned char *)v->raster->base;
	la->v_cur_of = 0;

	la->v_period = 1 * 66;
	la->v_cur_ct = 1 * 66;

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
	la->textfg = c->col_vdi2hw[1];
	la->textbg = c->col_vdi2hw[0];

	v->con = c;
	return;
}
void
install_console_handlers(CONSOLE *c)
{
	short	sr;

	set_constate(c, (long)&con_state_handler);
	set_xconout_raw(c, (long)&rawcon_output);

	sr = spl7();
	old_con_state	= con_state;
	con_state = (long)&do_con_state;

	old_xconout_console = xconout_console;
	xconout_console = (long)&do_xconout_console;

	old_xconout_raw = xconout_raw;
	xconout_raw = (long)&do_xconout_raw;
	spl(sr);
	return;
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
	short oldrate;

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
	register short column, columns;

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
	register short row = c->la->v_cur_y + 1;

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
	register short column = c->la->v_cur_x + 1;
	register short row = c->la->v_cur_y;
	register short chr = character & 0xff;

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
	register short chr;
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
	register short row;

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
	register short row = c->la->v_cur_y + 1;

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
	register short column = c->la->v_cur_x + 1;

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
	register short column = c->la->v_cur_x - 1;

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
	register short row = c->la->v_cur_y - 1;

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
	c->la->v_col_fg = c->col_vdi2hw[color & 0xf];
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
	c->la->v_col_bg = c->col_vdi2hw[color & 0xf];
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
	register short mx, my;

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
	return;
}

void
console_exit(CONSOLE *c)
{
	/* Nothing to do. This is where drivers go back to the normal
	*  mode, which will show the AES, for example.
	*/
	return;
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
	VIRTUAL *v;
	short cwidth, cheight, lines;
	short coords[4];

	f = c->f;
	v = c->v;

	cwidth = f->max_cell_width;
	cheight = f->top + f->bottom + 1;
	lines = y2 - y1 + 1;
	c->pattern.wrmode = MD_REPLACE - 1;

	if (lines == 1)
	{
		coords[0] = x1 * cwidth;
		coords[1] = y1 * cheight;
		coords[2] = ((x2 + 1) * cwidth) - 1;
		coords[3] = ((y1 + 1) * cheight) - 1;
		rectfill(v, (VDIRECT *)&coords, &c->pattern);
	}
	else if (lines == 2)
	{
		coords[0] = x1 * cwidth;
		coords[1] = y1 * cheight;
		coords[2] = ((c->la->v_cel_mx + 1) * cwidth) - 1;
		coords[3] = ((y1 + 1) * cheight) - 1;
		rectfill(v, (VDIRECT *)&coords, &c->pattern);

		coords[0] = 0;
		coords[1] = (y1 + 1) * cheight;
		coords[2] = (x2 * cwidth) - 1;
		coords[3] = ((y2 + 1) * cheight) - 1;
		rectfill(v, (VDIRECT *)&coords, &c->pattern);
	}
	else
	{

		coords[0] = x1 * cwidth;
		coords[1] = y1 * cheight;
		coords[2] = ((c->la->v_cel_mx + 1) * cwidth) - 1;
		coords[3] = ((y1 + 1) * cheight) - 1;
		rectfill(v, (VDIRECT *)&coords, &c->pattern);

		coords[0] = 0;
		coords[1] = (y1 + 1) * cheight;
		coords[2] = ((c->la->v_cel_mx + 1) * cwidth) - 1;
		coords[3] = (y2 * cheight) - 1;
		rectfill(v, (VDIRECT *)&coords, &c->pattern);

		coords[0] = 0;
		coords[1] = y2 * cheight;
		coords[2] = (x2 * cwidth) - 1;
		coords[3] = ((y2 + 1) * cheight) - 1;
		rectfill(v, (VDIRECT *)&coords, &c->pattern);
	}
	return;
}

/* Scroll nlines lines starting at y direction way ... yeah. */
void
scroll_lines( CONSOLE *c, short y, short nlines, short direction)
{
	short cheight, cwidth;
	FONT_HEAD *f;
	short pts[8];
	register short *srcpts, *dstpts;
	MFDB src, dst;

	f = c->f;

	cheight = f->top + f->bottom + 1;
	cwidth = f->max_cell_width;

	srcpts = (short *)&pts[0];
	dstpts = (short *)&pts[4];

	srcpts[0] = dstpts[0] = 0;
	srcpts[2] = dstpts[2] = (c->la->v_cel_mx * cwidth) - 1;

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
	ro_cpyfm(c->v, &src, &dst, pts, 3);
	return;
}

void
draw_text_cursor(CONSOLE *c)
{

	register short cwidth, cheight;
	short coords[4];

	cwidth = c->f->max_cell_width;
	cheight = c->f->top + c->f->bottom + 1;

	coords[0] = c->la->v_cur_x * cwidth;
	coords[1] = c->la->v_cur_y * cheight;
	coords[2] = coords[0] + cwidth - 1;
	coords[3] = coords[1] + cheight - 1;

	c->pattern.wrmode = MD_XOR - 1;
	rectfill( c->v, (VDIRECT *)&coords[0], &c->pattern);

	return;
}

void
undraw_text_cursor(CONSOLE *c)
{
	(*c->draw_text_cursor)(c);
	return;
}

void
draw_character(CONSOLE *c, short chr)
{
	MFDB dst, fontd;
	FONT_HEAD *f;
	VIRTUAL *v;
	register short cwidth, cheight, fc, bc;
	VDIRECT sclip;
	short coords[8];

	f = c->f;
	v = c->v;

	cwidth = f->max_cell_width;
	cheight = f->top + f->bottom + 1;
	expand_gdf_font( f, &fontd, chr, (long)0);

	coords[4] = c->la->v_cur_x * cwidth;
	coords[5] = c->la->v_cur_y * cheight;
	coords[6] = coords[4] + (cwidth - 1);
	coords[7] = coords[5] + (cheight - 1);

	coords[0] = coords[1] = 0;
	coords[2] = fontd.fd_w - 1;
	coords[3] = fontd.fd_h - 1;

	dst.fd_addr = 0;

	cwidth = v->clip_flag;
	sclip = v->clip;
	v->clip.x1 = v->clip.y1 = 0;
	v->clip.x2 = v->raster->w - 1;
	v->clip.y2 = v->raster->h - 1;

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


	rt_cpyfm( v, &fontd, &dst, (short *)coords, fc, bc, 0);

	v->clip_flag = cwidth;
	v->clip = sclip;

	return;
}
