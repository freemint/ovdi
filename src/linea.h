#ifndef	_OVDILINEA_H
#define _OVDILINEA_H

typedef struct linea_vartab LINEA_VARTAB;

#include "mouse.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

/* These are bits in reservd7 used by the console
*  driver. 
*/
#define	V_INVERSED	(1 << 4)
#define V_LINEWRAP	(1 << 3)
#define V_CURSDRAWN	1
#define V_CURSON	(1 << 7)
struct linea_vartab
{
	long		reservd0;
	FONT_HEAD	*cur_font;
	char		reservd1[46];

	short		m_pos_hx;
	short		m_pos_hy;
	short		m_planes;
	short		m_cdb_bg;
	short		m_cdb_fg;
	short		mask_form[32];

	INQ_TAB	 	inq;
	DEV_TAB	 	dev;

	short		gcurx;
	short		gcury;
	short		m_hid_ct;
	short		mouse_bt;
	short		req_col[16 * 3];
	SIZ_TAB		siz;		/* Siztab is 12 words long */
	short		unused[3];
	short		reservd2;	/* v_planes -- NOVA */
	short		reservd3;	/* Input Mode choice -- NOVA */
	VIRTUAL		*cur_work;
	FONT_HEAD	*def_font;
	FONT_HEAD	*font_ring[4];
	short		font_count;

	short		reservd4_0;
	short		mod_loc;	/* Input mode locator - nOVA */
	short		reservd4_1[41];
	short		mod_str;	/* Input mode string - NOVA */
	short		mod_val;	/* Input mode valuator - NOVA */

	char		cur_ms_stat;
	char		reservd5;
	short		v_hid_cnt;
	short		cur_x;
	short		cur_y;
	char		cur_flag;
	char		mouse_flag;
	long		reservd6;
	short		v_sav_x;
	short		v_sav_y;
	short		sav_len;
	unsigned char	*save_addr;
	short		save_stat;
	char		save_area[256];
	void		(*user_tim)(void);
	void		(*next_tim)(void);
	void		(*user_but)(void);
	void		(*user_cur)(void);
	void		(*user_mot)(void);

	short		v_cel_ht;
	short		v_cel_mx;
	short		v_cel_my;
	short		v_cel_wr;

	short		v_col_bg;
	short		v_col_fg;

	unsigned char	*v_cur_ad;
	short		v_cur_of;
	short		v_cur_x;
	short		v_cur_y;
	char		v_period;
	char		v_cur_ct;

	unsigned short	*v_fnt_ad;
	short		v_fnt_nd;
	short		v_fnt_st;
	short		v_fnt_wd;

	short		v_rez_hz;
	unsigned short 	*v_off_ad;
	short		v_cur_flag; /* reservd7; */

	short		v_rez_vt;
	short		bytes_lin;

/*O*/	short		planes;		/* Origin of Line A variable table */

	short		width;

	short		*contrl;
	short		*intin;
	short		*ptsin;
	short		*intout;
	short		*ptsout;

	short		colbit0;
	short		colbit1;
	short		colbit2;
	short		colbit3;

	short		lstlin;
	unsigned short	lnmask;
	short		wrmode;

	short		x1;
	short		y1;
	short		x2;
	short		y2;

	void		*patptr;
	short		patmsk;
	short		mfill;

	short		clip;
	short		xmincl;
	short		ymincl;
	short		xmaxcl;
	short		ymaxcl;

	short		xdda;
	short		ddainc;
	short		scaldir;
	short		mono;

	short		sourcex;
	short		sourcey;
	short		destx;
	short		desty;
	short		delx;
	short		dely;

	void		*fbase;
	short		fwidth;
	short		style;
	short		litemask;
	short		weight;
	short		roff;
	short		loff;
	short		scale;
	short		chup;
	short		textfg;
	void		*scrtchp;
	unsigned short	scrpt2;
	short		textbg;
	short		copytran;
	void		*seedabort;
};

struct bitblt
{
	short		width;
	short		height;
	short		planes;
	short		fg_col;
	short		bg_col;
	long		op_tab;

	short		s_x;
	short		s_y;
	unsigned char	*s_form;
	short		s_nxwd;
	short		s_nxln;
	short		s_nxpl;

	short		d_x;
	short		d_y;
	unsigned char	*d_form;
	short		d_nxwd;
	short		d_nxln;
	short		d_nxpl;

	unsigned char	*p_addr;
	short		p_nxln;
	short		p_nxpl;
	short		p_mask;
	unsigned char	space[24];
};
typedef struct bitblt BITBLT;

void init_linea_vartab(VIRTUAL *v, LINEA_VARTAB *la);
void set_linea_vector(void);
void get_linea_addresses(long *vt, long *fr, long *ft);

#endif	/* _OVDILINEA_H */
