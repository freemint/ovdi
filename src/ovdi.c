#include <osbind.h>
#include <mint/mintbind.h>
#include <mint/basepage.h>
#include <mint/dcntl.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdio.h>

#include "cnf.h"
#include "console.h"
#include "display.h"

#include "file.h"

#include "fonts.h"

#include "ovdi.h"
#include "ovdi_defs.h"
#include "ovdi_rasters.h"
#include "ovdi_lib.h"
#include "vdi_defs.h"
#include "vdi_globals.h"
#include "workstation.h"
#include "funcdef.h"
#include "libkern.h"

#include "memory.h"
#include "module.h"
#include "modinf.h"
#include "mousedrv.h"

#include "vbiapi.h"
#include "timerapi.h"
#include "kbdapi.h"

#include "std_driver.h"

/* #include "styles.h" */
#include "tables.h"	/* Contains the function jumptable, dev/siz/inq tabs amogst other things */

//#include "line.h"

#include "xbios.h"

extern	long old_trap2_vec;		/* in handler.s */
extern	void New_Trap2(void);		/* in handler.s */

static void loadparse_ovdi_cnf(void);

void v_nosys( VDIPB *, VIRTUAL *);

/*
 * This array of functions is passed to each module/driver's
 * init code. 
*/
static long (oSupexec)(void *func);
static OVDI_LIB ovdilib =
{
	get_cookie,
	memcpy,
	memset,
	bcopy,
	bzero,

	omalloc,
	free_mem,

	get_file_size,
	load_file,

	load_config,

	scrnlog,
	log,

	oSupexec,
};

short	MiNT = 0;
char	bootdev;

short	scrsizmm_x = 0;
short	scrsizmm_y = 0;

LINEA_VARTAB	*linea_vars;
OVDI_HWAPI	hw_api;

VIRTUAL		wks1;
VIRTUAL		la_wks;

PatAttr		WhiteRect;
PatAttr		BlackRect;

/* Keep a 'root' color info structure */
static COLINF colinf;
static short vdi2hw[256];
static short hw2vdi[256];
static RGB_LIST request_rgb[256];
static RGB_LIST actual_rgb[256];
static unsigned long pixelvalues[256];

extern BASEPAGE *_base;
char *vdi_fontlist = 0;
struct gdf_membuff loaded_vdi_gdfs = { 0, 0, 0 };

static char ovdi_cnf_file[]	= { "c:\\ovdi.cnf" };
static char module_path[128]	= { "c:\\ovdisys\\" };
static char tmp_file[20]	= { "c:\\ovdiboot.tmp" };

char gdf_path[128]	= { "c:\\gemsys\\" };
char confnt_name[16]	= { "\0" };
char sysf08_name[16]	= { "\0" };
char sysf09_name[16]	= { "\0" };
char sysf10_name[16]	= { "\0" };

