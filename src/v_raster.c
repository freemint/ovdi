#include "display.h"
#include "colors.h"
#include "display.h"
#include "rasters.h"
#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "vdi_defs.h"
#include "v_raster.h"
#include "vdi_globals.h"

void
vro_cpyfm( VDIPB *pb, VIRTUAL *v)
{
	MFDB *s, *d;
	RASTER *r = v->raster;

	s = (MFDB *)((((unsigned long)pb->contrl[7]) << 16) | (unsigned short)pb->contrl[8]);
	d = (MFDB *)((((unsigned long)pb->contrl[9]) << 16) | (unsigned short)pb->contrl[10]);
	ro_cpyfm( r, s, d, (short *)&pb->ptsin[0], v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1, pb->intin[0]);
	return;
}

void
vrt_cpyfm( VDIPB *pb, VIRTUAL *v)
{
	MFDB *s, *d;
	RASTER *r = v->raster;
	short fgc, bgc, wrmode;

	wrmode = pb->intin[0] - 1;

	fgc = v->colinf->color_vdi2hw[pb->intin[1]];
	bgc = v->colinf->color_vdi2hw[pb->intin[2]];

	s = (MFDB *)((((unsigned long)pb->contrl[7]) << 16) | (unsigned short)pb->contrl[8]);
	d = (MFDB *)((((unsigned long)pb->contrl[9]) << 16) | (unsigned short)pb->contrl[10]);
	rt_cpyfm( r, v->colinf, s, d, (short *)&pb->ptsin[0], v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1, fgc, bgc, wrmode);
	return;
}

void
vr_trnfm( VDIPB *pb, VIRTUAL *v)
{
	MFDB *s, *d;

	s = (MFDB *)(unsigned long)(((unsigned long)pb->contrl[7] << 16) | (unsigned short)pb->contrl[8]);
	d = (MFDB *)(unsigned long)(((unsigned long)pb->contrl[9] << 16) | (unsigned short)pb->contrl[10]);
	trnfm( v, s, d);
	return;
}

void
v_get_pixel( VDIPB *pb, VIRTUAL *v)
{
	short planes = v->driver->r.planes;
	RASTER *r = v->raster;
	unsigned long pixel;

	pixel = (*r->drawers->get_pixel)(r->base, r->bypl, pb->ptsin[0], pb->ptsin[1]);

	if (planes > 8)
	{
		short red, green, blue;

		red = get_color_bits(r->pixelformat, pixel, 0);
		green = get_color_bits(r->pixelformat, pixel, 1);
		blue = get_color_bits(r->pixelformat, pixel, 2);

		if (planes > 16)
		{
			pb->intout[1] = red & 0xff;
			pb->intout[0] = ((green & 0xff) << 8) | (blue & 0xff);
		}
		else
		{
			pb->intout[0] = 0;
			pb->intout[1] = ((red & 0x1f) << (16-5)) | ((green & 0x3f) << (16-5-6)) | (blue &0x1f);
		}
	}
	else
	{
		pb->intout[0] = (unsigned short)pixel;
		pb->intout[1] = v->colinf->color_hw2vdi[(unsigned short)pixel];
	}

	pb->contrl[N_INTOUT] = 2;
	return;
}
