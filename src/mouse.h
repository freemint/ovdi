#ifndef _MOUSE_H
#define _MOUSE_H

struct mform
{
	short	xhot;
	short	yhot;
	short	planes;
	short	fg_col;
	short	bg_col;
	unsigned short	mask[16];
	unsigned short	data[16];
};
typedef struct mform MFORM;

struct msave
{
	short	width;
	short	height;
	short	bypl;
	short	valid;
	unsigned char *addr;
	unsigned char data[(16*16) * 32];	/* Max 32 bits per pixel */
};
typedef struct msave MSAVE;

#endif	/* _MOUSE_H */
