/***************************************************************
 * Aufbau der Kontrollstruktur fÅr NOVA-Grafikkarten		*
 * all rights reserved						*
  **************************************************************/

#ifndef __ICB
#define __ICB

#include	"res.h"
#include	"xcb.h"

/* alle Zeiger auf Routinen erwarten ihre Parameter im */
/* Turbo-/Pure-C-Format: WORD-Parameter in ihrer Reihenfolge in */
/* den Registern d0-d2, Zeiger in den Registern a0-a1 */
struct icb
{
							/* Versionsnummer in ASCII */
	long		id;				/* erst ab Version 1.37 */
							/* Zeiger auf Routine zur  */
							/* Auflîsungsumschaltung */
	void		(*p_chres)(RESOLUTION *res);
	short		mode;				/* Auflîsungstyp: */
							/* 0: 16 Farben */
							/* 1: 2 Farben */
							/* 2: 256 Farben */
							/* 3: 32768 Farben */
							/* 4: 64536 Farben */
							/* 5: 16.7 Mio. (24 Bit) */
	short		bypl;				/* Bytes pro Bildschirmzeile */
	short		planes;				/* Anzahl Bildschirmplanes: */
							/* 1, 4, 8, 16 oder 24*/
	short		colors;				/* Anzahl Farben (VDI-Wert) */
	short		hc;				/* Hardcopymodus: */
							/* 0: 1 Bildpxl. = 1x1 Druckpxl. */
							/* 1: 1 Bildpxl. = 2x2 Druckpxl. */
							/* 2: 1 Bildpxl. = 4x4 Druckpxl. */
	short		max_x, max_y;			/* Bildschirmauflîsung */
							/* fÅr virtuelle Bildschirm- */
							/* verwaltung: */
	short		rmn_x, rmx_x;			/* physikalisch auf dem Monitor */
	short		rmn_y, rmx_y;			/* dargestellter Bereich */

	unsigned char	*scr_base;			/* Zeiger auf physikalische */
							/* Bildschirmadresse */
	unsigned char	*reg_base;			/* Zeiger auf I/O-Adressen */
	char		name[36];			/* Name der aktuellen Auflîsung */

	long		bl_count;			/* ZÑhler fÅr Dunkelschaltung */
							/* Startwert fÅr ZÑhler in */
	long		bl_max;				/* 1/200stel Sekunden */
	short		bl_on;				/* != 0: Bildschirm ist dunkel */
	
	short		card;				/* Graphikkartentyp */

	unsigned char	speed_tab[12];			/* Maus-Beschleunigungstabelle */

							/* virtuelle Bildschirmverwaltung: */
							/* es wird getestet, ob der Punkt */
							/* (x;y) sich im dargestellten */
							/* Bildabschnitt befindet */
							/* falls nicht, wird der darge- */
							/* stellte Abschnitt so verschoben, */
							/* daû der Punkt gerade sichtbar */
							/* wird */
	void		(*chng_vrt)(short x, short y);
	short		boot_drv;			/* Laufwerk, von dem der Nova- */
							/* Treiber gestartet wurde */
							/* Hier befindet sich auch die */
							/* Auflîsungsdatei (im AUTO-Ordner) */

	long		(*set_vec)(void);		/* Ausgabevektoren neu setzen */
							/* muû im Supervisormodus auf- */
							/* gerufen werden */

	void		*main_work;			/* Zeiger auf 1. VDI-Wkst. */
	short		(*handler)(short x, short y);	/* Zeiger auf Funktionshandler */
							/* ebenfalls im Supervisormodus */
							/* aufrufen */
	void		(*init_col)(void);		/* Zeiger auf Farbinitialisierung */
	unsigned short	scrn_cnt;			/* Anzahl Bildschirme */
	long		scrn_siz;			/* Grîûe eines Bildschirms in Bytes */
	void		*base;				/* Adresse von Bildschirmseite 0 */
	void		(*p_setscr)(XCB *xcb, struct icb *icb, void *scradr);		/* Zeiger auf Routine, die */
							/* neue physikalische Bildschirm- */
							/* adresse setzt */
	short		v_top, v_bottom,		/* RÑnder fÅr virt. Speicherverwaltung */
			v_left, v_right;

/**/
	short		max_light;
	void		(*chng_pos)(RESOLUTION *res, short dir, short offset);
	void		(*chng_edg)(short *coords);

	short		hw_flags;
	short		calib_on;
	short		calib_col[6*3];
	void		(*init_colorcalib)(void);
	short		*calib_tab;
	short		*recalib_tab;
	
};

typedef struct icb ICB;

#endif	/* __ICB */
