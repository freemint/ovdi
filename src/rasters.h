#ifndef _RASTERS_H
#define _RASTERS_H

#include "ovdi_types.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

O_Int	fix_raster_coords	(O_Pos *, O_Pos *, O_Pos *);

void	rt_cpyfm	( RASTER *r, COLINF *c, MFDB *src, MFDB *dst, O_Pos *coords, VDIRECT *clip, O_Int fgcol, O_Int bgcol, O_Int wrmode);
void	ro_cpyfm	( RASTER *r, MFDB *src, MFDB *dst, O_Pos *coords, VDIRECT *clip, O_Int wrmode);
void	trnfm		( MFDB *src, MFDB *dst);

void conv_vdi2dev_1b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_vdi2dev_2b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_vdi2dev_4b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_vdi2dev_8b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_vdi2dev_16b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_vdi2dev_24b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_vdi2dev_32b	( O_u16 *src, O_u16 *dst, O_u32 splen);

void conv_dev2vdi_1b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_dev2vdi_2b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_dev2vdi_4b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_dev2vdi_8b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_dev2vdi_16b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_dev2vdi_24b	( O_u16 *src, O_u16 *dst, O_u32 splen);
void conv_dev2vdi_32b	( O_u16 *src, O_u16 *dst, O_u32 splen);

#endif	/* _RASTERS_H */
