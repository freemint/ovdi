#ifndef _RASTERS_H
#define _RASTERS_H

#include "ovdi_defs.h"
#include "vdi_defs.h"

short	fix_raster_coords	(short *, short *, short *);
void	expand		(long *pixelvalues, short splanes, unsigned short *src, short dplanes, unsigned short *dst, short pwidth, short pheight );

void	rt_cpyfm	( VIRTUAL *v, MFDB *src, MFDB *dst, short *coords, short fgcol, short bgcol, short wrmode);
void	ro_cpyfm	( VIRTUAL *v, MFDB *src, MFDB *dst, short *coords, short wrmode);
void	trnfm		( VIRTUAL *v, MFDB *src, MFDB *dst);

void conv_vdi2dev_1b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_vdi2dev_2b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_vdi2dev_4b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_vdi2dev_8b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_vdi2dev_16b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_vdi2dev_24b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_vdi2dev_32b	( unsigned short *src, unsigned short *dst, unsigned long splen);

void conv_dev2vdi_1b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_dev2vdi_2b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_dev2vdi_4b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_dev2vdi_8b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_dev2vdi_16b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_dev2vdi_24b	( unsigned short *src, unsigned short *dst, unsigned long splen);
void conv_dev2vdi_32b	( unsigned short *src, unsigned short *dst, unsigned long splen);

#endif	/* _RASTERS_H */
