/*
	TheMouseDriver:

 I imagine a 3 layer design, where layer 1 is the 'main' driver layer,
 layer 2 is the device driver and layer 3 is the mouse-rendering part.

 Layer 1 basically takes input from layer 2, when mouse moves, buttons
are pressed, etc., makes the necessary housekeeping. Then layer 3 is
used to render the mouse cursor/graphics onscreen.

 With this design, one only needs to write a new mouse device driver
(layer 2) to support new pointing equpment. And only new mouse-graphics
(layer 3) is needed to support new graphics cards.

 VDI uses Layer 1 to get/set mouse-related parameters, like cursor
 shape, get mouse-status, etc.

 This file contains Layer 1 of the mouse driver!

*/

#include "display.h"
#include "linea.h"
#include "mouse.h"
#include "mousedrv.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "ovdi_types.h"
#include "../../sys/mint/arch/asm_spl.h"

void set_xmf_res(VIRTUAL *v);
void set_new_mform(VIRTUAL *v, MFORM *mf);

/* ASM wrappers - just because I cannot get gcc to save d0-d7/a0-a6 instead of d2-d7/a2-a6 */
extern void m_abs_move(short x, short y);
extern void m_rel_move(short x, short y);
extern void m_but_chg(unsigned short bs);
extern void m_int(void);

void mouse_relative_move(register short x, register short y) __attribute__ (());
void mouse_absolute_move(register short x, register short y);
void mouse_buttons_change(register unsigned short bs);

static void scale_mouse(MOUSEDRV *, short, short);
static void check_coords(MOUSEDRV *);

void reset_mouse_curs(void);
void show_mouse_curs(short);
void hide_mouse_curs(void);
void enable_mouse_curs(void);
void disable_mouse_curs(void);
void mouse_interrupt(void);

unsigned long get_button_status(void);
void get_mouse_coordinates(short *xy);
unsigned long set_mouse_vector(short vecnum, unsigned long vector);
void mdonothing(void);

MOUSEAPI	mapi;
MOUSEDRV	md;

//XMFORM		*current_xmf;
XMFORM		std_xmf;
XMSAVE		std_xms;
unsigned char	mfdata[16*16*32];
unsigned short	mfmask[16];
unsigned char	msbuff[16*16*32];

short arrow_cdb[] = 
{
	1,0,1,0,1,

	0xC000, //%1100000000000000
	0xE000, //%1110000000000000       // background definition
	0xF000, //%1111000000000000
	0xF800, //%1111100000000000

	0xFC00, //%1111110000000000
	0xFE00, //%1111111000000000
	0xFF00, //%1111111100000000
	0xFF80, //%1111111110000000

	0xFFc0, //%1111111111000000
	0xFFE0, //%1111111111100000
	0xFE00, //%1111111000000000
	0xEF00, //%1110111100000000

	0xCF00, //%1100111100000000
	0x8780, //%1000011110000000
	0x0780, //%0000011110000000
	0x0380, //%0000001110000000
/*----*/
	0x0000, //%0000000000000000
	0x4000, //%0100000000000000       // foreground definition
	0x6000, //%0110000000000000
	0x7000, //%0111000000000000

	0x7800, //%0111100000000000
	0x7C00, //%0111110000000000
	0x7E00, //%0111111000000000
	0x7F00, //%0111111100000000

	0x7F80, //%0111111110000000
	0x7C00, //%0111110000000000
	0x6C00, //%0110110000000000
	0x4600, //%0100011000000000

	0x0600, //%0000011000000000
	0x0300, //%0000001100000000
	0x0300, //%0000001100000000
	0x0000 //%0000000000000000
};

