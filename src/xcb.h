
#ifndef _XCB_H
#define _XCB_H

#include	"res.h"
#include	"libkern.h"

/* alle Zeiger auf Routinen erwarten ihre Parameter im */
/* Turbo-/Pure-C-Format: WORD-Parameter in ihrer Reihenfolge in */
/* den Registern d0-d2, Zeiger in den Registern a0-a1 */
struct xcb
{
	long	version;
	unsigned char	resolution;
	unsigned char	blnk_time;
	unsigned char	ms_speed;
	char		old_res;
	void		(*p_chres)(RESOLUTION *res);
	short		mode;
	short		bypl;
	short		planes;
	short		colors;
	short		hc;
	short		max_x;
	short		max_y;
	short		rmn_x;
	short		rmx_x;
	short		rmn_y;
	short		rmx_y;

	short		v_top;
	short		v_bottom;
	short		v_left;
	short		v_right;

	void		(*p_setcol)(short index, unsigned char *colors);
	void		(*p_chng_vrt)(short x, short y);
	void		(*inst_xbios)(short flag);
	void		(*pic_on)(short flag);
	void		(*chng_pos)(void);
	void		(*p_setscr)(void *adr);
	void		*base;
	void		*scr_base;
	short		scrn_cnt;
	unsigned long	scrn_siz;
	void		*reg_base;
	void		(*p_vsync)(void);
	char		name[36];
	unsigned long	mem_size;

	/* This stuff below here is only there from version 1.20!! */
	char		hw_flags[8];	/* Four card-dependant bytes */
	short		cpu;
	void		(*set_gamma)(unsigned char *gamma);
	short		gamma_able;
	short		gamma_rgb[3];
	unsigned char	*mem_reg;
};
typedef struct xcb XCB;

void	do_p_chres(long routine, RESOLUTION *res, long scrofst);
void	do_p_setscr(long routine, void *scrnadr);
void	do_p_setcol(long routine, short pen, unsigned char *colors);
void	do_p_vsync(long routine);
void	do_p_chng_vrt(long routine, short x, short y);

#endif	/* _ICB_H */
