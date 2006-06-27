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

#include "mousedrv.h"

#include "display.h"
#include "linea_vars.h"
#include "mouse.h"
#include "ovdi_mouse.h"
#include "mouseapi.h"
#include "pdvapi.h"
#include "ovdi_defs.h"
#include "ovdi_types.h"
#include "../../sys/mint/arch/asm_spl.h"


static void set_xmf_res(RASTER *r, COLINF *c);
static void set_new_mform(MFORM *mf);
static void set_xmf_color(XMFORM *xmf);

/* ASM wrappers - just because I cannot get gcc to save d0-d7/a0-a6 instead of d2-d7/a2-a6 */
extern void m_abs_move(short x, short y);
extern void m_rel_move(short x, short y);
extern void m_but_chg(unsigned short bs);
extern void m_int(void);

static void enable_mouse(void);
static void disable_mouse(void);

void _cdecl mouse_relative_move( short x, short y) __attribute__ (());
void _cdecl mouse_absolute_move(short x, short y);
void _cdecl mouse_buttons_change(unsigned short bs);

static void scale_mouse(MOUSEDRV *, short, short);
static void check_coords(MOUSEDRV *);

static void reset_mouse_curs(void);
static void show_mouse_curs(short);
static void hide_mouse_curs(void);
static void enable_mouse_curs(void);
static void disable_mouse_curs(void);
void mouse_interrupt(void);

static unsigned long get_button_status(void);
static void get_mouse_coordinates(short *xy);
static unsigned long set_mouse_vector(short vecnum, unsigned long vector);
static void mdonothing(void);

static MOUSEAPI	mapi;
static MOUSEDRV	md;

//XMFORM		*current_xmf;
static XMFORM	std_xmf;
static XMSAVE	std_xms;
static unsigned char	mfdata[16*16*32];
static unsigned short	mfmask[16];
static unsigned char	msbuff[16*16*32];

static short arrow_cdb[] = 
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
init_mouse(OVDI_HWAPI *hw, LINEA_VARTAB *la)
{
	XMFORM *xmf;
	XMSAVE *xms;
	MOUSEDRV *m = &md;
	MOUSEAPI *ma = &mapi;

/* Set up the api interface - this is what the VDI uses to communicate
 * with the mouse driver (layer 1)
*/
	ma->enable		= enable_mouse;
	ma->disable		= disable_mouse;
	ma->setxmfres		= set_xmf_res;
	ma->setnewmform		= set_new_mform;
	ma->resetmcurs		= reset_mouse_curs;
	ma->enablemcurs		= enable_mouse_curs;
	ma->disablemcurs	= disable_mouse_curs;
	ma->showmcurs		= show_mouse_curs;
	ma->hidemcurs		= hide_mouse_curs;
	ma->relmovmcurs		= m_rel_move;		//mouse_relative_move;
	ma->absmovmcurs		= m_abs_move;		//mouse_absolute_move;
	ma->butchg		= m_but_chg;			//mouse_buttons_change;
	ma->getbutstat		= get_button_status;
	ma->getmcoords		= get_mouse_coordinates;
	ma->setvector		= set_mouse_vector;
	ma->housekeep		= m_int;			//mouse_interrupt;

 /* Setup the structures containing mouse graphics */
	xmf = &std_xmf;
	xms = &std_xms;

	xmf->save = xms; //&std_xms;
	xmf->mask = mfmask;
	xmf->data = mfdata;
	xms->save = msbuff;

/* Setup the mousedrv structure - this structure is shared by the main mouse driver (layer 1),
 * the pointing device driver (layer 2), and the 'mouse rendering' driver (layer 3).
*/
	m->la = la;
	m->raster = NULL;
	m->colinf = NULL;
	m->draw_mcurs = (DRAW_MC *)mdonothing; //(draw_mc)mdonothing;
	m->undraw_mcurs = (UNDRAW_MC *)mdonothing; //(undraw_mc)mdonothing;

	m->flags	= 0;
	m->interrupt	= 1;
	m->current_xmf	= xmf;
	m->current_xms	= xms;

 /* Set up things related to the pointing device driver */
	m->pdapi	= hw->pointdev;
	m->cb.relcmove	= m_rel_move;
	m->cb.abscmove	= m_abs_move;
	m->cb.buttchng	= m_but_chg;
	m->cb.relwheel	= (MWHEEL *)&mdonothing;
	m->cb.abswheel	= (MWHEEL *)&mdonothing;

#if 0
	m->relmovmcurs	= m_rel_move;		//mouse_relative_move;
	m->absmovmcurs	= m_abs_move;		//mouse_absolute_move;
	m->butchg	= m_but_chg;			//mouse_buttons_change;
#endif

	m->vreschk	= hw->device->vreschk;
	m->msema	= hw->device->msema;

/* Gather necessary info about the graphics, and install the 'mouse rendering' (layer 3)
 * part of the threesome.
*/
	set_new_mform((MFORM *)&arrow_cdb);

 /* setup stuff in the Line A variable table */
	la->user_but = mdonothing;
	la->user_mot = mdonothing;
	la->user_cur = mdonothing;

	la->mouse_flag = 0;
	la->cur_flag = 0;
	la->cur_ms_stat = 0;
	la->m_hid_ct = 1;
	la->mouse_bt = 0;
	la->gcurx = m->current_x;
	la->gcury = m->current_y;

/* The pointing device driver (layer 2) fills in 'buttons', 'wheels',
 * and puts the address of its start/stop functions in the 'start' and 'stop'
 * elements of the mousedrv structure. Then it saves the address of it and
 * uses the 'relmovmcurs', 'absmovmcurs' and 'butchg' functions to report
 * to layer 1 any movements/button changes
*/
	(*m->pdapi->install)(&m->cb, &m->pdi);

	ma->buttons	= m->pdi.buttons;
	ma->wheels	= m->pdi.wheels;
	ma->type	= m->pdi.type;

	m->bs_mask	= ~(0xffff << ma->buttons);

/* Now the mouse driver is ready for action. The VDI now have to intall the mouse
 * drivers 'housekeep' function into a VBI interrupt or something, as this is the
 * function that actually undraws/draws the cursor when it moves. Then the VDI has to
 * call 'enable' to turn the driver on.
*/
	return ma;
}

