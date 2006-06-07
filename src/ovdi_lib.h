#ifndef	_OVDI_LIB_H
#define _OVDI_LIB_H

#include "ovdi_types.h"
#include "libkern.h"
#include "parser.h"

struct ovdi_lib
{
	short	_cdecl (*getcookie)	(long tag, long *ret);

	void *	_cdecl (*memcpy)	(void *dst, const void *src, unsigned long nbytes);
	void *	_cdecl (*memset)	(void *dst, int fill, unsigned long size);
	void	_cdecl (*bcopy)		(const void *src, void *dst, unsigned long nbyte);
	void	_cdecl (*bzero)		(void *dst, unsigned long size);
	
	long	_cdecl (*strlen)	(const char *str);
	char *	_cdecl (*strcpy)	(char *d, const char *s);
	
	void *	_cdecl (*malloc)	(unsigned long size, short type);
	void	_cdecl (*mfree)		(void *where);

	long	_cdecl (*get_fsize)	(char *fname);
	long	_cdecl (*load_file)	(char *fname, unsigned long bytes, char *buff);

	void	_cdecl (*cnf)		(char *fname, struct parser_item *items);

	void	_cdecl (*scrnlog)	(const char *t, ...);
	void	_cdecl (*log)		(const char *t, ...);

	long	_cdecl (*supexec)	(void *func);
};
typedef struct ovdi_lib OVDI_LIB;

#endif	/* _OVDI_LIB_H */