long
ovdi_init(void)
{
	OVDI_HWAPI *hw = &hw_api;
	struct module_desc md;
	long lavt, lafr, laft;
	void (*init)(OVDI_LIB *, struct module_desc *);

	scrnlog("OVDI start adr %lx, ends at adr %lx\n", _base->p_tbase,
		_base->p_tbase + _base->p_tlen + _base->p_blen + _base->p_dlen);

	bzero(&hw_api, sizeof(OVDI_HWAPI));
	bzero(&wks1, sizeof(VIRTUAL));

	get_linea_addresses(&lavt, &lafr, &laft);
	lavt -= 910;
	linea_vars = (LINEA_VARTAB *)lavt;

	bootdev = Dgetdrv() + 'a';
	ovdi_cnf_file[0] = bootdev;
	module_path[0] = bootdev;
	gdf_path[0] = bootdev;
	tmp_file[0] = bootdev;

	/*
	 * Load and parse ovdi.cnf
	*/
	loadparse_ovdi_cnf();
	//return -1;

	{
		BASEPAGE *b;
		short olddrive;
		long err, r;
		struct _dta *odta;
		struct _dta ndta;

		odta = Fgetdta();
		Fsetdta(&ndta);
		olddrive = Dgetdrv();

		if (module_path[1] == ':')
		{
			(void)Dsetdrv( toupper(module_path[0]) - 'A');
			r = Dsetpath((char *)&module_path[2]);
		}
		else
			r = Dsetpath(module_path);

		if (!r)
			r = Fsfirst("*.vdi", 0);

		while (!r)
		{
			b = load_module(ndta.dta_name, &err);

			if (err == 0)
			{
				md.types = 0;
				(long)init	= (long)b->p_tbase;
				(*init)(&ovdilib, &md);
				if (md.types & D_VHW)
				{
					OVDI_DEVICE *d = md.vhw;
					d->nxtapi	= hw->device;
					hw->device	= d;
				}
				if (md.types & D_VBI)
				{
					VBIAPI *v = md.vbi;
					v->nxtapi	= hw->vbi;
					hw->vbi		= v;
				}
				if (md.types & D_PDV)
				{
					PDVAPI	*p = md.pdv;
					p->nxtapi	= hw->pointdev;
					hw->pointdev	= p;
				}
				if (md.types & D_KBD)
				{
					KBDAPI *k = md.kbd;
					k->nxtapi	= hw->keyboard;
					hw->keyboard	= k;
				}
				if (md.types & D_TIM)
				{
					TIMEAPI *t = md.tim;
					t->nxtapi	= hw->time;
					hw->time	= t;
				}

				scrnlog("Sucessfully loaded module %s %lx\n", ndta.dta_name, b->p_tbase);
			}
			else
				scrnlog("Failed to Load module %s\n", ndta.dta_name);

			r = Fsnext();
		}
		(void)Dsetdrv(olddrive);
		Fsetdta(odta);
	}

	if (!hw->device)
	{
		scrnlog("Could not load Graphics card driver!\n");
		(void)Cnecin();
		return -1;
	}

	if (!hw->vbi)
	{
		scrnlog("No VBI driver loaded! oVDI NOT installed!\n");
		(void)Cnecin();
		return -1;
	}
	if (!hw->pointdev)
	{
		scrnlog("No Pointing device driver loaded! oVDI NOT installed\n");
		(void)Cnecin();
		return -1;
	}
	if (!hw->keyboard)
	{
		scrnlog("No keybaord driver loaded! oVDI NOT installed\n");
		(void)Cnecin();
		return -1;
	}
	if (!hw->time)
	{
		scrnlog("No Time driver loaded! oVDI NOT installed!\n");
		(void)Cnecin();
		return -1;
	}

	/*
	 * Here we will check if several modules with the same service was
	 * loaded, and let user select what to use or somethings...
	*/

	/*
	 * Setup systemfont
	*/
	init_systemfonts(&SIZ_TAB_rom, &DEV_TAB_rom);

	scrnlog("\n\n oVDI: Hit a key to continue\n");
	(void)Cnecin();

/* ---------------------------- */
	{
		OVDI_DRIVER *drv;
		OVDI_DEVICE *dev;
		RASTER *r;
		COLINF *c;
		long  usp;

		/*
		 * Open the graphics device. This will make driver attempt to set a
		 * bootresolution. In anycase, raster is setup describing the screen.
		 * Open returns a OVDI_DRIVER pointer...
		*/
		dev = hw->device;
		drv = (*dev->open)(dev);
		if (!drv)
		{
			scrnlog("Could not open graphics device! oVDI not installed\n");
			return -1;
		}

		/*
		 * Use raster setup by driver.
		*/
		r = &drv->r;
		c = &colinf;

		hw->driver = drv;
		hw->colinf = c;
		(*hw->vbi->install)();				/* Install VBI basics */
		(*hw->keyboard->install)();			/* Install keyboard basics */
		(*hw->time->install)(linea_vars);		/* Install timer basics */
		hw->mouse	= init_mouse(hw, linea_vars);	/* Initialize mouse driver layer 1 */

		/*
		 * setup the root colinf structure for this raster.
		 * ALLOC MEM FOR THIS INSTEAD!
		*/
		c->color_vdi2hw = (short *)&vdi2hw;
		c->color_hw2vdi = (short *)&hw2vdi;
		c->pixelvalues = (unsigned long *)&pixelvalues;
		c->request_rgb = (RGB_LIST *)&request_rgb;
		c->actual_rgb = (RGB_LIST *)&actual_rgb;

		/*
		 * Setup vital stuff for the raster structure
		*/
		init_raster(drv, r);
		raster_reschange(r, c);

		/*
		 * Init the console driver
		*/
		hw->console	= init_console(hw, r, linea_vars);

		/*
		 * Make relevant changes to DEV/INQ and Line A variable tables..
		*/
		reschange_devtab(&DEV_TAB_rom, r);
		reschange_inqtab(&INQ_TAB_rom, r);
		init_linea_vartab(0, linea_vars);
		linea_reschange(linea_vars, r, c);

		/*
		 * Install some vectors ...
		*/
		set_linea_vector();
		install_console_handlers(hw->console);
		install_xbios();

		/*
		 * Need to go supervising now...
		*/
		usp = Super(1);
		if (!usp)
			usp = Super(0);
		else
			usp = 0;

		/*
		 * Install console-drivers 'blink-cursor' VBI function - no blinking without it ;-)
		 * Then enable VBI driver...
		*/
		(*hw->vbi->add_func)((unsigned long)hw->console->textcursor_blink, 25);
		(*hw->vbi->enable)();

		if (usp)
			Super(usp);

		/*
		 * Set console font, and enter the console....
		*/
		if (confnt_name[0])
			set_console_font(hw->console, (const char *)&gdf_path, (char *)&confnt_name);
		enter_console(hw->console);

		/*
		 * Now oVDI hopefully RULE this machine.. he he.
		*/
	}
/* ---------------------------- */

	/*
	 * Install VDI trap handler
	*/
	old_trap2_vec = (long) Setexc(0x22, New_Trap2);

	scrnlog("OVDI start adr %lx, ends at adr %lx\n", _base->p_tbase, _base->p_tbase + _base->p_tlen + _base->p_blen + _base->p_dlen);
	scrnlog("Linea vartab %lx, font ring %lx, func tab %lx", linea_vars, lafr, laft);

	return 0;
}

