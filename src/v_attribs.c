#include "display.h"
#include "colors.h"
#include "draw.h"
#include "vdi_defs.h"
#include "ovdi_defs.h"
#include "vdi_globals.h"
#include "v_attribs.h"

#define MIN_WRMODE  1;

void
vswr_mode( VDIPB *pb, VIRTUAL *v)
{
	lvswr_mode( v, pb->intin[0]);
	pb->intout[0] = v->wrmode + 1;
	return;
}

void
lvswr_mode( VIRTUAL *v, short wrmode )
{
	if (wrmode < MIN_MD_MODE)
		v->wrmode = MIN_MD_MODE;
	else if (wrmode > MAX_MD_MODE)
		v->wrmode = MAX_MD_MODE;

	v->wrmode = wrmode - 1;
	return;
}

void
vs_clip( VDIPB *pb, VIRTUAL *v)
{
	lvs_clip( v, pb->intin[0], (VDIRECT *)&pb->ptsin[0]);
	return;
}
void
lvs_clip( VIRTUAL *v, short flag, VDIRECT *rclip)
{
	RASTER *r = v->raster;

	if (flag == 1)
	{
		VDIRECT clip;

		sortcpy_corners((short *)rclip, (short *)&clip);

		if (clip.x1 < 0)
			clip.x1 = 0;
		else if (clip.x1 > r->w - 1)
			clip.x1 = r->w - 1;
	
		if (clip.y1 < 0)
			clip.y1 = 0;
		else if (clip.y1 > r->h - 1)
			clip.y1 = r->h - 1;

		if (clip.x2 < 0)
			clip.x2 = 0;
		else if (clip.x2 > r->w - 1)
			clip.x2 = r->w - 1;

		if (clip.y2 < 0)
			clip.y2 = 0;
		else if (clip.y2 > r->h - 1)
			clip.y2 = r->h - 1;

		v->clip = clip;
		v->clip_flag = 1;
	}
	else
	{
		v->clip.x1 = v->clip.y1 = v->clip_flag = 0;
		v->clip.x2 = r->w - 1;
		v->clip.y2 = r->h - 1;
	}
	return;
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

//	log("vs_color: pid %d - %s; pen %d, red %d %d, green %d %d, blue %d %d\n", v->pid, v->procname, pb->intin[0],
//		pb->intin[1], color.red, pb->intin[2], color.green, pb->intin[3], color.blue);

	lvs_color( v, pb->intin[0], &color);

	/* should this not return anything? */
	return;
}

void
lvs_color( VIRTUAL *v, short vdipen, RGB_LIST *color)
{
	short hwpen;

	if (vdipen > (Planes2Pens[v->driver->r.planes] - 1))
		return;

	hwpen = v->color_vdi2hw[vdipen];

	v->request_rgb[hwpen] = *color;

	reqrgb_2_actrgb( v->driver->r.pixelformat,
			v->rgb_levels,
			&v->request_rgb[hwpen],
			&v->actual_rgb[hwpen],
			&v->driver->r.pixelvalues[hwpen]);

	if (v->driver == v->physical)
		(*v->driver->dev->setcol)(v->driver, hwpen, &v->actual_rgb[hwpen]);

	return;
}
void
vq_color( VDIPB *pb, VIRTUAL *v)
{
	short vdipen, hwpen, flag;

	//log("vq_color: pid %d - %s ",v->pid, v->procname);

	vdipen = pb->intin[0];

	if (vdipen >= (Planes2Pens[v->driver->r.planes]))
	{
exit:		//log(" out of range!\n");
		pb->intout[0] = - 1;
		return;
	}

	hwpen = v->color_vdi2hw[vdipen];
	flag = pb->intin[1];

	if (flag == COLOR_REQUESTED)
	{
		pb->intout[1] = v->request_rgb[hwpen].red;
		pb->intout[2] = v->request_rgb[hwpen].green;
		pb->intout[3] = v->request_rgb[hwpen].blue;
		//log(" req pen %d, red %d, green %d, blue %d\n", pb->intin[0], pb->intout[1], pb->intout[2], pb->intout[3]);
	}
	else if (flag == COLOR_ACTUAL)
	{
		RGB_LIST color;

		get_rgb_relatives( &v->actual_rgb[hwpen], v->rgb_levels, &color);
		pb->intout[1] = color.red;
		pb->intout[2] = color.green;
		pb->intout[3] = color.blue;
		//log(" act pen %d, red %d, green %d, blue %d\n", pb->intin[0], pb->intout[1], pb->intout[2], pb->intout[3]);
	}
	else
		goto exit;

	pb->intout[0] = vdipen;
	return;
}
