#ifndef	_VDI_GLOBALS_H
#define _VDI_GLOBALS_H

#include "gdf_defs.h"
#include "vdi_defs.h"
#include "linea.h"

struct cookie
{
	long tag;
	long value;
};
typedef	struct cookie COOKIE;

#ifdef	CJAR
#undef	CJAR
#endif
#define CJAR	((struct cookie **)0x5A0L);

extern	struct xgdf_head *sysfnt08p;
extern	struct xgdf_head *sysfnt09p;
extern	struct xgdf_head *sysfnt10p;

extern	struct xgdf_head xsystemfont08;
extern	struct xgdf_head xsystemfont09;
extern	struct xgdf_head xsystemfont10;

extern	char bootdev;
extern	short MiNT;

extern	short scrsizmm_x;
extern	short scrsizmm_y;

extern	LINEA_VARTAB *linea_vars;
extern	LINEA_VARTAB la_vt;
extern	DEV_TAB	DEV_TAB_rom;
extern	INQ_TAB	INQ_TAB_rom;
extern	SIZ_TAB SIZ_TAB_rom;
extern	OVDI_VTAB v_vtab[];
extern	VIRTUAL wks1;
extern	VIRTUAL	la_wks;

extern	RGB_LIST rgb_levels;
extern	RGB_LIST rgb_bits;
extern	long realpixels[];
extern	RGB_LIST req_pens[];
extern	RGB_LIST act_pens[];

extern	unsigned short LINE_STYLE[];

extern short Planes2Pens[];
extern short Planes2xinc[];
extern	short VDI2HW_colorindex[];
extern	short HW2VDI_colorindex[];
extern	char ICB_MouseAccTab[12];
extern	short CalibColTab[18];

extern	PatAttr WhiteRect;
extern	PatAttr BlackRect;

extern	signed char *marker[];

extern	long old_trap2_vec;
extern	void New_Trap2(void);
extern	int  get_cookie(long tag, long *ret);
extern	unsigned long Ssuper(unsigned long stack);

/* things in styles.h */

extern short SOLID;

#endif	/* _VDI_GLOBALS_H */