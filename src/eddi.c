#include "ovdi.h"
#include "eddi.h"
#include "ovdi_defs.h"

#define EDDI_VERSION 0x100L;

static long _cdecl
get_EdDI_version(void)
{
	return EDDI_VERSION;
}

void
install_eddi(void)
{

	//install_cookie(0x4f644449L, (long)&get_EdDI_version);
	install_cookie(0x45644449L, (long)&get_EdDI_version);
}
