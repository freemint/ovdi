#include "display.h"
#include "colors.h"
#include "draw.h"
#include "vdi_defs.h"
#include "ovdi_defs.h"
#include "vdi_globals.h"
#include "v_attribs.h"
#include "v_fill.h"
#include "v_line.h"
#include "v_perimeter.h"
#include "v_text.h"

#define MIN_WRMODE  1;

void
vswr_mode( VDIPB *pb, VIRTUAL *v)
{
	short wrmode;
	wrmode = pb->intin[0];
	lvswr_mode( v, wrmode);
	lvsf_wrmode( v, wrmode);		/* Fill writing mode */
	lvsudf_wrmode( v, wrmode);	/* User defined fill writing mode */
	lvsl_wrmode( v, wrmode);		/* line writing mode */
	lvsprm_wrmode( v, wrmode);	/* perimeter writing mode */
	lvst_wrmode( v, wrmode);		/* Text writing mode */
	pb->intout[0] = v->wrmode + 1;
}

void
lvswr_mode( VIRTUAL *v, short wrmode)
{
	set_writingmode( wrmode, &v->wrmode);
}

void
set_writingmode( short wrmode, short *ret)
{
	if (wrmode < MIN_MD_MODE)
		wrmode = MIN_MD_MODE;
	else if (wrmode > MAX_MD_MODE)
		wrmode = MAX_MD_MODE;

	if (ret)
		*ret = wrmode - 1;

}

void
vs_clip( VDIPB *pb, VIRTUAL *v)
{
	lvs_clip( v, pb->intin[0], (short *)&pb->ptsin[0]);
}
void
lvs_clip( VIRTUAL *v, short flag, short *rclip)
{
	RASTER *r = v->raster;
	CLIPRECT *clip = &v->clip;

	if (flag == 1)
	{

		sortcpy_corners( rclip, (short *)&clip->x1);

		if (clip->x1 < 0)
			clip->x1 = 0;
		else if (clip->x1 > r->w - 1)
			clip->x1 = r->w - 1;
	
		if (clip->y1 < 0)
			clip->y1 = 0;
		else if (clip->y1 > r->h - 1)
			clip->y1 = r->h - 1;

		if (clip->x2 < 0)
			clip->x2 = 0;
		else if (clip->x2 > r->w - 1)
			clip->x2 = r->w - 1;

		if (clip->y2 < 0)
			clip->y2 = 0;
		else if (clip->y2 > r->h - 1)
			clip->y2 = r->h - 1;

		clip->flag = 1;
	}
	else
	{
		clip->x1 = clip->y1 = clip->flag = 0;
		clip->x2 = r->x2;
		clip->y2 = r->y2;
	}
}

void
vs_color( VDIPB *pb, VIRTUAL *v)
{
	RGB_LIST color;
	short cval, i;
	short *p;

	p = (short *)&color;

	for (i = 1; i < 5; i++)
	{
		cval = pb->intin[i];
		if (cval < 0)
			cval = 0;
		else if (cval > 1000)
			cval = 1000;

		*p++ = cval;
	}

	color.alpha	= 0;
	color.ovl	= 0;

	i = calc_vdicolor( v->raster, v->colinf, pb->intin[0], &color);
	if (i != -1)
		lvs_color( v, i, &v->colinf->actual_rgb[i]);

	/* should this not return anything? */
}

short
calc_vdicolor( RASTER *r, COLINF *c, short vdipen, RGB_LIST *color)
{
	short hwpen;

	if (vdipen >= c->pens || vdipen < 0)
		return -1;

	hwpen = c->color_vdi2hw[vdipen];

	c->request_rgb[hwpen] = *color;

	reqrgb_2_actrgb( r->res.pixelformat,
			 &r->rgb_levels,
			&c->request_rgb[hwpen],
			&c->actual_rgb[hwpen],
			&c->pixelvalues[hwpen]);

	return hwpen;
}

void
lvs_color( VIRTUAL *v, short hwpen, RGB_LIST *color)
{
	if (v->driver == v->physical && v->raster->res.clut && !(v->flags & V_OSBM))
		(*v->driver->dev->setcol)(v->driver, hwpen, color);

}

void
vq_color( VDIPB *pb, VIRTUAL *v)
{
	short vdipen, hwpen, flag;
	COLINF *c;
	RASTER *r;

	r = v->raster;
	c = v->colinf;

	vdipen = pb->intin[0];

	if (vdipen >= c->pens || vdipen < 0)
	{
exit:
		pb->intout[0] = - 1;
		return;
	}

	hwpen = c->color_vdi2hw[vdipen];
	flag = pb->intin[1];

	if (flag == COLOR_REQUESTED)
	{
		pb->intout[1] = c->request_rgb[hwpen].red;
		pb->intout[2] = c->request_rgb[hwpen].green;
		pb->intout[3] = c->request_rgb[hwpen].blue;
	}
	else if (flag == COLOR_ACTUAL)
	{
		RGB_LIST color;

		get_rgb_relatives( &c->actual_rgb[hwpen], &r->rgb_levels, &color);
		pb->intout[1] = color.red;
		pb->intout[2] = color.green;
		pb->intout[3] = color.blue;
	}
	else
		goto exit;

	pb->intout[0] = vdipen;
}
