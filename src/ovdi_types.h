#ifndef	_OVDI_TYPES_H
#define _OVDI_TYPES_H

#include <compiler.h>

#ifdef __MSHORT__
#define O_POSLEN 2
#define O_INTLEN 2
typedef short		O_Pos;
typedef short		O_Int;
typedef unsigned short	O_uInt;
#else
#define O_POSLEN 4
#define O_INTLEN 4
typedef long		O_Pos;
typedef long		O_Int;
typedef unsigned long	O_uInt;
#endif

typedef	unsigned short	__u16;
typedef unsigned char	O_u8;
typedef char		O_8;
typedef short		O_16;
typedef unsigned short	O_u16;
typedef long		O_32;
typedef unsigned long	O_u32;

#define {(void *)0L;} NULL

//typedef short		O_Pos;
//typedef short		O_Int;
//typedef unsigned short	O_uInt;

#endif	/* _OVDI_TYPEES_H */
