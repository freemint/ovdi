/* This is the pointing device driver for ATari-mouses  */

#include <osbind.h>

#include "modinf.h"
#include "ovdi_lib.h"
#include "pdvapi.h"

#define ON 1

void init(OVDI_LIB *l, struct module_desc *ret);

static char sname[] = "Atari IKBD Mouse\0";
static char lname[] = "Atari IKBD Mouse pointing device driver for oVDI\0";

static short install(MDRV_CB *, PDVINFO *);

static void exit(void);

static void start_reporting(void);
static void end_reporting(void);
void IKBD_Mouse(char *pkt);
extern void ikbd_mouse(void);

static struct mdrv_cb *CB = 0;

static PDVAPI pdvapi =
{
	0,
	0x00000001,
	sname,
	lname,

	install,
	exit,
	start_reporting,
	end_reporting,
};

static short flags = 0;

static unsigned char mparams[] =
{
	0,	/* topmode	*/
	0,	/* buttons	*/
	1,	/* xparam	*/
	1	/* yparam	*/
};

void
init(OVDI_LIB *l, struct module_desc *ret)
{
	PDVAPI *p = &pdvapi;

	ret->types	= D_PDV;
	ret->pdv	= (void *)p;
}

static short
install(MDRV_CB *cb, PDVINFO *pdi)
{
	CB = cb;

	pdi->type	= PDT_MOUSE;
	pdi->buttons	= 2;
	pdi->wheels	= 0;

	return 0;
}

static void
exit(void)
{
	end_reporting();
	CB = 0;
}

static void
start_reporting()
{
	if (CB)
	{
		flags |= ON;
		Initmous( 1, &mparams, &ikbd_mouse);
	}
}

static void
end_reporting()
{
	flags &= ~ON;
	Initmous( 0, -1L, -1L);
}

void
IKBD_Mouse(char *pkt)
{
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

			(*CB->buttchng)(buts);
	
			x = *pkt++;
			y = *pkt;

			if (x | y)
				(*CB->relcmove)(x, y);
		}
	}
}
