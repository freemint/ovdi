#ifndef _PDVAPI_H
#define _PDVAPI_H

#include "ovdi_types.h"

struct mdrv_cb
{
	void (*relcmove)(O_Int x, O_Int y);
	void (*abscmove)(O_Int x, O_Int y);
	void (*buttchng)(O_uInt bs);
	void (*relwheel)(O_Int id, O_Int pos, O_uInt bs);
	void (*abswheel)(O_Int id, O_Int pos, O_uInt bs);
};
typedef struct mdrv_cb MDRV_CB;
	
struct pdvinfo
{
	O_16	type;
	O_16	buttons;
	O_16	wheels;
};
typedef struct pdvinfo PDVINFO;

struct pdvapi
{
	struct pdvapi	*nxtapi;
	O_32		version;
	char		*sname;
	char		*lname;
	char		*filename;
	char		*pathname;
	O_Int		(*install)(MDRV_CB *cb, PDVINFO *pdi);
	void		(*exit)(void);
	void		(*start)(void);
	void		(*stop)(void);
};
typedef struct pdvapi PDVAPI;	

#define	PDT_MOUSE	1;
#define PDT_TABLET	2;


#endif	/* _PDVAPI_H */
