#include "ovdi_types.h"

long col2long1b[] =
{
	0x00000000,0x00000000,
	0xffff0000,0x00000000,
	0x0000ffff,0x00000000,
	0xffffffff,0x00000000,
	0x00000000,0xffff0000,
	0xffff0000,0xffff0000,
	0x0000ffff,0xffff0000,
	0xffffffff,0xffff0000,
	0x00000000,0x0000ffff,
	0xffff0000,0x0000ffff,
	0x0000ffff,0x0000ffff,
	0xffffffff,0x0000ffff,
	0x00000000,0xffffffff,
	0xffff0000,0xffffffff,
	0x0000ffff,0xffffffff,
	0xffffffff,0xffffffff
};

O_u16 shifts1b[] = 
{
	0xffff,
	0xffff >> 1,
	0xffff >> 2,
	0xffff >> 3,
	0xffff >> 4,
	0xffff >> 5,
	0xffff >> 6,
	0xffff >> 7,
	0xffff >> 8,
	0xffff >> 9,
	0xffff >> 10,
	0xffff >> 11,
	0xffff >> 12,
	0xffff >> 13,
	0xffff >> 14,
	0xffff >> 15,
	0
};

O_u16 fillbuff1b[4*16];
O_u16 maskbuff1b[16];
