#include <osbind.h>

#include "mousedrv.h"

void init_mouse_device(struct mouseapi *mapi);
void exit_mouse_device(void);

void start_reporting(void);
void end_reporting(void);
void IKBD_Mouse(char *pkt);
extern void ikbd_mouse(void);

struct mouseapi *m_api;

unsigned char mparams[] =
{
	0,	/* topmode	*/
	0,	/* buttons	*/
	1,	/* xparam	*/
	1	/* yparam	*/
};


void
init_mouse_device(struct mouseapi *mapi)
{
#ifndef PRG_TEST
	m_api = mapi;

	mapi->buttons = 2;
	mapi->wheels = 0;

	start_reporting();
#endif
	return;
}

void
exit_mouse_device(void)
{
	end_reporting();
	return;
}

void
start_reporting()
{
	Initmous( 1, &mparams, &ikbd_mouse);
	return;
}

void
end_reporting()
{
	Initmous( 0, -1L, -1L);
	return;
}


void
IKBD_Mouse(char *pkt)
{
	struct mouseapi *mapi = m_api;
	register char *p = pkt;
	register unsigned short buts;
	register short x, y;
	unsigned char head;


	head = *p++;

	if ((head & 0xf8) == 0xf8)
	{
		buts = (short)head & 3;
		if (buts & 1)
			buts = (buts >> 1) | 2;
		else
			buts = buts >> 1;

		(*mapi->butchg)(buts);
	
		x = *p++;
		y = *p++;

		if (x | y)
			(*mapi->relmovmcurs)(x, y);
	}
	return;
}
