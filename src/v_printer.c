#include "vdi_defs.h"
#include "ovdi_defs.h"
#include "memory.h"
#include "workstation.h"
#include "vdi_globals.h"

#include "v_printer.h"

/* Opcode 1, subfunct 1 - NVDI 4.1x */
void
v_opnprn(VDIPB *pb, VIRTUAL *root)
{
	int handle = 0, i;
	struct opnwk_input *wkin = (struct opnwk_input *)&pb->intin[0];
	PRN_SETTINGS *wkin_pset = wkin->dev.prn.pset;

	if (wkin_pset->magic == 0x70736574L)	/* 'pset' */
	{
		VIRTUAL *new;
		PRN_SETTINGS *pset;

		(long)new = (long)omalloc(sizeof(VIRTUAL) + sizeof(PRN_SETTINGS), MX_PREFTTRAM | MX_SUPER);

		if (new)
		{
			for (i = 2; i < MAX_VIRTUALS; i++)
			{
				if (!v_vtab[i].v)
				{
					handle = i;
					break;
				}
			}
		
			if (handle)
			{
				wkin = (struct opnwk_input *)&pb->intin[0];

				(long)pset = (long)new + sizeof(VIRTUAL);

				bzero(new, sizeof(VIRTUAL));
				memcpy(pset, wkin_pset, sizeof(PRN_SETTINGS));

				get_MiNT_info(new);
				new->handle = handle;
				copy_common_virtual_vars(new, root);

				new->prn_settings = pset;

				pset->size_id	= wkin->dev.prn.size_id;
				{
					long *s, *d;
					s = (long *)wkin->dev.prn.device;
					d = (long *)&pset->device;
					for (i = 0; i < 32; i++)
						*d++ = *s++;
				}
			}
			else
			{
				free_mem(new);
				handle = 0;
			}
		}
	}
	pb->contrl[HANDLE] = handle;
}
