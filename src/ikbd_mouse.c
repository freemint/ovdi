/* This is the pointing device driver for ATari-mouses  */

#include <osbind.h>
#include "ovdi_defs.h"

#define ON 1

void init_mouse_device(struct mousedrv *);
void exit_mouse_device(void);

static void start_reporting(void);
static void end_reporting(void);
void IKBD_Mouse(char *pkt);
extern void ikbd_mouse(void);

static struct mousedrv *m_drv;
static short flags = 0;
static unsigned char mparams[] =
{
	0,	/* topmode	*/
	0,	/* buttons	*/
	1,	/* xparam	*/
	1	/* yparam	*/
};


void
init_mouse_device(struct mousedrv *mdrv)
{
	m_drv = mdrv;

	flags		= 0;
	mdrv->buttons	= 2;
	mdrv->wheels	= 0;

	mdrv->start	= start_reporting;
	mdrv->stop	= end_reporting;
	return;
}

void
exit_mouse_device(void)
{
	end_reporting();
	return;
}

static void
start_reporting()
{
	flags |= ON;
	Initmous( 1, &mparams, &ikbd_mouse);
	return;
}

static void
end_reporting()
{
	flags &= ~ON;
	Initmous( 0, -1L, -1L);
	return;
}


void
IKBD_Mouse(char *pkt)
{
	struct mousedrv *mdrv = m_drv;
	register unsigned short buts;
	register short x, y;
	char head;

	if (flags & ON)
	{
		head = *pkt++;

		if ((head & 0xf8) == 0xf8)
		{
			buts = (short)head & 3;
			if (buts & 1)
				buts = (buts >> 1) | 2;
			else
				buts = buts >> 1;

			(*mdrv->butchg)(buts);
	
			x = *pkt++;
			y = *pkt;

			if (x | y)
				(*mdrv->relmovmcurs)(x, y);
		}
	}
	return;
}
	
		
