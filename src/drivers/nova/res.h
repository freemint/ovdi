#ifndef __RES
#define __RES

/* Auflîsungsstruktur */
/* LÑnge der Struktur: 86 Bytes */

struct resolution
{
	char	name[33];	/* Name der Auflîsung */
				/* FÅr Nicht-C-Programmierer: */
				/* der Offset der nÑchsten Variablen */
				/* zum Strukturstart betrÑgt 34 Bytes */
	short	mode;		/* Auflîsungsart (siehe ICB.H) */
	short	bypl;		/* Bytes pro Zeile */
	short	planes;		/* Anzahl Planes */
	unsigned short	colors;	/* Anzahl Farben */
	short	hc_mode;	/* Hardcopy-Modus */
	short	max_x;		/* maximale x- und y-koordinate */
	short	max_y;
	short	real_x;		/* reale max. x- und u-koordinate */
	short	real_y;
				/* folgende Variablen sollten nicht */
				/* beachtet werden */
	short	freq;		/* Frequenz in MHz */
	char	freq2;		/* 2. Frequenz (SIGMA Legend II) */
	char	low_res;	/* halbe Pixelrate */
	char	r_3c2;		/* Register 3c2 */
	char	r_3d4[25];	/* Register 3d4, Index 0 bis $18 */
	char	extended[3];	/* Register 3d4, Index $33 bis $35 */
};
typedef struct resolution RESOLUTION;

#endif	/* __RES */