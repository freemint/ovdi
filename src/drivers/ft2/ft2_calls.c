#include <ft2build.h>
#include FT_FREETYPE_H


long
get_instance_size(void)
{
	return sizeof(FT_Library);
}

long
open(void *lib)
{
	long error;
	FT_Library lib;

	error = FT_Init_FreeType( &lib );

	return (long)&lib;
}