/* ====================================================================================== */
/* ====================================================================================== */
static void
cnf_getpath( const char *src, char *dst)
{
	char c;
	short slash = 0;

	while ( (c = *src++) )
	{
		if (c == '\\' || c == '/')
			slash = 1;
		else
			slash = 0;

		*dst++ = c;
	}

	if (!slash)
		*dst++ = '\\';

	*dst = 0;
}
/*
 * Set the path where oVDI looks for drivers.
*/
static void
cnf_modulepath(const char *path)
{
	char *dst = module_path;

	cnf_getpath( path, dst );

	//scrnlog("Modulepath set to '%s'\n", dst);
}
/*
 * Set gemdos fontpath, where we find the GDF (GemDos Fonts).
*/
static void
cnf_gdfpath(const char *path)
{
	char *dst = gdf_path;

	cnf_getpath( path, dst );

	//scrnlog("GDF path set to '%s'\n", dst);
}
/*
 * Set console font.
*/
static void
cnf_consolefnt(const char *line)
{
	char *dst = confnt_name;
	while (*line) *dst++ = *line++;

	//scrnlog("Console font = '%s'\n", confnt_name);
}
/*
 * Get systemfonts. We only need 3 arguments here,
 * any more arguments is an error.
*/
static void
cnf_sysfonts(short num, char *fname)
{
	char *dst = 0;

	switch (num)
	{
		case 0: dst = sysf08_name; break;
		case 1: dst = sysf09_name; break;
		case 2: dst = sysf10_name; break;
		case -1:
		case -2: break;
		default: scrnlog("Too many arguments to 'sys_fonts'!\n"); break;
	}
	if (dst)
		while (*fname) { *dst++ = *fname++; }
}
/*
 * Called for each fontname in a 'vdi_fonts' parameter.
 * This routine is called once for every font filename
 * given as argument to 'vdi_fonts' config variable.
 * Because we dont know how many arguments (fonts) is
 * listed, we store the list in a temp file, and when
 * we have all fonts, allocate memory to hold the list
 * and keep the font-list there.
*/
static void
cnf_vdifonts(short num, char *path)
{
	long fh = -1;

	/*
	 * If argument number == 0, we get the first argument (yeah, really),
	 * and we need to create the temp file
	*/
	if (num == 0)
		fh = Fcreate( (char *) &tmp_file, O_RDWR);
	/*
	 * Subsequent fonts, just open temp file ..
	*/
	else if (num > 0)
		fh = Fopen( (char *) &tmp_file, O_RDWR);
	/*
	 * if num == 1, there are no more arguments coming,
	 * and we allocate ram for the list, load file into that ram,
	 * delete temp file, make 'vdi_fontlist' point to list in ram,
	 * and yeah, we've got a fontlist!
	*/
	else if ( num == -1 )
	{
		long fs;
		char *mem;

		fs = get_file_size( (char *) &tmp_file );
		mem = 0;

		if (fs > 0)
		{
			mem = (char *)omalloc(fs + 2, 0);
			if (mem)
			{
				if ( load_file( (char *)&tmp_file, fs, mem) != fs)
				{
					scrnlog("error loading font-list, fs %ld\n", fs);
					free_mem(mem);
					mem = 0;
				}
				vdi_fontlist = mem;
			}
			else
				scrnlog("no memory??\n");
		}
		Fdelete( (char *)&tmp_file );
		if (mem)
			mem[fs + 1] = mem[fs + 2] = 0;

		//scrnlog("end of list %lx\n", vdi_fontlist);
	}
	/*
	 * When num == -2, the config parser did not find the end
	 * of the list. 
	*/
	else if ( num == -2 )
	{
		scrnlog("error - end of list not found\n");
	}

	if (fh > 0)
	{
		Fseek ( 0L, fh, SEEK_END );
		Fwrite ( fh, (long)strlen(path) + 1, path);
		Fclose (fh);
	}
}
/*
 * Set the font-cache size
*/
extern long gdf_cache_size;
static void
cnf_gdfcache(long size)
{
	if (gdf_cache_size)
		scrnlog("gdf_cache redefined to %ld, was %ld\n", size, gdf_cache_size);
	gdf_cache_size = size;
}
/*
 * Set the logfile
*/
static void
cnf_logfile(const char *file)
{
	set_log_file(file);
}

