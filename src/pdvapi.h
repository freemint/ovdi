#ifndef _PDVAPI_H
#define _PDVAPI_H

struct mdrv_cb
{
	void (*relcmove)(short x, short y);
	void (*abscmove)(short x, short y);
	void (*buttchng)(unsigned short bs);
	void (*relwheel)(short id, short pos, unsigned short bs);
	void (*abswheel)(short id, short pos, unsigned short bs);
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
	short		(*install)(MDRV_CB *cb, PDVINFO *pdi);
	void		(*exit)(void);
	void		(*start)(void);
	void		(*stop)(void);
};
typedef struct pdvapi PDVAPI;	

#define	PDT_MOUSE	1;
#define PDT_TABLET	2;


#endif	/* _PDVAPI_H */
