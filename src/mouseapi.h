#ifndef _MOUSEAPI_H
#define _MOUSEAPI_H

#include "mouse.h"
#include "ovdi_defs.h"

struct mouseapi
{
	O_16		type;
	O_16		buttons;
	O_16		wheels;
	
	void		(*enable)(void);
	void		(*disable)(void);

	void		(*setxmfres)(struct raster *r, struct colinf *c);
	void		(*setnewmform)(MFORM *mf);
	void		(*resetmcurs)();
	void		(*enablemcurs)();
	void		(*disablemcurs)();
	void		(*showmcurs)(O_Int);
	void		(*hidemcurs)();

	void		(*relmovmcurs)(O_Int x, O_Int y);
	void		(*absmovmcurs)(O_Int x, O_Int y);
	void		(*butchg)(O_uInt bs);

	O_u32 		(*getbutstat)();
	void		(*getmcoords)(O_16 *xy);

#define MVEC_BUT	0
#define MVEC_CUR	1
#define MVEC_MOV	2
	O_u32		(*setvector)(O_Int vecnum, O_u32 vector);
	void		(*housekeep)(void);
};
typedef struct mouseapi MOUSEAPI;

#endif	/* _MOUSEAPI_H */