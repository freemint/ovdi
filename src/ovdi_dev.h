#ifndef	_OVDI_DEV_H
#define	_OVDI_DEV_H

//#include "console.h"
#include "vdi_defs.h"
#include "ovdi_defs.h"
#include "linea.h"
#include "mouse.h"

typedef	struct ovdidrv_functab OVDIDRV_FUNCTAB;
typedef struct ovdi_driver OVDI_DRIVER;
typedef struct ovdi_device OVDI_DEVICE;

typedef	struct xmform XMFORM;
typedef struct xmsave XMSAVE;
typedef struct mousedrv MOUSEDRV;
typedef struct mouseapi MOUSEAPI;

//typedef void		(*draw_pixel)(unsigned char *adr, long data);
//typedef long		(*read_pixel)(unsigned char *adr);
//typedef void		(*put_pixel)(short x, short y, long data);
//typedef unsigned long	(*get_pixel)(unsigned char *scrbase, short bypl, short x, short y);
//typedef void		(*raster_op)(unsigned char *srcbase, short srcbypl, unsigned char *dstbase, short dstbypl, short width, short height, short dir);

//typedef void (*draw_mc)(register XMFORM *xmf, register short x, register short y);
//typedef void (*undraw_mc)(register XMSAVE *xms);

//typedef draw_pixel *drawpixel_functs[];
//typedef raster_op *raster_ops[];

struct ovdidrv_functab
{

	struct ovdi_drawers	*drawers_1b;
	struct ovdi_drawers	*drawers_2b;
	struct ovdi_drawers	*drawers_4b;
	struct ovdi_drawers	*drawers_8b;
	struct ovdi_drawers	*drawers_15b;
	struct ovdi_drawers	*drawers_16b;
	struct ovdi_drawers	*drawers_24b;
	struct ovdi_drawers	*drawers_32b;
	
	short		(*get_vbitics)(void);
	short		(*add_vbifunc)(unsigned long function, unsigned long tics);
	void		(*remove_vbifunc)(unsigned long function);
	void		(*reset_vbi)(void);
	void		(*enable_vbi)(void);
	void		(*disable_vbi)(void);

//	draw_mc draw_mc;
//	undraw_mc undraw_mc;

//	unsigned long	(*get_pixel)(unsigned char *base, short bypl, short x, short y);
//	void		(*put_pixel)(unsigned char *base, short bypl, short x, short y, unsigned long pixel);
	
//	draw_pixel *pixelfuncts;
//	draw_pixel *rt_functs;
//	raster_op *raster_operations;

	/*drawpixel_functs *pixelfuncts;*/
//	long	reservd[128-3];
	
};

/* Structure describing a raster - all drawing primitives get its info from */
/* this structure */
struct raster
{
	unsigned char	*base;
	short		flags;
	short		format;
	short		w, h;
	short		clut;
	short		planes;
	short		bypl;
	short		wpixel, hpixel;
	char		*pixelformat;
	unsigned long	*pixelvalues;
};
typedef struct raster RASTER;

struct ovdi_driver
{
	long		version;
	short		format;

#define PF_ATARI	1
#define PF_PLANES	2
#define PF_PACKED	4
#define	PF_FALCON	8
#define	PF_BE		16
#define	PF_BS		32

	RASTER		r;
	
	unsigned long	palette;
	short		v_top, v_bottom, v_left, v_right;

	void		*vram_start;
	long		vram_size;

	short		scr_count;
	long		scr_size;

	void		*log_base;

	OVDI_DEVICE	*dev;

	struct ovdi_drawers	*drawers_1b;
	struct ovdi_drawers	*drawers_2b;
	struct ovdi_drawers	*drawers_4b;
	struct ovdi_drawers	*drawers_8b;
	struct ovdi_drawers	*drawers_15b;
	struct ovdi_drawers	*drawers_16b;
	struct ovdi_drawers	*drawers_24b;
	struct ovdi_drawers	*drawers_32b;
	
/* If add_vbifunc is NULL, the built in vbi functions are copied into
 * these elements and used.
 * If driver wants to provide vbi functions, ALL the below functions
 * are considered to be valid if add_vbifunc is not NULL */
	short		(*add_vbifunc)(unsigned long function, unsigned long tics);
	short		(*get_vbitics)(void);
	void		(*remove_vbifunc)(unsigned long function);
	void		(*reset_vbi)(void);
	void		(*enable_vbi)(void);
	void		(*disable_vbi)(void);

};

struct ovdi_device
{
	long version;
	char *name;

	OVDI_DRIVER *	(*open)(struct ovdi_device *dev, short dev_id);
	long		(*close)(OVDI_DRIVER *drv);
	unsigned char *	(*setpscr)(OVDI_DRIVER *drv, unsigned char *scrnadr);
	unsigned char *	(*setlscr)(OVDI_DRIVER *drv, unsigned char *logscr);
	void		(*setcol)(OVDI_DRIVER *drv, short pen, RGB_LIST *colors);
	void		(*vsync)(OVDI_DRIVER *drv);

 	void		(*vreschk)(short x, short y);
};

OVDI_DEVICE *	device_init		(struct ovdi_lib *lib);


#endif	/* _OVDI_DEV_H */