static void
enable_mouse(void)
{
	MOUSEDRV *m = &md;

	(*m->pdapi->start)();		/* start the mouse device driver */
	m->flags |= MDRV_ENABLED;	/* Indicate mouse is enabled */
}
static void
disable_mouse(void)
{
	MOUSEDRV *m = &md;

	if (m->flags & MDRV_ENABLED)
	{
		disable_mouse_curs();
		(*m->pdapi->stop)();		/* stop the mouse device driver */
		m->flags &= ~MDRV_ENABLED;	/* Indicate mouse is disabled */
	}
}

/* Let the mouse driver know about resolution changes, and fetch
 * the address of the mouse-rendering and virtual res check functions
 * from the main driver structure.
 */
static void
set_xmf_res(RASTER *r, COLINF *c)
{
	register MOUSEDRV *m = &md;
	register XMFORM *xmf = m->current_xmf;

	m->interrupt++;
	hide_mouse_curs();
	m->current_xms->valid = 0;

	m->raster	= r;
	m->colinf	= c;

	xmf->mx		= r->w - 1;
	xmf->my		= r->h - 1;
	xmf->bypl	= r->bypl;
	xmf->scr_base	= r->base;

	set_xmf_color(xmf);

	if (r->drawers->draw_mcurs)
		m->draw_mcurs	= r->drawers->draw_mcurs;
	else
		m->draw_mcurs = (DRAW_MC *)mdonothing;

	if (r->drawers->undraw_mcurs)
		m->undraw_mcurs	= r->drawers->undraw_mcurs;
	else
		m->undraw_mcurs = (UNDRAW_MC *)mdonothing;

	m->min_x	= m->min_y = 0;
	m->max_x	= r->w - 1;
	m->max_y	= r->h - 1;

	m->hide_ct = 1;
	m->current_x = m->max_x >> 1;
	m->current_y = m->max_y >> 1;
	m->current_bs = 0;
	m->changed_bs = 0;
	m->last_bs = 0;

	show_mouse_curs(0);
	m->interrupt--;
}

