#ifndef _RASTERS_H
#define _RASTERS_H

#include "ovdi_types.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

short	_cdecl fix_raster_coords(short *, short *, short *);

void	_cdecl rt_cpyfm	( RASTER *r, COLINF *c, MFDB *src, MFDB *dst, short *coords, VDIRECT *clip, short fgcol, short bgcol, short wrmode);
void	_cdecl ro_cpyfm	( RASTER *r, MFDB *src, MFDB *dst, short *coords, VDIRECT *clip, short wrmode);
void	_cdecl trnfm		( RASTER *r, MFDB *src, MFDB *dst);

void _cdecl conv_vdi2dev_1b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_vdi2dev_2b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_vdi2dev_4b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_vdi2dev_8b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_vdi2dev_16b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_vdi2dev_24b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_vdi2dev_32b	( unsigned short *src, unsigned short *dst, unsigned long splen);

void _cdecl conv_dev2vdi_1b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_dev2vdi_2b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_dev2vdi_4b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_dev2vdi_8b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_dev2vdi_16b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_dev2vdi_24b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void _cdecl conv_dev2vdi_32b	( unsigned short *src, unsigned short *dst, unsigned long splen);

#endif	/* _RASTERS_H */
