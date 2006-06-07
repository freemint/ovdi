#ifndef _PDVAPI_H
#define _PDVAPI_H

#include "ovdi_types.h"
typedef void CMOVE(short x, short y);
typedef void MBCHG(unsigned short bs);
typedef void MWHEEL(short id, short pos, unsigned short bs);

struct mdrv_cb
{
	CMOVE	*relcmove;
	CMOVE	*abscmove;
	MBCHG	*buttchng;
	MWHEEL	*relwheel;
	MWHEEL	*abswheel;
#if 0
	void (*relcmove)(short x, short y);
	void (*abscmove)(short x, short y);
	void (*buttchng)(unsigned short bs);
	void (*relwheel)(short id, short pos, unsigned short bs);
	void (*abswheel)(short id, short pos, unsigned short bs);
#endif
};
typedef struct mdrv_cb MDRV_CB;
	
struct pdvinfo
{
	short	type;
	short	buttons;
	short	wheels;
};
typedef struct pdvinfo PDVINFO;

struct pdvapi
{
	struct pdvapi	*nxtapi;
	long		version;
	char		*sname;
	char		*lname;
	char		*filename;
	char		*pathname;
	short		(*install)(MDRV_CB *cb, PDVINFO *pdi);
	void		(*exit)(void);
	void		(*start)(void);
	void		(*stop)(void);
};
typedef struct pdvapi PDVAPI;	

#define	PDT_MOUSE	1;
#define PDT_TABLET	2;


#endif	/* _PDVAPI_H */
