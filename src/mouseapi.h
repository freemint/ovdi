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
	void		(*resetmcurs)(void);
	void		(*enablemcurs)(void);
	void		(*disablemcurs)(void);
	void		(*showmcurs)(short);
	void		(*hidemcurs)(void);

	void		(*relmovmcurs)(short x, short y);
	void		(*absmovmcurs)(short x, short y);
	void		(*butchg)(unsigned short bs);

	unsigned long 	(*getbutstat)(void);
	void		(*getmcoords)(short *xy);

#define MVEC_BUT	0
#define MVEC_CUR	1
#define MVEC_MOV	2
	unsigned long	(*setvector)(short vecnum, unsigned long vector);
	void		(*housekeep)(void);
};
typedef struct mouseapi MOUSEAPI;

#endif	/* _MOUSEAPI_H */