static struct parser_item parstab_ovdicnf[] =
{
	{ "MODULE_PATH",	PI_V_T,	cnf_modulepath	},
	{ "GDF_PATH",		PI_V_T, cnf_gdfpath	},
	{ "CON_FONT",		PI_V_A, cnf_consolefnt	},
	{ "SYS_FONTS",		PI_V_l, cnf_sysfonts	},
	{ "VDI_FONTS",		PI_V_l, cnf_vdifonts	},
	{ "GDF_CACHE",		PI_V_L, cnf_gdfcache	},
	{ "LOGFILE",		PI_V_T, cnf_logfile	},
	{ NULL }
};

static void
loadparse_ovdi_cnf(void)
{
	ovdi_cnf_file[0] = bootdev;
	load_config((char *)ovdi_cnf_file, &parstab_ovdicnf[0]);	
}

/* ====================================================================================== */
/* ====================================================================================== */
short logit = 0;
/*
 * The oVDI function dispatcher.
*/
long
oVDI( VDIPB *pb )
{
	short func;
	VDI_function *f;
	VIRTUAL *v;

	func = pb->contrl[FUNCTION];

	if (func == 241)	/* remove me! */
		func = 8;

	if (func > MAX_VDI_FUNCTIONS)
	{

#if 0
		v = v_vtab[pb->contrl[HANDLE]].v;
		if (v)
			log("Calling old trap 2, func %d: pid %d %s\n", func, v->pid, v->procname);
		else
			log("Calling old trap 2, func %d - ILLEGAL HANDLE\n", func);
		
#endif
		return -1L;
	}

	f = (VDI_function *)v_jmptab[func];

	/*
	 * Opening a physical is a special case
	*/
	if (func == OPEN_WORKSTATION)
	{
		if (hw_api.device)
		{
			v_opnwk(pb, &wks1, &la_wks, &hw_api);
			return 0L;
		}
		else
		{
			pb->contrl[HANDLE] = 0;
			return 0L;
		}
	}
	/*
	 * No special cases, get correct VIRTUAL and
	 * just call the function
	*/
	v = v_vtab[pb->contrl[HANDLE]].v;
	if (!v)
		return 0L;

	/*
	 * Clear the N_INTOUT and N_PTSOUT variables.
	 * Is this correct? 
	*/
	pb->contrl[N_PTSOUT] = 0;
	pb->contrl[N_INTOUT] = 0;

	v->func = func;		/* Store the actual VDI function */

#if 0
//	if ( !(strcmp("PROFILE2", v->procname)) )
	if ( (Kbshift(-1) & 0x1) )// && func == 121)
		logit = 1;
	else
		logit = 0;

	if (logit) // && func == 0)
	{

		//log("%s, func %d..", func);

		//log("func %d subfuc %d, %d, %d, %d, %d ..", func, pb->contrl[SUBFUNCTION],
		//	pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3]);

		log("%s (%d) - func %d, subfuc %d, ni %d, np %d\n",
			v->procname, v->pid, func, pb->contrl[SUBFUNCTION], pb->contrl[N_INTIN], pb->contrl[N_PTSIN]);
		log("  in %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d, %d, %d, %d, %d\n",
			pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3],
			pb->intin[4], pb->intin[5], pb->intin[6], pb->intin[7],
			pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
			pb->ptsin[4], pb->ptsin[5], pb->ptsin[6], pb->ptsin[7]);

		log("     %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d, %d, %d, %d, %d\n",
			pb->intout[0], pb->intout[1], pb->intout[2], pb->intout[3],
			pb->intout[4], pb->intout[5], pb->intout[6], pb->intout[7],
			pb->ptsout[0], pb->ptsout[1], pb->ptsout[2], pb->ptsout[3],
			pb->ptsout[4], pb->ptsout[5], pb->ptsout[6], pb->ptsout[7]);
	}

	(*f)(pb, v);

	if (logit)// && func == 0)
	{
		log(" out %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d, %d, %d, %d, %d\n",
			pb->intin[0], pb->intin[1], pb->intin[2], pb->intin[3],
			pb->intin[4], pb->intin[5], pb->intin[6], pb->intin[7],
			pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3],
			pb->ptsin[4], pb->ptsin[5], pb->ptsin[6], pb->ptsin[7]);

		log("     %d, %d, %d, %d, %d, %d, %d, %d - %d, %d, %d, %d, %d, %d, %d, %d\n",
			pb->intout[0], pb->intout[1], pb->intout[2], pb->intout[3],
			pb->intout[4], pb->intout[5], pb->intout[6], pb->intout[7],
			pb->ptsout[0], pb->ptsout[1], pb->ptsout[2], pb->ptsout[3],
			pb->ptsout[4], pb->ptsout[5], pb->ptsout[6], pb->ptsout[7]);

		log(" nio %d, npo %d - leave\n\n", pb->contrl[N_INTOUT], pb->contrl[N_PTSOUT]);
	//	log("leave\n\n");
	}

#else
	/*
	 * Call the VDI function
	*/
	(*f)(pb, v);
#endif
	return 0L;
}


int _cdecl
get_cookie(long tag, long *ret)
{
	COOKIE *jar;
	int r = 0;
	long usp;

	usp = Super(1);
	if (!usp)
		usp = Super(0L);
	else
		usp = 0L;

	jar = *CJAR;

	if (!jar)
		return r;

	while(jar->tag)
	{
		if (jar->tag == tag)
		{
			if (ret)
				*ret = jar->value;
			r = 1;
			break;
		}
		jar++;
	}

	if (usp)
		(void)Super(usp);

	return r;
}

/*
 * oVDI lib wrapper for Supexec
*/
static long
oSupexec(void *func)
{
	return Supexec(func);
}

void
v_nosys( VDIPB *pb, VIRTUAL *v)
{
	return;
}