/* Grab data from a standard mform structure, as used by */
/* current versions of the different VDI/AES's		 */
static void
set_new_mform(MFORM *mf)
{
	int i;
	register unsigned short *dest, *data, *mask;
	register MOUSEDRV *m = &md;
	register XMFORM *xmf = m->current_xmf;

	m->interrupt++;
	hide_mouse_curs();
	
	xmf->xhot	= mf->xhot;
	xmf->yhot	= mf->yhot;
	xmf->planes	= mf->planes;
	xmf->mfbypl	= 2;
	xmf->width	= 16;
	xmf->height	= 16;

	xmf->fg_col	= mf->fg_col;
	xmf->bg_col	= mf->bg_col;

	set_xmf_color(xmf);

	dest = (unsigned short *)xmf->data;
	data = (unsigned short *)&mf->data;
	mask = (unsigned short *)&mf->mask;

	for (i = 0; i < 16; i++)
	{
		*dest++ = mask[i];
		*dest++ = data[i];
	}

	show_mouse_curs(0);
	m->interrupt--;
}
static void
set_xmf_color(XMFORM *xmf)
{
	MOUSEDRV *m = &md;
	RASTER *r = m->raster;
	COLINF *c = m->colinf;

	if (r && c)
	{
		int f, b, mc;

		mc = c->pens - 1;

		f = xmf->fg_col;
		if (f > mc)
			f = mc;

		b = xmf->bg_col;
		if (b > mc)
			b = mc;

		xmf->fg_pix = c->pixelvalues[c->color_vdi2hw[f]];
		xmf->bg_pix = c->pixelvalues[c->color_vdi2hw[b]];
	}
}	
			


/* These are the functions called by mouse device drivers	*/
/* (layer 2) to let the high-level know about mouse activity	*/
void _cdecl
mouse_relative_move(short x, short y)
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
	(	"movem.l d2-d7/a0-a6,-(sp)\n\t"		\
		"move.l	 %2,a0\n\t"			\
		"jsr	 (a0)\n\t"			\
		"movem.l (sp)+,d2-d7/a0-a6\n\t"		\
		:
		: "d"(nx),"d"(ny),"a"(la->user_mot)
		: "a0"
	);

	m->current_x = nx;
	m->current_y = ny;

	la->gcurx = nx;
	la->gcury = ny;

	__asm__ volatile
	(	"movem.l d2-d7/a0-a6,-(sp)\n\t"		\
		"move.l	 %2,a0\n\t"			\
		"jsr	 (a0)\n\t"			\
		"movem.l (sp)+,d2-d7/a0-a6\n\t"		\
		:
		: "d"(nx),"d"(ny),"a"(la->user_cur)
		: "a0"
	);

	m->flags |= MC_MOVED;
}

void _cdecl
mouse_absolute_move(short x, short y)
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
	(	"movem.l d2-d7/a0-a6,-(sp)\n\t"		\
		"move.l	 %2,a0\n\t"			\
		"jsr	 (a0)\n\t"			\
		"movem.l (sp)+,d2-d7/a0-a6\n\t"		\
		:
		: "d"(nx),"d"(ny),"a"(la->user_mot)
		: "a0"
	);

	m->current_x = nx;
	m->current_y = ny;

	la->gcurx = nx;
	la->gcury = ny;

	__asm__ volatile
	(	"movem.l d2-d7/a0-a6,-(sp)\n\t"		\
		"move.l	 %2,a0\n\t"			\
		"jsr	 (a0)\n\t"			\
		"movem.l	 (sp)+,d2-d7/a0-a6\n\t"	\
		:
		: "d"(x),"d"(y),"a"(la->user_cur)
		: "a0"
	);

	m->flags |= MC_MOVED;
}

