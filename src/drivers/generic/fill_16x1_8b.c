#include "ovdi_defs.h"
#include "8b_generic.h"

void
draw_solid_rect_8b(RASTER *r, COLINF *c, short *corners, short wrmode, short color)
{
	unsigned char *dst, *d;
	short i, bypl, dx, dy;
	unsigned long pixel;

	bypl = r->bypl;
	pixel = color & 0xff;
	pixel |= (pixel << 24) | (pixel << 16) | (pixel << 8);
	dx = corners[2] - corners[0] + 1;
	dy = corners[3] - corners[1] + 1;

	d = (unsigned char *)r->base + corners[0] + (long)corners[1] * bypl;

	SYNC_RASTER(r);

	switch (wrmode)
	{
		case 0: /* MD_REPLACE */
		case 1: /* MD_TRANS */
		{
			for (; dy > 0; dy--)
			{
				dst = d;

				for (i = dx >> 4; i > 0; i--)
				{
					*(unsigned long *)((unsigned long *)dst)++ = (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ = (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ = (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ = (long)pixel;
				}
				for (i = dx & 0xf; i > 0; i--)
					*(unsigned char *)((unsigned char *)dst)++ = (unsigned char)pixel;

				d += bypl;
			}
			break;
		}
		case 2: /* MD_EOR */
		{
			for (; dy > 0; dy--)
			{
				dst = d;

				for (i = dx >> 4; i > 0; i--)
				{
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
				}
				for (i = dx & 0xf; i > 0; i--)
					*(unsigned char *)((unsigned char *)dst)++ ^= (unsigned char)pixel;

				d += bypl;
			}
			break;
		}
		case 3:
		{
			for (; dy > 0; dy--)
			{
				dst = d;

				for (i = dx >> 4; i > 0; i--)
				{
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
					*(unsigned long *)((unsigned long *)dst)++ ^= (long)pixel;
				}
				for (i = dx & 0xf; i > 0; i--)
					*(unsigned char *)((unsigned char *)dst)++ ^= (unsigned char)pixel;

				d += bypl;
			}
			break;
		}
	}
}
