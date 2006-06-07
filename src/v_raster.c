#include "display.h"
#include "colors.h"
#include "display.h"
#include "rasters.h"
#include "ovdi_defs.h"
#include "vdi_defs.h"
#include "v_raster.h"
#include "vdi_globals.h"
#include "patch_gem.h"

extern int logit;

void
vro_cpyfm( VDIPB *pb, VIRTUAL *v)
{
	int i;
	MFDB *s, *d;
	RASTER *r = v->raster;
	short p[8];

	for (i = 0; i < 8; i++)
		p[i] = pb->ptsin[i];

	s = *(MFDB **)&(pb->contrl[7]);
	d = *(MFDB **)&(pb->contrl[9]);
	RO_CPYFM( r, s, d, (short *)&p, v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1, pb->intin[0]);
}

void
vrt_cpyfm( VDIPB *pb, VIRTUAL *v)
{
	int i;
	MFDB *s, *d;
	RASTER *r = v->raster;
	int fgc, bgc, wrmode;
	short p[8];

	for (i = 0; i < 8; i++)
		p[i] = pb->ptsin[i];

	wrmode = pb->intin[0] - 1;

	fgc = v->colinf->color_vdi2hw[pb->intin[1]];
	bgc = v->colinf->color_vdi2hw[pb->intin[2]];

	s = *(MFDB **)&(pb->contrl[7]);
	d = *(MFDB **)&(pb->contrl[9]);
	RT_CPYFM( r, v->colinf, s, d, (short *)&p, v->clip.flag ? (VDIRECT *)&v->clip.x1 : (VDIRECT *)&r->x1, fgc, bgc, wrmode);
}

void
vr_trnfm( VDIPB *pb, VIRTUAL *v)
{
	MFDB *s, *d;

	s = *(MFDB **)&(pb->contrl[7]);
	d = *(MFDB **)&(pb->contrl[9]);
	//trnfm( v, s, d);
	if (!MiNT && !(v->flags & V_OSBM))
		patch_gem(v->raster->res.planes, v->raster->w - 1);
	trnfm(s, d);
}

void
v_get_pixel( VDIPB *pb, VIRTUAL *v)
{
	int planes = v->driver->r.res.planes;
	RASTER *r = v->raster;
	unsigned long pixel;

	pixel = (*r->drawers->get_pixel)(r->base, r->bypl, pb->ptsin[0], pb->ptsin[1]);

	if (planes > 8)
	{
		short red, green, blue;

		red = get_color_bits(r->res.pixelformat, pixel, 0);
		green = get_color_bits(r->res.pixelformat, pixel, 1);
		blue = get_color_bits(r->res.pixelformat, pixel, 2);

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
}
