#ifndef	_MODULE_H
#define _MODULE_H

BASEPAGE *	load_module(char *filename, long *err);
long		callout_init(BASEPAGE *b, long param);

#endif	/* _MODULE_H */
