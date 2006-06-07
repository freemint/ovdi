/*
 * This file belongs to oVDI.
 *
 * This file contains color and pixel related functions.
 *
 * Copyright 2004 Odd Skancke <ozk@atari.org>
 * All rights reserved.
 *
 * Author: Odd Skancke <ozk@atari.org>
 * Started: 2004-01-01
 *
 * please send suggestions, patches or bug reports to me.
 *
 *
 * changes since last version:
 *
 * - initial revision
 *
 * known bugs:
 *
 * -
 *
 * todo:
 *
 *
 */

/*
 * To illustrate how pixelformat is described, here is the one that describes
 * Falcon pixelformat.
 * This descritor is also used by vq_extend()

 * Falcon's 15-bit pixelformat bit layout
 * char pf_15b_falc[] =
 * {
 *	  5,  11,   1,		5 red bits start at bit 11. Add one to get to next red bit
 *	 11, 255,   0,		11 unused red bits followin the above red bits
 *
 *	  5,   6,   1,		5 green bits starting at bit 8
 *	 11, 255,   0,		11 Unused green bits
 *
 *	  5,   0,   1,		5 blue bits start at bit 0
 *	 11, 255,   0,		11 unused blue bits
 *
 *	 32, 255,   0,		32 unused alpha channel bits
 *
 *	  1,   5,   1,		1 overlay bit, starting at bit 5
 *	 31, 255,   0,		31 unused genlock/overlay bits
 *
 *	 32, 255,   0,		No unused bits
 *	 0			end of table
 * };
 */
#include "colors.h"
#include "display.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"

/*
 * Calculate real RGB values based on 'relative' RGB values (0 - 1000).
 * if pixelret is non-NULL, pixel value is written to that pointer.
*/
void
reqrgb_2_actrgb(char *pixfmt, RGB_LIST *levels, RGB_LIST *reqrgb, RGB_LIST *rgbvals, unsigned long *pixelret)
{
	unsigned long register tmp;

	tmp = reqrgb->red;
	tmp *= levels->red;
	tmp += 500;
	tmp /= 1000;
	rgbvals->red = tmp;

	tmp = reqrgb->green;
	tmp *= levels->green;
	tmp += 500;
	tmp /= 1000;
	rgbvals->green = tmp;

	tmp = reqrgb->blue;
	tmp *= levels->blue;
	tmp += 500;
	tmp /= 1000;
	rgbvals->blue = tmp;

	tmp = reqrgb->alpha;
	tmp *= levels->alpha;
	tmp += 500;
	tmp /= 1000;
	rgbvals->alpha = tmp;

	tmp = reqrgb->ovl;
	tmp *= levels->ovl;
	tmp += 500;
	tmp /= 1000;
	rgbvals->ovl = tmp;
#if 0
	rgbvals->red	= (unsigned short)(((long)reqrgb->red 	* levels->red + 500) / 1000);
	rgbvals->green	= (unsigned short)(((long)reqrgb->green * levels->green + 500) / 1000);
	rgbvals->blue	= (unsigned short)(((long)reqrgb->blue 	* levels->blue + 500) / 1000);
	rgbvals->alpha	= (unsigned short)(((long)reqrgb->alpha * levels->alpha + 500) / 1000);
	rgbvals->ovl	= (unsigned short)(((long)reqrgb->ovl 	* levels->ovl + 500) / 1000);
#endif
	if (pixelret)
		*pixelret = calc_pixelvalue( pixfmt, rgbvals);
}
/*
 * Convert real RGB values 'values', using RGB levels 'levels' into
 * relative RGB values written to 'result'.
*/
void
get_rgb_relatives( RGB_LIST *values, RGB_LIST *levels, RGB_LIST *result)
{
	unsigned long tmp;

	tmp = values->red;
	tmp *= 1000;
	tmp += (levels->red >> 1);
	tmp /= levels->red;
	result->red = tmp;

	tmp = values->green;
	tmp *= 1000;
	tmp += (levels->green >> 1);
	tmp /= levels->green;
	result->green = tmp;

	tmp = values->blue;
	tmp *= 1000;
	tmp += (levels->blue >> 1);
	tmp /= levels->blue;
	result->blue = tmp;

	tmp = values->alpha;
	tmp *= 1000;
	tmp += (levels->alpha >> 1);
	tmp /= levels->alpha;
	result->alpha = tmp;

	tmp = values->ovl;
	tmp *= 1000;
	tmp += (levels->ovl >> 1);
	tmp /= levels->ovl;
	result->ovl = tmp;
#if 0
	result->red	= ((long)values->red 	* 1000) / levels->red;
	result->green	= ((long)values->green	* 1000) / levels->green;
	result->blue	= ((long)values->blue	* 1000) / levels->blue;

	result->alpha	= levels->alpha ? ((long)values->alpha	* 1000) / levels->alpha : 0;
	result->ovl	= levels->ovl ? ((long)values->ovl	* 1000) / levels->ovl : 0;
#endif
}

