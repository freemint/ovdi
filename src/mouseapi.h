#ifndef _MOUSEAPI_H
#define _MOUSEAPI_H

#include "mouse.h"
#include "ovdi_defs.h"

struct mouseapi
{
	short		type;
	short		buttons;
	short		wheels;
	
	void _cdecl	(*enable)(void);
	void _cdecl	(*disable)(void);

	void _cdecl	(*setxmfres)(struct raster *r, struct colinf *c);
	void _cdecl	(*setnewmform)(MFORM *mf);
	void _cdecl	(*resetmcurs)(void);
	void _cdecl	(*enablemcurs)(void);
	void _cdecl	(*disablemcurs)(void);
	void _cdecl	(*showmcurs)(short);
	void _cdecl	(*hidemcurs)(void);

	void _cdecl	(*relmovmcurs)(short x, short y);
	void _cdecl	(*absmovmcurs)(short x, short y);
	void _cdecl	(*butchg)(unsigned short bs);

	unsigned long _cdecl 	(*getbutstat)(void);
	void _cdecl		(*getmcoords)(short *xy);

#define MVEC_BUT	0
#define MVEC_CUR	1
#define MVEC_MOV	2
	unsigned long _cdecl	(*setvector)(short vecnum, unsigned long vector);
	void _cdecl		(*housekeep)(void);
};
typedef struct mouseapi MOUSEAPI;

#endif	/* _MOUSEAPI_H */