void _cdecl
mouse_buttons_change(unsigned short bs)
{
	register MOUSEDRV *m = &md;
	register unsigned short nbs __asm__ ("d0") = bs;
	register unsigned short obs = m->current_bs;

	if (nbs != obs)
	{
		__asm__ volatile
		(	"movem.l d1-d7/a0-a6,-(sp)\n\t"		\
			"move.l	%1,a0\n\t"			\
			"jsr	(a0)\n\t"			\
			"movem.l	(sp)+,d1-d7/a0-a6\n\t"	\
			:
			: "d"(nbs),"a"(m->la->user_but)
			: "d0","d1","a0"
		);

		m->current_bs = nbs;
		m->changed_bs ^= nbs;
		m->last_bs = m->changed_bs;
	}
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
}
#if 0
static short speed_tab[] =
{ 0, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 14, 255, 255 };
#else
static short speed_tab[] =
{ 0, 255, 255, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 14, 255, 255 };
#endif
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
}

void
mouse_interrupt(void)
{
	register MOUSEDRV *m = &md;

	if ( !(m->flags & MDRV_ENABLED) || m->interrupt)
		return;

	m->interrupt++;

	if (m->flags & MC_ENABLED)
	{
		if (m->flags & MC_MOVED)
		{
			MOUSE_SEMA(m);
			if (!m->hide_ct)
			{
				(*m->undraw_mcurs)(m->current_xms);
				(*m->draw_mcurs)(m->current_xmf, m->current_x, m->current_y);
			}

			if (m->vreschk)
				(*m->vreschk)(m->current_x, m->current_y);

			m->flags &= ~MC_MOVED;
		}
	}
	m->interrupt--;
}

static void
reset_mouse_curs(void)
{
	enable_mouse_curs();
	show_mouse_curs(0);
}

static void
enable_mouse_curs(void)
{
	register MOUSEDRV *m = &md;

	m->hide_ct = 1;
	m->flags |= MC_ENABLED;
	m->interrupt = 0;
}

static void
disable_mouse_curs(void)
{
	register MOUSEDRV *m = &md;

	hide_mouse_curs();
	m->flags &= ~MC_ENABLED;
	m->interrupt = 1;
}
	
static void
show_mouse_curs(short reset)
{
	register MOUSEDRV *m = &md;

	m->interrupt++;

	if ((m->flags & MC_ENABLED) && m->hide_ct)
	{
		MOUSE_SEMA(m);
		if (reset)
		{
			/* If a reset, check if cursor was shown and undraw if it was.
			 * Else we end up drawing the new cursor over the already shown one.
			*/
			if (!m->hide_ct)
				(*m->undraw_mcurs)(m->current_xms);

			m->hide_ct = 1;
		}

		m->hide_ct--;

		if (!m->hide_ct)
		{
			(*m->draw_mcurs)(m->current_xmf, m->current_x, m->current_y);
			m->flags &= ~MC_MOVED;
		}
	}
	m->interrupt--;
}

static void
hide_mouse_curs(void)
{
	register MOUSEDRV *m = &md;

	m->interrupt++;
	if (m->flags & MC_ENABLED)
	{
		MOUSE_SEMA(m);
		if (!m->hide_ct)
			(*m->undraw_mcurs)(m->current_xms);

		m->hide_ct++;
	}
	m->interrupt--;
}

static unsigned long
get_button_status(void)
{
	MOUSEDRV *m = &md;
	unsigned long bs;

	bs = (unsigned long)m->last_bs << 16;
	m->last_bs = 0;
	bs |= (unsigned short)m->current_bs;
	return bs;
}


static void
get_mouse_coordinates(short *xy)
{
	MOUSEDRV *m = &md;

	*xy++	= m->current_x;
	*xy	= m->current_y;
}

static unsigned long
set_mouse_vector(short vecnum, unsigned long vector)
{
	register MOUSEDRV *m = &md;
	unsigned long oldvec = 0;

	switch (vecnum)
	{
		case MVEC_BUT:
		{
			oldvec = (unsigned long)m->la->user_but;
			m->la->user_but = (void *)vector;
			break;
		}
		case MVEC_CUR:
		{
			oldvec = (unsigned long)m->la->user_cur;
			m->la->user_cur = (void *)vector;
			break;
		}
		case MVEC_MOV:
		{
			oldvec = (unsigned long)m->la->user_mot;
			m->la->user_mot = (void *)vector;
			break;
		}
	}
	return oldvec;
}

static void
mdonothing()
{
}
