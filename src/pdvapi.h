#ifndef _PDVAPI_H
#define _PDVAPI_H

#include "ovdi_types.h"
typedef void _cdecl CMOVE(short x, short y);
typedef void _cdecl MBCHG(unsigned short bs);
typedef void _cdecl MWHEEL(short id, short pos, unsigned short bs);

/*
 * A Pointer to
 */
struct mdrv_cb
{
	CMOVE	*relcmove;
	CMOVE	*abscmove;
	MBCHG	*buttchng;
	MWHEEL	*relwheel;
	MWHEEL	*abswheel;
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
	short _cdecl	(*install)(MDRV_CB *cb, PDVINFO *pdi);
	void  _cdecl	(*exit)(void);
	void  _cdecl	(*start)(void);
	void  _cdecl	(*stop)(void);
};
typedef struct pdvapi PDVAPI;	

#define	PDT_MOUSE	1;
#define PDT_TABLET	2;


#endif	/* _PDVAPI_H */