/* At the time when this function is called, the linea variable table is not
*  not completely setup! So, do not rely on anything in it, and only setup
*  own things.
*/
MOUSEAPI *
init_mouse(VIRTUAL *v, LINEA_VARTAB *la)
{
	XMFORM *xmf;
	XMSAVE *xms;
	MOUSEDRV *m = &md;
	MOUSEAPI *ma = &mapi;

 /* Set up the api interface - this is what the VDI uses */
	ma->setxmfres = set_xmf_res;
	ma->setnewmform = set_new_mform;
	ma->resetmcurs = reset_mouse_curs;
	ma->enablemcurs = enable_mouse_curs;
	ma->disablemcurs = disable_mouse_curs;
	ma->showmcurs = show_mouse_curs;
	ma->hidemcurs = hide_mouse_curs;
	ma->relmovmcurs = m_rel_move;		//mouse_relative_move;
	ma->absmovmcurs = m_abs_move;		//mouse_absolute_move;
	ma->butchg = m_but_chg;			//mouse_buttons_change;
	ma->getbutstat = get_button_status;
	ma->getmcoords = get_mouse_coordinates;
	ma->setvector = set_mouse_vector;
	ma->housekeep = m_int;			//mouse_interrupt;

	xmf = &std_xmf;
	xms = &std_xms;

 /* Setup the structures containing mouse graphics */
	xmf->save = &std_xms;
	xmf->mask = mfmask;
	xmf->data = mfdata;
	xms->save = msbuff;

	m->current_xmf = xmf;
	m->current_xms = xms;

	set_xmf_res(v);
	set_new_mform(v, (MFORM *)&arrow_cdb);

 /* setup stuff in the Line A variable table */
	la->user_but = mdonothing;
	la->user_mot = mdonothing;
	la->user_cur = mdonothing;

	la->mouse_flag = 0;
	la->cur_flag = 0;
	la->cur_ms_stat = 0;
	la->m_hid_ct = 1;
	la->mouse_bt = 0;
	la->gcurx = v->raster->w >> 1;
	la->gcury = v->raster->h >> 1;

	m->la = la;
	m->hide_ct = 1;
	m->min_x = m->min_y = 0;
	m->max_x = v->raster->w - 1;
	m->max_y = v->raster->h - 1;
	m->current_x = la->gcurx;
	m->current_y = la->gcury;
	m->current_bs = 0;
	m->changed_bs = 0;
	m->last_bs = 0;

	reset_mouse_curs();

	/* At this point we should look for Layer 2 and 3 drivers	*/
	/* For now we use ikbd_mouse.c for layer 2 and the		*/
	/* draw_mousecurs_Xb()/restore_msave_Xb in the Xb_driver.c	*/
	/* files, where X is 8 for 8bps, 16 for 16bps, etc. We find the */
	/* address of these functions in ovdi's	driver structure	*/

	init_mouse_device(ma);

	m->bs_mask = ~(0xffff << ma->buttons);

	log("bs_mask = %x\n", m->bs_mask);
	
	return ma;
}

/* Let the mouse driver know about resolution changes */
void
set_xmf_res(VIRTUAL *v)
{
	register MOUSEDRV *m = &md;
	register XMFORM *xmf = m->current_xmf;
	register OVDI_DRIVER *d = v->driver;

	xmf->mx		= d->r.w - 1;
	xmf->my		= d->r.h - 1;
	xmf->bypl	= d->r.bypl;
	xmf->scr_base	= d->r.base;

	m->draw_mcurs	= d->f.draw_mc;
	m->undraw_mcurs	= d->f.undraw_mc;
	m->vreschk	= d->dev->vreschk;

	return;
}

/* Grab data from a standard mform structure, as used by */
/* current versions of the different VDI/AES's		 */
void
set_new_mform(VIRTUAL *v, MFORM *mf)
{
	int i;
	register unsigned short *dest, *data, *mask;
	register MOUSEDRV *m = &md;
	register XMFORM *xmf = m->current_xmf;

	m->flags	&= ~MC_ENABLED;

	xmf->xhot	= mf->xhot;
	xmf->yhot	= mf->yhot;
	xmf->planes	= mf->planes;
	xmf->mfbypl	= 2;
	xmf->width	= 16;
	xmf->height	= 16;
	xmf->fg_col	= v->color_vdi2hw[mf->fg_col];
	xmf->bg_col	= v->color_vdi2hw[mf->bg_col];
	xmf->fg_pix	= v->raster->pixelvalues[xmf->fg_col];
	xmf->bg_pix	= v->raster->pixelvalues[xmf->bg_col];

	dest = (unsigned short *)xmf->data;
	data = (unsigned short *)&mf->data;
	mask = (unsigned short *)&mf->mask;

	for (i = 0; i < 16; i++)
	{
		*dest++ = mask[i];
		*dest++ = data[i];
	}

	m->flags	|= MC_ENABLED;
	return;
}

/* These are the functions called by mouse device drivers	*/
/* (layer 2) to let the high-level know about mouse activity	*/
void
mouse_relative_move(register short x, register short y)
{
	register MOUSEDRV *m = &md;
	register LINEA_VARTAB *la = m->la;
	register short nx __asm__ ("d0") = x;
	register short ny __asm__ ("d1") = y;

	nx += m->current_x;
	ny += m->current_y;
	scale_mouse(m, nx, ny);
	check_coords(m);
	nx = m->current_x;
	ny = m->current_y;

	/* Call user_mot function */
	__asm__ volatile
	("
		move.l	%2,a0
		jsr	(a0)
	"	:
		: "d"(nx),"d"(ny),"a"(la->user_mot)
		: "a0"
	);

	m->current_x = nx;
	m->current_y = ny;

	la->gcurx = nx;
	la->gcury = ny;

	__asm__ volatile
	("
		move.l	%2,a0
		jsr	(a0)
	"	:
		: "d"(nx),"d"(ny),"a"(la->user_cur)
		: "a0"
	);

	m->flags |= MC_MOVED;

	return;
}

void
mouse_absolute_move(register short x, register short y)
{
	register MOUSEDRV *m = &md;
	register LINEA_VARTAB *la = m->la;
	register short nx __asm__ ("d0") = x;
	register short ny __asm__ ("d1") = y;

	scale_mouse(m, nx, ny);
	check_coords(m);
	nx = m->current_x;
	ny = m->current_y;

	/* Call user_mot function */
	__asm__ volatile
	("
		move.l	%2,a0
		jsr	(a0)
	"	:
		: "d"(nx),"d"(ny),"a"(la->user_mot)
		: "a0"
	);

	m->current_x = nx;
	m->current_y = ny;

	la->gcurx = nx;
	la->gcury = ny;

	__asm__ volatile
	("
		move.l	%2,a0
		jsr	(a0)
	"	:
		: "d"(x),"d"(y),"a"(la->user_cur)
		: "a0"
	);

	m->flags |= MC_MOVED;

	return;
}

