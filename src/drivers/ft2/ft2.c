#include "ovdi_defs.h"
#include "ovdi_lib.h"
#include "modinf.h"
#include "memory.h"
#include "ft2.h"

extern long get_instance_size(void);
extern long open(void *);

OVDI_LIB *l;

int
main(void)
{ return 0; }

void _cdecl
init(OVDI_LIB *lib, struct module_desc *ret, char *p, char *f)
{
	long handle, size;
	void *where;

	l = lib;

	size = get_instance_size();
	where = (void *)(*l->get_mem)(size, MX_PREFTTRAM | MX_GLOBAL);

	handle = open(where);
}

