#ifndef _MOUSE_H
#define _MOUSE_H

#include "ovdi_types.h"

struct mform
{
	O_16	xhot;
	O_16	yhot;
	O_16	planes;
	O_16	fg_col;
	O_16	bg_col;
	O_u16	mask[16];
	O_u16	data[16];
};
typedef struct mform MFORM;

struct msave
{
	O_16	width;
	O_16	height;
	O_16	bypl;
	O_16	valid;
	O_u8	*addr;
	O_u8	data[(16*16) * 32];	/* Max 32 bits per pixel */
};
typedef struct msave MSAVE;

#endif	/* _MOUSE_H */
