#include <osbind.h>
#include <mintbind.h>
#include <mint/basepage.h>

#include "module.h"

BASEPAGE *
load_module(char *filename, long *err)
{
	BASEPAGE *b = 0;
	long keep;

	b = (BASEPAGE *)Pexec( 3, filename, "", 0);

	if ((long)b < 0L)
	{
		*err = (long)b;
		return 0;
	}

	keep	= 0x200 + b->p_tlen + b->p_dlen + b->p_blen;

	Mshrink((void *)b, (long)keep);

	*err = 0;
	return b;
}

long
callout_init(BASEPAGE *b, long param)
{
	long init_ret;
	long (*callout)(long);

	callout = (long (*)(long))b->p_tbase;

	init_ret = (*callout)(param);

	return init_ret;
}