/* Return specified channel of a pixel-value. which selects what channel to get
* which == 0: Get red bits
* which == 1: Get gree bits
* which == 2: get blue bits
* which == 3: Get alpha bits
* which == 4: get overlay/genlocking bits
*/
short
get_color_bits( char *pf, unsigned long pixel, short which)
{
	int i;
	register short colbits, shift;

	colbits = shift = 0;

	for (i = 0; i < which; i++)
	{
		while (pf[2])
			pf += 3;

		if (!pf[0])
			return -1;
	}

	while (pf[2])
	{
		colbits |= (short)(((pixel >> pf[1]) & (0xffffffffUL >> (32 - pf[0]))) << shift);
		shift += pf[0];
		pf += 3;
	}

	return colbits;
}

/* Get intensity levels per channel in a pixel */
void
get_rgb_levels( char *pf, RGB_LIST *levels)
{
	RGB_LIST rgbbits;

	get_rgb_bits(pf, &rgbbits);

	levels->red	= (unsigned short)(1 << rgbbits.red) - 1;
	levels->green	= (unsigned short)(1 << rgbbits.green) - 1;
	levels->blue	= (unsigned short)(1 << rgbbits.blue) - 1;
	levels->alpha	= (unsigned short)(1 << rgbbits.alpha) - 1;
	levels->ovl	= (unsigned short)(1 << rgbbits.ovl) - 1;
}

/* Get number of bits per channel in a pixel */
void
get_rgb_bits( char *pf, RGB_LIST *rgbbits)
{
	register short red, green, blue, alpha, ovl;

	red = green = blue = alpha = ovl = 0;

	while ( pf[2] )
	{
		red += pf[0];
		pf += 3;
	}
	pf += 3;
	while ( pf[2] )
	{
		green += pf[0];
		pf += 3;
	}
	pf += 3;
	while ( pf[2] )
	{
		blue += pf[0];
		pf += 3;
	}
	pf += 3;
	while ( pf[2] )
	{
		alpha += pf[0];
		pf += 3;
	}
	pf += 3;
	while ( pf[2] )
	{
		ovl += pf[0];
		pf += 3;
	}
	
	rgbbits->red	= red;
	rgbbits->green	= green;
	rgbbits->blue	= blue;
	rgbbits->alpha	= alpha;
	rgbbits->ovl	= ovl;
}

/* Calculate the true pixelvalue (as written to video-ram) */
unsigned long
calc_pixelvalue( char *pf, RGB_LIST *rgbv )
{
	register unsigned long pixelvalue = 0;
	register short tmp;

	tmp = rgbv->red;

	while (pf[2])
	{
		pixelvalue |= ((unsigned long)(tmp & (0xffff >> (16 - pf[0])))) << pf[1];
		tmp >>= pf[0];
		pf += 3;
	}
	pf += 3;
	tmp = rgbv->green;
	while (pf[2])
	{
		pixelvalue |= ((unsigned long)(tmp & (0xffff >> (16 - pf[0])))) << pf[1];
		tmp >>= pf[0];
		pf += 3;
	}
	pf += 3;
	tmp = rgbv->blue;
	while (pf[2])
	{
		pixelvalue |= ((unsigned long)(tmp & (0xffff >> (16 - pf[0])))) << pf[1];
		tmp >>= pf[0];
		pf += 3;
	}

	return pixelvalue;
}