void
mouse_buttons_change(register unsigned short bs)
{
	register MOUSEDRV *m = &md;
	register short nbs __asm__ ("d0") = bs;
	register short obs = m->current_bs;


	if (nbs == obs)
		return;

	__asm__ volatile
	("
		move.l	%1,a0
		jsr	(a0)
	"	:
		: "d"(nbs),"a"(m->la->user_but)
		: "d0","d1","a0"
	);

	
	m->current_bs = nbs;
	m->changed_bs ^= nbs;
	m->last_bs = m->changed_bs;

	return;
}

static void
check_coords(MOUSEDRV *m)
{
	if (m->current_x < m->min_x)
		m->current_x = m->min_x;
	else if (m->current_x > m->max_x)
		m->current_x = m->max_x;

	if (m->current_y < m->min_y)
		m->current_y = m->min_y;
	else if (m->current_y > m->max_y)
		m->current_y = m->max_y;
	return;
}
	
static short speed_tab[] =
{ 0, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 14, 255, 255 };

static void
scale_mouse(MOUSEDRV *m, short x, short y)
{
	short nx, ny, x_off, y_off, idx;
	short *st;

	nx = x - m->current_x;
	if (nx < 0)
		x_off = -nx;
	else
		x_off = nx;

	ny = y - m->current_y;
	if (ny < 0)
		y_off = -ny;
	else
		y_off = ny;

	idx = x_off > y_off ? x_off : y_off;

	x_off = 0;
	st = (short *)&speed_tab;
	while (idx > *st++)
		x_off++;

	//nx *= 3;
	nx *= x_off;
	//ny *= 3;
	ny *= x_off;

	m->current_x += nx;
	m->current_y += ny;

	return;
}

void
mouse_interrupt()
{
	register MOUSEDRV *m = &md;
	register long flags = m->flags;

	if (flags & MC_ENABLED)
	{
		if (flags & MC_MOVED)
		{
			if (!m->hide_ct)
			{
				(*m->undraw_mcurs)(m->current_xms);
				(*m->draw_mcurs)(m->current_xmf, m->current_x, m->current_y);
			}

			if (m->vreschk)
				(*m->vreschk)(m->current_x, m->current_y);

			flags &= ~MC_MOVED;
		}
		m->flags = flags;
	}
	return;
}

void
reset_mouse_curs()
{
	enable_mouse_curs();
	return;
}

void
enable_mouse_curs()
{
	register MOUSEDRV *m = &md;

	m->flags |= MC_ENABLED;
	show_mouse_curs(1);
	return;
}

void
disable_mouse_curs(void)
{
	register MOUSEDRV *m = &md;

	hide_mouse_curs();
	m->flags &= ~MC_ENABLED;
	return;
}
	
void
show_mouse_curs(short reset)
{
	register MOUSEDRV *m = &md;
	register long flags = m->flags;

	if (flags & MC_ENABLED)
	{
		m->flags &= ~MC_ENABLED;
		if (reset)
			m->hide_ct = 1;

		if (m->hide_ct == 1)
		{
			(*m->draw_mcurs)(m->current_xmf, m->current_x, m->current_y);
		}
		m->hide_ct -= 1;
		m->flags = flags;
	}
	return;
}

void
hide_mouse_curs(void)
{
	register MOUSEDRV *m = &md;
	register long flags = m->flags;

	if (flags & MC_ENABLED)
	{
		m->flags &= ~MC_ENABLED;
		if (!m->hide_ct)
			(*m->undraw_mcurs)(m->current_xms);

		m->hide_ct++;
		m->flags = flags;
	}
	return;
}

unsigned long
get_button_status(void)
{
	MOUSEDRV *m = &md;
	unsigned long bs;

	bs = (unsigned long)m->last_bs << 16;
	m->last_bs = 0;
	bs |= (unsigned short)m->current_bs;
	return bs;
}


void
get_mouse_coordinates(short *xy)
{
	MOUSEDRV *m = &md;

	*xy++	= m->current_x;
	*xy	= m->current_y;

	return;
}

unsigned long
set_mouse_vector(short vecnum, unsigned long vector)
{
	register MOUSEDRV *m = &md;
	unsigned long oldvec = 0;

	switch (vecnum)
	{
		case MVEC_BUT:
		{
			oldvec = (unsigned long)m->la->user_but;
			(unsigned long)m->la->user_but = vector;
			break;
		}
		case MVEC_CUR:
		{
			oldvec = (unsigned long)m->la->user_cur;
			(unsigned long)m->la->user_cur = vector;
			break;
		}
		case MVEC_MOV:
		{
			oldvec = (unsigned long)m->la->user_mot;
			(unsigned long)m->la->user_mot = vector;
			break;
		}
	}
	return oldvec;
}

			

void
mdonothing()
{
	return;
}
