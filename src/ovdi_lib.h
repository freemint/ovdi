#ifndef	_OVDI_LIB_H
#define _OVDI_LIB_H

#include "ovdi_types.h"
#include "libkern.h"
#include "parser.h"

struct ovdi_lib
{
	O_Int	_cdecl (*getcookie)	(O_32 tag, O_32 *ret);


	void *	_cdecl (*memcpy)	(void *dst, const void *src, O_u32 nbytes);
	void *	_cdecl (*memset)	(void *dst, O_Int fill, O_u32 size);
	void	_cdecl (*bcopy)		(const void *src, void *dst, O_u32 nbyte);
	void	_cdecl (*bzero)		(void *dst, O_u32 size);

	void *	_cdecl (*get_mem)	(O_u32 size, O_Int type);
	void	_cdecl (*free_mem)	(void *where);

	long	_cdecl (*get_fsize)	(char *fname);
	long	_cdecl (*load_file)	(char *fname, O_u32 bytes, char *buff);

	void	_cdecl (*cnf)		(char *fname, struct parser_item *items);

	void	_cdecl (*scrnlog)	(const char *t, ...);
	void	_cdecl (*filelog)	(const char *t, ...);

	O_32	_cdecl (*supexec)	(void *func);
};
typedef struct ovdi_lib OVDI_LIB;

#endif	/* _OVDI_LIB_H */
