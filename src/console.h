#ifndef	_CONSOLE_H
#define _CONSOLE_H

#include "linea_vars.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

#define BLINK_ON	1

/* Cursor types definitions */
#define TCT_BLOCK		0	/* Standard block-type cursor */
#define TCT_LEFT_SL	1	/* Left-oriented Vertical Single-Line */
#define TCT_BOTTOM_SL 	2	/* Bottom-oriented single-line horizontal cursor */

struct	console
{
	FONT_HEAD 		*f;
	FONT_HEAD		*loaded_font;
	struct ovdi_driver	*drv;
	struct raster		*r;
	struct colinf		*colinf;
	struct linea_vartab	*la;
	
	short		tc_flags;	/* text cursor flags */
	short		tc_type;

	short		tps;
	short		curs_hide_ct;
	short		blinkrate;
	short		nxt_blink;
	short		save_row;
	PatAttr		pattern;
	
	/* Device API */
	void (*enter_console)(struct console *c);
	void (*exit_console)(struct console *c);
	void (*draw_text_cursor)(struct console *c);
	void (*undraw_text_cursor)(struct console *c);
	void (*textcursor_blink)(void);
	void (*scroll_lines)(struct console *c, short y, short nlines, short direction);
	void (*erase_lines)(struct console *c, short x1, short y1, short x2, short y2);
	void (*draw_character)(struct console *c, short character);

	void (*csout_char)(short character);

	const char *fontpath;
	char fontfile[32];

	struct pattern_data pd;
};
typedef struct console CONSOLE;

typedef void (*EscFunc)(CONSOLE *c);

CONSOLE * init_console(OVDI_HWAPI *hw, RASTER *r, LINEA_VARTAB *la);
void	install_console_handlers(CONSOLE *c);
void	change_console_resolution(CONSOLE *c, struct raster *r);
void	set_console_font(CONSOLE *c, const char *fontpath, char *fontfile);
void	enter_console(CONSOLE *c);
void	exit_console(CONSOLE *c);
short	conf_textcursor_blink(CONSOLE *c, short mode, short rate);
void	con_state_handler(CONSOLE *c, short character);
void	rawcon_output(CONSOLE *c, short chr);
void	VT52_handler(CONSOLE *c, short character);

void Esc_nosys		(CONSOLE *c);
void Esc_A		(CONSOLE *c);
void Esc_B		(CONSOLE *c);
void Esc_C		(CONSOLE *c);
void Esc_D		(CONSOLE *c);
void Esc_E		(CONSOLE *c);
void Esc_H		(CONSOLE *c);
void Esc_I		(CONSOLE *c);
void Esc_J		(CONSOLE *c);
void Esc_K		(CONSOLE *c);
void Esc_L		(CONSOLE *c);
void Esc_M		(CONSOLE *c);
void Esc_Y		(CONSOLE *c);
void Esc_Y_save_row	(CONSOLE *c, short character);
void Esc_Y_save_column	(CONSOLE *c, short character);
void Esc_b		(CONSOLE *c);
void Esc_b_collect	(CONSOLE *c, short character);
void Esc_c		(CONSOLE *c);
void Esc_c_collect	(CONSOLE *c, short character);
void Esc_d		(CONSOLE *c);
void Esc_e		(CONSOLE *c);
void Esc_f		(CONSOLE *c);
void Esc_j		(CONSOLE *c);
void Esc_k		(CONSOLE *c);
void Esc_l		(CONSOLE *c);
void Esc_o		(CONSOLE *c);
void Esc_p		(CONSOLE *c);
void Esc_q		(CONSOLE *c);
void Esc_v		(CONSOLE *c);
void Esc_w		(CONSOLE *c);

void bell		(CONSOLE *c);
void tab		(CONSOLE *c);
void LineFeed		(CONSOLE *c);
void CarrigeReturn	(CONSOLE *c);

/* device driver routines */
void console_enter	(CONSOLE *c);
void console_exit	(CONSOLE *c);
void text_cursor_on	(CONSOLE *c);
void text_cursor_off	(CONSOLE *c);
void reset_text_cursor	(CONSOLE *c);
void move_text_cursor	(CONSOLE *c, short x, short y);
void show_text_cursor	(CONSOLE *c);
void hide_text_cursor	(CONSOLE *c);
void draw_text_cursor	(CONSOLE *c);
void undraw_text_cursor	(CONSOLE *c);
void textcursor_blink	(CONSOLE *c);


void erase_lines	(CONSOLE *c, short x1, short y1, short x2, short y2);
void scroll_lines	(CONSOLE *c, short y, short nlines, short direction);
void draw_character	(CONSOLE *c, short character);

/* asm wrappers */
extern long	old_con_state;
extern long	old_xconout_console;
extern long	old_xconout_raw;

void	cs_output_character(short character);
void	do_con_state(void);
void	do_xconout_console(void);
void	do_xconout_raw(void);
void	do_txtcurs_blnk(void);

void	set_constate(CONSOLE *c, long routine);		/* in contermh.s */
void	set_xconout_raw(CONSOLE *c, long routine);	/* in contermh.s */
void	call_bellhook(void);				/* in contermh.s */

#endif	/* _CONSOLE_H */
