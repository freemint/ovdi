#ifndef	_OVDI_TYPES_H
#define _OVDI_TYPES_H

#include <compiler.h>

#ifndef _cdecl
#define _cdecl __CDECL
#endif

#ifdef __MSHORT__
#define O_POSLEN 2
#define O_INTLEN 2
#else
#define O_POSLEN 4
#define O_INTLEN 4
#endif

typedef	unsigned short	__u16;


//typedef short		short;
//typedef short		short;
//typedef unsigned short	unsigned short;

#endif	/* _OVDI_TYPEES_H */
