#include <mint/mintbind.h>
#include <fcntl.h>

#include "libkern.h"
#include "file.h"
#include "vdi_globals.h"

long _cdecl
get_file_size( char *fn )
{
	long	fs;

	if (MiNT)
	{
		XATTR x;

		fs = Fxattr(0, fn, &x);
		if (fs < 0)
			return fs;

		fs = x.size;
	}
	else
	{
		struct _dta new;
		struct _dta *old;

		old = Fgetdta();
		Fsetdta(&new);
		fs = Fsfirst(fn, 0);
		Fsetdta(old);

		if (fs < 0)
			return fs;

		fs = new.dta_size;
	}
	return fs;
}

long _cdecl
load_file(char *fn, unsigned long bytes, char *b)
{
	long fh, lbytes;

	fh = Fopen(fn, O_RDONLY);

	if (fh < 0)
		return fh;

	lbytes = Fread(fh, bytes, b);
	Fclose(fh);
	return lbytes;
}

