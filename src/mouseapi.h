#ifndef _MOUSEAPI_H
#define _MOUSEAPI_H

#include "mouse.h"
#include "ovdi_defs.h"

struct mouseapi
{
	short		type;
	short		buttons;
	short		wheels;
	
	void		(*enable)(void);
	void		(*disable)(void);

	void		(*setxmfres)(struct raster *r, struct colinf *c);
	void		(*setnewmform)(MFORM *mf);
	void		(*resetmcurs)();
	void		(*enablemcurs)();
	void		(*disablemcurs)();
	void		(*showmcurs)(short);
	void		(*hidemcurs)();

	void		(*relmovmcurs)(register short x, register short y);
	void		(*absmovmcurs)(register short x, register short y);
	void		(*butchg)(register unsigned short bs);

	unsigned long 	(*getbutstat)();
	void		(*getmcoords)(short *xy);

#define MVEC_BUT	0
#define MVEC_CUR	1
#define MVEC_MOV	2
	unsigned long	(*setvector)(short vecnum, unsigned long vector);
	void		(*housekeep)(void);
};
typedef struct mouseapi MOUSEAPI;

#endif	/* _MOUSEAPI_H */