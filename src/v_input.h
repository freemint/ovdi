#ifndef _V_INPUT_H
#define _V_INPUT_H

#include "ovdi_defs.h"
#include "vdi_defs.h"

void	vqin_mode	( VDIPB *pb, VIRTUAL *v);
void	vsin_mode	( VDIPB *pb, VIRTUAL *v);
int	lvsin_mode	( VIRTUAL *v, int dev, int mode);

void	vxx_locator	( VDIPB *pb, VIRTUAL *v);
void	vxx_valuator	( VDIPB *pb, VIRTUAL *v);
void	vxx_choice	( VDIPB *pb, VIRTUAL *v);
void	vxx_string	( VDIPB *pb, VIRTUAL *v);

void	vq_key_s	( VDIPB *pb, VIRTUAL *v);


#endif	/* _V_INPUT_H */
