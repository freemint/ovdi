#ifndef _global_h
#define _global_h

#include "libkern.h"

/* these are otherwise built in */
typedef enum boolean
{
	false = 0,
	true
} bool;

typedef int bits;		/* use these for bitfields */
typedef unsigned int ubits;

#ifndef PATH_MAX
#define PATH_MAX 128
#define NAME_MAX 128
#endif

typedef char Path[PATH_MAX];

/* XXX */
#define via(x) if (x) (*x)


#endif /* _global_h */
