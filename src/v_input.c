#include <osbind.h>

#include "display.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "v_input.h"
#include "mousedrv.h"
#include "kbddrv.h"

#define SCAN_F1		0x3b
#define SCAN_F10	0x44

#define KEY_UP		0x48
#define KEY_DN		0x50
#define KEY_LT		0x4b
#define KEY_RT		0x4d

void
vqin_mode( VDIPB *pb, VIRTUAL *v)
{
	short mode = -1;

	switch (pb->intin[0])
	{
		case 1:
		{
			mode = v->locator;
			break;
		}
		case 2:
		{
			mode = v->valuator;
			break;
		}
		case 3:
		{
			mode = v->choise;
			break;
		}
		case 4:
		{
			mode = v->string;
			break;
		}
	}

	if (mode == -1)
		pb->intout[0] = 0;
	else
		pb->intout[0] = mode & REQ_MODE ? 1 : 2;

	pb->contrl[N_INTOUT] = 1;
	return;
}

void
vsin_mode( VDIPB *pb, VIRTUAL *v)
{
	pb->intout[0] = lvsin_mode( v, pb->intin[0], pb->intin[1]);
	pb->contrl[N_INTOUT] = 1;
	return;
}
	
short
lvsin_mode( VIRTUAL *v, short dev, short mode)
{
	short *flag = 0;
	short ret;

	switch (dev)
	{
		case 1:
		{
			flag = (short *)&v->locator;
			break;
		}
		case 2:
		{
			flag = (short *)&v->valuator;
			break;
		}
		case 3:
		{
			flag = (short *)&v->choise;
			break;
		}
		case 4:
		{
			flag = (short *)&v->string;
			break;
		}
	}

	if (flag)
	{
		if (mode == 1)
		{
			*flag |= REQ_MODE;
			ret = 1;
		}
		else if (mode == 2)
		{
			*flag &= ~REQ_MODE;
			ret = 2;
		}
		else
			ret = 0;
	}
	else
		ret = 0;

	return ret;
}
	

/* Mouse, Mouse buttons, and keyboard */
void
vxx_locator( VDIPB *pb, VIRTUAL *v)
{
	unsigned long bs, key;

	bs = key = 0;

	(*v->mouseapi->absmovmcurs)(pb->ptsin[0], pb->ptsin[1]);

	if (v->locator & REQ_MODE)
	{

		do {}
		while ( (!(key = (*v->kbdapi->keywaiting)())) &&
			(!(bs = (*v->mouseapi->getbutstat)() & 0xffffUL))
			);

		if (key)
		{
			key = (*v->kbdapi->getkey)();
			pb->intout[0] = (unsigned short)key & 0xff;
			pb->contrl[N_INTOUT] = 1;
		}

		if (bs)
		{
			if (bs & MB_LEFT)
				pb->intout[0] = 0x20;
			else if (bs & MB_RIGHT)
				pb->intout[0] = 0x21;
			pb->contrl[N_INTOUT] = 1;
		}
			
		(*v->mouseapi->getmcoords)((short *)&pb->ptsout[0]);
		pb->contrl[N_PTSOUT] = 1;

		display("buttons %lx, key %lx\n", bs, key);

	}
	else /* Sample mode */
	{
		short xy[2];
		short ret = 0;

		bs = (*v->mouseapi->getbutstat)();
		key = (*v->kbdapi->keywaiting)();
		(*v->mouseapi->getmcoords)((short *)&xy);

		if (xy[0] != pb->ptsin[0] || xy[1] != pb->ptsin[1])
		{
			pb->ptsout[0] = xy[0];
			pb->ptsout[1] = xy[1];
			pb->contrl[N_PTSOUT] = 1;
		}

		if (key)
		{
			key = (*v->kbdapi->getkey)();
			pb->intout[0] = (unsigned short)key & 0xff;
			pb->contrl[N_INTOUT] = 1;
		}	

		if (bs)
		{
			ret |= 2;
			
			if (bs | MB_LEFT)
				pb->intout[0] = 0x20;
			else
				pb->intout[0] = 0x21;

			pb->contrl[N_INTOUT] = 1;
		}
	}
	return;
}

/* Currently undefined */
void
vxx_valuator( VDIPB *pb, VIRTUAL *v)
{
	if (v->valuator & REQ_MODE)
	{
	}
	else /* sample mode */
	{
	}
	return;
}

/* Function keys */
void
vxx_choice( VDIPB *pb, VIRTUAL *v)
{
	unsigned long key;
	short scan;

	if (v->valuator & REQ_MODE)
	{
		key = (*v->kbdapi->waitkey)();
		scan = (short)((key >> 8) & 0xff);
		if (scan >= SCAN_F1 && scan <= SCAN_F10)
		{
			pb->intout[0] = scan - SCAN_F1 + 1;
			pb->contrl[N_INTOUT] = 1;
		}

	}
	else /* sample mode */
	{
		if ( (*v->kbdapi->keywaiting)() )
		{
			key = (*v->kbdapi->getkey)();
			scan = (short)((key >> 8) & 0xff);
			if (scan >= SCAN_F1 && scan <= SCAN_F10)
			{
				pb->intout[0]  = scan - SCAN_F1 + 1;
				pb->contrl[N_INTOUT] = 1;
			}
		}
	}
	return;
}

/* string (keyboard) */
void
vxx_string( VDIPB *pb, VIRTUAL *v)
{
	short maxlen, keep, i;
	unsigned long key;

	maxlen = pb->intin[0];

	if (maxlen < 0)
	{
		keep = 0xffff;
		maxlen = -maxlen;
	}
	else
		keep = 0xff;

	if (v->string & REQ_MODE)
	{
		for (i = 0; i < maxlen; i++)
		{
			key = (*v->kbdapi->waitkey)();

			if ( (key & 0xff) == 13)
				break;

			pb->intout[i] = (unsigned short)key & keep;
		}
		pb->contrl[N_INTOUT] = i;
	}
	else /* sample mode */
	{
		i = 0;
		while ( (i < maxlen) && (key = (*v->kbdapi->keywaiting)()) )
		{
			key = (*v->kbdapi->getkey)();
			pb->intout[i] = (unsigned short)key & keep;
			i++;
		}
		pb->contrl[N_INTOUT] = i;
	}
	return;
}

void
vq_key_s( VDIPB *pb, VIRTUAL *v)
{
	unsigned long ks;

	ks = (*v->kbdapi->getks)();
	pb->intout[0] = (short)((*v->kbdapi->getks)() & 0xffffUL);
	pb->contrl[N_INTOUT] = 1;
	return;
}
