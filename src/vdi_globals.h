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

/* ========  sysfonts.c  ============ */
extern short	sysfnt_minwchar;
extern short	sysfnt_maxwchar;
extern short	sysfnt_minhchar;
extern short	sysfnt_maxhchar;
extern short	sysfnt_faces;

extern	struct xgdf_head *sysfnt08p;
extern	struct xgdf_head *sysfnt09p;
extern	struct xgdf_head *sysfnt10p;

extern	struct xgdf_head xsystemfont08;
extern	struct xgdf_head xsystemfont09;
extern	struct xgdf_head xsystemfont10;

extern short systemfont08[];
extern short systemfont09[];
extern short systemfont10[];

/* ========= ovdi.c ============== */
extern	char bootdev;
extern	short MiNT;
extern	short scrsizmm_x;
extern	short scrsizmm_y;
extern	LINEA_VARTAB *linea_vars;
extern	PatAttr WhiteRect;
extern	PatAttr BlackRect;
extern	struct pattern_data WRdata;
extern	struct pattern_data BRdata;

extern	char *vdi_fontlist;
extern	struct gdf_membuff loaded_vdi_gdfs;
extern	char gdf_path[];
extern	char confnt_name[];
extern	char sysf08_name[];
extern	char sysf09_name[];
extern	char sysf10_name[];

/* =============  tables.h ============== */
extern	OVDI_VTAB v_vtab[];
extern	DEV_TAB	DEV_TAB_rom;
extern	INQ_TAB	INQ_TAB_rom;
extern	SIZ_TAB SIZ_TAB_rom;
extern	short Planes2Pens[];
extern	short Planes2xinc[];
extern	short VDI2HW_colorindex[];
extern	short HW2VDI_colorindex[];
extern	signed char *marker[];

/* ========== styles.h ============== */
extern	unsigned short LINE_STYLE[];
extern	short SOLID;

/* things in styles.h */


#endif	/* _VDI_GLOBALS_H */
