/*
 * This file belongs to oVDI. 
 *
 * This file is based on cnf.c from FreeMiNT source tree, and its original
 * author is Ralph Lowinski <ralph@aquaplan.de>. The file have been
 * heavily modified by Odd Skancke <ozk@atari.org> to fit the needs in oVDI
 * project.
 *
 * Copyright 1999 Ralph Lowinski <ralph@aquaplan.de>
 * Copyright 2004 Odd Skancke <ozk@atari.org>
 * All rights reserved.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Author: Odd Skancke <ozk@atari.org>
 * Started: 2004-01-01
 *
 * please send suggestions, patches or bug reports to me.
 *
 *
 * changes since last version:
 *
 * - initial revision
 *
 * known bugs:
 *
 * -
 *
 * todo:
 *
 *
 */

#include <osbind.h>
#include <fcntl.h>

#include "cnf.h"
#include "display.h"
#include "file.h"
#include "global.h"

#include "libkern.h"

#include "memory.h"

const char *MSG_cnf_tos_error = "TOS error %ld while looking for '%s'";

const char *MSG_cnf_attempt_load = "While attempting to load config file '%s': ";
const char *MSG_cnf_file_not_found = "file not found";
const char *MSG_cnf_error_executing = "error while attempting to execute";
const char *MSG_cnf_cannot_include = "cannot open include file '%s'";
const char *MSG_cnf_invalid_arg = "invalid argument line for 'set', skipped.";
const char *MSG_cnf_set_option = "option '-%c'";
const char *MSG_cnf_set_ignored = " ignored.";
const char *MSG_cnf_parser_skipped = ", skipped.";
const char *MSG_cnf_empty_file = "[%s] is an empty file, skipped.";
const char *MSG_cnf_cant_allocate = "[%s] can't allocate %ld bytes, break.";
const char *MSG_cnf_no_memory = "cannot allocate %ld bytes, break.";
const char *MSG_cnf_reading_mintcnf = "\r\nReading `%s' ... ";
const char *MSG_cnf_not_successful = "not successful, break.\r\n\r\n";
const char *MSG_cnf_bytes_done = "%ld bytes done.\r\n";
const char *MSG_cnf_unknown_variable = "unknown variable";
const char *MSG_cnf_syntax_error = "syntax error";
const char *MSG_cnf_keyword_not_supported = "keyword '%s' not supported yet";
const char *MSG_cnf_needs_equation = "variable '%s' needs equation";
const char *MSG_cnf_argument_for = "argument %d for '%s' ";
const char *MSG_cnf_missed = "missed";
const char *MSG_cnf_must_be_a_num = "must be a number";
const char *MSG_cnf_out_of_range = "out of range";
const char *MSG_cnf_must_be_a_bool = "must be of type boolean (y/n)";
const char *MSG_cnf_missing_quotation = "missing quotation";
const char *MSG_cnf_junk = "junk at end of line ignored.";
const char *MSG_cnf_unknown_tag = "!!! unknown tag type %04x for '%s'!!!\n";

const char *MSG_cnf_missing_endbracket = "missing end bracket\r\n";
const char *MSG_cnf_missing_begbracket = "missing start bracket\r\n";


#define NUM_DRIVES 32

typedef union
{
	int	_err;
	short	s;
	long	l;
	ulong	u;
	bool	b;
	char	*c;
}GENARG;

typedef struct
{
	int a, b;
}RANGE;

#define Range( a, b )   (((long)(a)<<16)|(int)(b))

typedef struct
{
	ulong opt;		/* options */
	/* */
	const char *file;	/* name of current cnf file   */
	int        line;	/* current line in file       */
	/* */
	char   *dst;		/* write to, NULL indicates an error */
	char   *src;		/* read from                         */
	/* */
	char *env_ptr;		/* temporary pointer into that environment for setenv */
	long env_len;		/* length of the environment */
}PARSINF;

#define SET(opt)     (1ul << ((opt) -'A'))

static const char *drv_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";
#define CHAR2DRV(c) ((int)(strrchr(drv_list, toupper((int)c & 0xff)) - drv_list))

static void parser	(long fh, PARSINF *inf, long f_size, struct parser_item *items);
static void parser_msg	(PARSINF *inf, const char *msg);

/*----------------------------------------------------------------------------*/
/*============================================================================*/
/* The parser uses callbacks to start actions. Using the same interfaces, both
 * comands  and  variable  assignments  differ  only  in the type entry in the
 * parser's   item  table  (see  below).   The  notation  of  the  interface's
 * declaration  names  starts  with  'PCB_',  followed by one ore more letters
 * for the argument type(s):
 * 'L' is a long integer, either decimal or hex, 'B' is a boolean containing 0
 * or 1,  ' D'  is  a comma  separated  drive list as bit mask, 'T' is a token
 * without  any  spaces  (e.g.  a path),  'A'  the  rest of the line including
 * possible spaces. 'A' can only be the last parameter.
 * Note  that  'T' and 'A' are the same parameter types but has been parsed in
 * different ways.
 * Types with a following 'x' get additional parser infornation.
 *
 * These callback types are available for the parser:
 */
typedef void	(PCB_L)   (long                                    );
typedef void	(PCB_Lx)  (long,                       PARSINF *   );
typedef void	(PCB_B)   (bool                                    );
typedef void	(PCB_Bx)  (bool,                       PARSINF *   );
typedef void	(PCB_D)   (ulong                                   );
typedef void	(PCB_Dx)  (ulong,                      PARSINF *   );
typedef void	(PCB_T)   (const char *                            );
typedef PCB_T	 PCB_A                                              ;
typedef void	(PCB_Tx)  (const char *,               PARSINF *   );
typedef PCB_Tx	 PCB_Ax                                             ;
typedef void	(PCB_TT)  (const char *, const char *              );
typedef PCB_TT	 PCB_TA                                             ;
typedef void	(PCB_TTx) (const char *, const char *, PARSINF *   );
typedef PCB_TTx	 PCB_TAx                                            ;
typedef void	(PCB_0TT) (int,          const char *, const char *);
typedef void	(PCB_ATK) (const char *, const char *, long        );

typedef void	(PCB_l)   (int, const char *);
typedef void	(PCB_lx)   (int, const char *, PARSINF *);

/* and */
# define _NOT_SUPPORTED_  NULL

/* PITYPE was here */

/* The item table, note the 'NULL' entry at the end. */

/*----------------------------------------------------------------------------*/

void _cdecl
load_config(char *fname, struct parser_item *items)
{
	PARSINF inf  = { 0ul, NULL, 1, NULL, NULL, NULL, 0ul };
	long fs, fh;

	fs = get_file_size( fname );

	if (fs < 0)
	{
		scrnlog( MSG_cnf_attempt_load, fname);
		scrnlog( MSG_cnf_file_not_found);
		scrnlog("\n");
		return;
	}

	fh = Fopen( fname, O_RDONLY);

	if (fh < 0)
	{
		scrnlog( MSG_cnf_tos_error, fh, fname);
		scrnlog("\n");
	}
	else
	{
		inf.file = fname;
		parser (fh, &inf, fs, items);
	}
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*============================================================================*/
static void
p_REFF (PITYPE type, void *reff, GENARG arg)
{
	if (type == PI_R_L)  *(long*)reff = arg.l;
	else                *(short*)reff = arg.s;
}
/*============================================================================*/
/* The parser itself and its functions ...
 */
/* () Brackets, [] square brackets, {} curly brackets */
#define NUL '\0'
#define SGL '\''
#define DBL '"'
#define EOL '\n'
#define ESC '\\'   /* not 0x1B !!! */
#define OBR '{'
#define CBR '}'
#define REM '#'
 
enum { ARG_MISS, ARG_NUMB, ARG_RANG, ARG_BOOL, ARG_QUOT, ARG_MOBR, ARG_MCBR, ARG_LEND };
/*----------------------------------------------------------------------------*/
static void
parser_msg (PARSINF *inf, const char *msg)
{
	if (inf)
	{
		scrnlog ("[%s:%d] ", inf->file, inf->line);
	}
	else if (!msg)
	{
		msg = MSG_cnf_parser_skipped;
	}
	if (msg)
	{
		scrnlog (msg);
		scrnlog ("\r\n");
	}
}

/*----------------------------------------------------------------------------*/
static void
parse_spaces (PARSINF *inf)
{
	char c;

	while (isspace (c = *(inf->src)) && c != '\n')
		inf->src++;
}
/*----------------------------------------------------------------------------*/
static void
parse_nextline (PARSINF *inf)
{
	while ( *inf->src && *inf->src != EOL )
		inf->src++;

	if (*inf->src == EOL)
	{
		inf->line++;
		inf->src++;
	}
}
/*----------------------------------------------------------------------------*/
static GENARG
parse_token (PARSINF *inf, bool upcase)
{
	char   *dst = inf->dst, *src = inf->src;
	char   delim = NUL;
	char   c;
	GENARG ret;

	ret.c = inf->dst;

	do
	{
		if ((c = *(src++)) == NUL)
		{
			--src; /* correct overread zero */

		} 
		else if (c == SGL || c == DBL)
		{
			if      (delim == NUL) { delim  = c;   continue; }
			else if (delim == c)   { delim  = NUL; continue; }

		}
		else if (c == ESC  && ( delim == DBL  || (delim == NUL  && (inf->opt & SET('C'))) ))
		{
			if (!*src || *src == EOL || (*src == '\r' && *(src+1) == EOL))
			{
				c = NUL;   /* leading backslash honestly ignored :-) */

			}
			else if (delim != SGL) switch ( (c = *(src++)) )
			{
				case 't': c = '\t';   break;
				case 'n': c = '\n';   break;
				case 'r': c = '\r';   break;
				case 'e': c = '\x1B'; break; /* special: for nicer output */
			}

		}
		else if (c == EOL  || (isspace(c) && delim == NUL))
		{
			if (c == EOL)
				--src; /* correct overread eol */
			c = NUL;
		}
		else if (c == CBR || c == OBR || c == ',')
		{
			c = NUL;
			--src;
		}
		else if (upcase)
		{
			c = toupper((int)c & 0xff);
		}
		*(dst++) = c;

	} while (c);

	if (src == inf->src)
	{
		dst     = NULL; /* ARG_MISS */
	}
	else if (delim != NUL)
	{
		ret._err = ARG_QUOT;
		dst      = NULL;
	}
	inf->dst = dst;
	inf->src = src;

	return ret;
}

/*----------------------------------------------------------------------------*/
static GENARG
parse_liststart(PARSINF *inf)
{
	char c;
	GENARG ret;

	ret.c = inf->dst;

	do
	{
		parse_spaces(inf);

		if ( (c = *(inf->src++)) == 0)
		{
			ret._err = ARG_MOBR;
			inf->src--;
			inf->dst = NULL;
		}
		else if (c == EOL)
		{
			parse_spaces(inf);
			if ( (c = *(inf->src++)) != OBR)
			{
				c = NUL;
				inf->dst = NULL;
				inf->src--;
				ret._err = ARG_MOBR;
			}
			else
			{
				inf->line++;
				c = NUL;
			}
		}	
		else if (c == OBR)
		{
			c = NUL;
		}
		else if (c == CBR)
		{
			c = NUL;
			inf->dst = NULL;
			inf->src--;
			ret._err = ARG_MOBR;
		}	
	} while (c);

	return ret;
}

/*----------------------------------------------------------------------------*/
static GENARG
parse_listentry(PARSINF *inf)
{
	char c;
	GENARG ret;

	do
	{
		parse_spaces(inf);

		if ( (c = *(inf->src++)) == 0)
		{
			inf->src--;
			inf->dst = 0;
			ret._err = ARG_MCBR;
		}
		else if (c == EOL)
		{
			inf->line++;
			continue;
		}
		else if (c == REM)
		{
			scrnlog("Skipping REM..\n");
			parse_nextline(inf);
			continue;
		}
		else if (c == OBR)
		{
			inf->src--;
			inf->dst = 0;
			ret._err = ARG_MCBR;
			c = NUL;
		}
		else if (c == CBR)
		{
			c = NUL;
			inf->dst = NULL;
			ret._err = ARG_LEND;
		}
		else
		{
			inf->src--;
			c = NUL;
		}
	} while (c);

	if (inf->dst)
	{
		ret = parse_token(inf, 0);
	}

	return ret;
}
	
/*----------------------------------------------------------------------------*/
static GENARG
parse_line (PARSINF *inf)
{
	GENARG ret;

	ret.c = inf->dst;

	while (*(inf->src) && *(inf->src) != EOL)
	{
		parse_token  (inf, false);
		parse_spaces (inf);
		if (*(inf->src))
			*(inf->dst -1) = ' ';
	}
	if (inf->dst == ret.c)
		inf->dst = ret.c +1;

	*(inf->dst -1) = NUL;

	return ret;
}

/*----------------------------------------------------------------------------*/
static GENARG
parse_drvlst (PARSINF *inf)
{
	GENARG ret;

	ret.u = 0;

	while (*(inf->src))
	{
		long drv = strchr (drv_list, toupper((int)*(inf->src) & 0xff)) - drv_list;
		if (drv < 0)
			break;
		if (drv >= NUM_DRIVES )
		{
			ret._err = ARG_RANG;
			break;
		}
		ret.l |= 1ul << drv;
		inf->src++;
		parse_spaces (inf);
		if (*(inf->src) != ',')
			break;
		inf->src++;
		parse_spaces (inf);
	}
	return ret;
}

/*----------------------------------------------------------------------------*/
static GENARG
parse_long (PARSINF *inf, RANGE *range)
{
	char   *src      = inf->src;
	int    sign      = 1;
	GENARG ret;ret.l = 0;

	if (*src == '-')
	{
		sign = -1;
		src++;
	}

	while (isdigit(*src))
		ret.l = (ret.l * 10) + (*(src++) - '0');

	if (src > inf->src)
	{
		if      (toupper((int)*src & 0xff) == 'K')
		{
			ret.l *= 1024l;
			src++;
		}
		else if (toupper((int)*src & 0xff) == 'M')
		{
			ret.l *= 1024l*1024;
			src++;
		}
	}

	*(inf->dst++) = '\0'; /* not really necessary */

	if (src == inf->src  || (*src && !isspace(*src)))
	{
		ret._err = ARG_NUMB;
		inf->dst = NULL;
	}
	else if (range  && (range->a > ret.l  ||  ret.l > range->b))
	{
		ret._err = ARG_RANG;
		inf->dst = NULL;
	}
	else
	{
		ret.l *= sign;
	}
	inf->src = src;

	return ret;
}

/*----------------------------------------------------------------------------*/
static GENARG
parse_short (PARSINF *inf, RANGE *range)
{
	RANGE  s_rng = {0x8000,0x7FFF};
	GENARG ret   = parse_long (inf, (range ? range : &s_rng));
	if (inf->dst)
		ret.s = (short)ret.l;

	return ret;
}

/*----------------------------------------------------------------------------*/
static GENARG
parse_bool (PARSINF *inf)
{
	char *tab[]      = { "\00","\11", "\0N","\1Y", "\0NO","\1YES",
	                     "\0OFF","\1ON", "\0FALSE","\1TRUE", NULL };
	char *token      = (parse_token (inf, true)).c;
	GENARG ret;

	ret.b = false;

	if (!inf->dst)
	{
		ret._err = ((GENARG)token)._err;
	}
	else
	{
		char **p = tab;
		while (*p && strcmp (&(*p)[1], token)) p++;
		if (!*p)
		{
			ret._err = ARG_BOOL;
			inf->dst = NULL;
		}
		else
		{
			ret.b    = ((*p)[0] != '\0');
		}
	}

	return ret;
}

/*----------------------------------------------------------------------------*/
static void
parser (long fh, PARSINF *inf, long f_size, struct parser_item *items)
{
	long b_len = f_size + 20;
	int  state = 1;	/* 0: skip, 1: key, 2: arg(s), 3: call */
	char *buf;

	if (f_size == 0)
	{
		if (!(inf->opt & SET ('Q')))
		{
			scrnlog(MSG_cnf_empty_file, inf->file);
		}
		return;
	}

	buf = omalloc(b_len + 20, 0);

	if (!buf)
	{
		scrnlog (MSG_cnf_cant_allocate, inf->file, b_len);
		return;
	}

	inf->src = buf + b_len - f_size - 1;

	if (!(inf->opt & SET ('Q')))
	{
		scrnlog (MSG_cnf_reading_mintcnf, inf->file);
	}

	if ( Fread(fh, f_size, inf->src) != f_size)
	{
		scrnlog (MSG_cnf_not_successful);
		free_mem (buf);
		return;
	}
	else
	{
		Fclose(fh);
		if (!(inf->opt & SET ('Q')))
			scrnlog ( MSG_cnf_bytes_done, f_size);
	}

	inf->src[f_size] = 0;

	while (*(inf->src))
	{
		const struct parser_item *item    = NULL;
		GENARG                   arg[2]   = { {0}, {0} };
		int                      arg_type = 0;
		int                      arg_num  = 0;

		if (state == (1))
		{   /*---------- process keyword */
			char c;

			/*--- (1.1) skip leading spaces and empty or comment lines */
			while ( (c = *(inf->src)) )
			{
				if      (c == '#')    while ((c = *(++inf->src)) && c != EOL);
				if      (c == EOL)    inf->line++;
				else if (!isspace(c)) break;
				inf->src++;
			}
			if (*(inf->src) == NUL) break;   /* <eof> */

			if (inf->opt & SET('V'))
			{
				char save, *end = inf->src;
				while (*end && *end != '\r' && *end != '\n')
					end++;
				save = *end;
				*end = NUL;
				scrnlog ("%s\r\n", inf->src);
				*end = save;
			}

			/*--- (1.2) now read the keyword */
			inf->dst = buf;
			while ((c = toupper((int)*(inf->src) & 0xff)) && c != '='  && !isspace(c))
			{
				*(inf->dst++) = c;
				inf->src++;
			}
			*(inf->dst++) = '\0';

			/*--- (1.3) find item */
			item = items;
			while (item->key && strcmp (buf, item->key))
				item++;

			if (!item->key)
			{   /*--- (1.3.1) keyword not found */
				parse_spaces (inf);   /* skip to next character */
				parser_msg (inf, NULL);
				scrnlog (*(inf->src) == '=' ? MSG_cnf_unknown_variable : MSG_cnf_syntax_error);
				scrnlog (" '%s'", buf);
				parser_msg (NULL,NULL);
				state = 0;

			}
			else if (!item->cb)
			{   /*--- (1.3.2) found, but not supported */
				parser_msg (inf, NULL);
				scrnlog(MSG_cnf_keyword_not_supported, item->key);
				parser_msg (NULL,NULL);
				state = 0;

			}
			else if (item->type & PI_V__)
			{   /*--- (1.3.3) check equation */
				parse_spaces (inf);   /* skip to '=' */

				if (*(inf->src) == '=')
					inf->src++;
			}
			if (state)
			{   /*----- (1.3.4) success */
				arg_type = item->type & 0x00FF;
				inf->dst = buf;
				state    = 2;
			}
		}

		while (state == (2))
		{   /*---------- process arg */
			RANGE *range = (item->dat ? (RANGE*)&item->dat : NULL);
			char  *start;

			parse_spaces (inf);   /*--- (2.1) skip leading spaces */

			start = inf->src;   /*--- (2.2) read argument */

			switch (arg_type & 0x0F)
			{
				case 0x01: arg[arg_num] = parse_short		(inf, range); break;
				case 0x02: arg[arg_num] = parse_long		(inf, range); break;
				case 0x03: arg[arg_num] = parse_bool		(inf);        break;
				case 0x04: arg[arg_num] = parse_token		(inf, false); break;
				case 0x05: arg[arg_num] = parse_line		(inf);        break;
				case 0x06: arg[arg_num] = parse_drvlst		(inf);        break;
				case 0x07: arg[arg_num] = parse_liststart	(inf);        break;
			}

			if (!inf->dst)
			{   /*--- (2.3) argument failure */
				const char *msg = MSG_cnf_missed;
				if (inf->src != start)
				{
					switch (arg[arg_num]._err)
					{
						case ARG_NUMB: msg = MSG_cnf_must_be_a_num;	break;
						case ARG_RANG: msg = MSG_cnf_out_of_range;	break;
						case ARG_BOOL: msg = MSG_cnf_must_be_a_bool;	break;
						case ARG_QUOT: msg = MSG_cnf_missing_quotation;	break;
						case ARG_MOBR: msg = MSG_cnf_missing_begbracket; break;
						case ARG_MCBR: msg = MSG_cnf_missing_endbracket; break;
					}
					parser_msg (inf, NULL);
					scrnlog (MSG_cnf_argument_for, arg_num +1, item->key);
					scrnlog (msg);
					parser_msg (NULL,NULL);
					state = 0;

				}
			}
			else
			{   /*----- (2.4) success */
				arg_num++;
				state = ((arg_type >>= 4) & 0x0F ? 2 : 3);
			}
		}

		if (state == (3))
		{   /*---------- handle the callback */

			union { void *_v;
				PCB_Lx *l;
				PCB_Bx *b;
				PCB_Dx *u;
				PCB_Tx *c;
			        PCB_TTx *cc;
				PCB_0TT *_cc;
				PCB_ATK *ccl;
				PCB_lx	*lst;
			} cb;
			int	itemtype;

			cb._v = item->cb;


			/*--- (3.1) check for following characters */

			itemtype = item->type & 0xf7ff;

			parse_spaces (inf);   /* skip following spaces */

			if (*(inf->src)  &&  *(inf->src) != '\n'  &&  *(inf->src) != '#' && itemtype != PI_V_l)
			{
				parser_msg (inf, MSG_cnf_junk);
				state = 0;
			}
			switch (itemtype) //item->type & 0xF7FF)
			{   /*--- (3.2) do the call */
				/* We allege that every callback can make use of the parser
				 * information and put it always on stack. If it is not necessary
				 * it will simply ignored.
				 */
					#define A0L arg[0].l
					#define A0B arg[0].b
					#define A0U arg[0].u
					#define A0C arg[0].c
					#define A1C arg[1].c
				case PI_C_L: case PI_V_L: (*cb.l  )(  A0L,             inf); break;
				case PI_C_B: case PI_V_B: (*cb.b  )(  A0B,             inf); break;
				case PI_C_D: case PI_V_D: (*cb.u  )(  A0U,             inf); break;
				case PI_C_T: case PI_V_T:
				case PI_C_A: case PI_V_A: (*cb.c  )(  A0C,             inf); break;
				case PI_C_TT:
				case PI_C_TA:             (*cb.cc )(  A0C,A1C,         inf); break;
				case PI_C_0TT:            (*cb._cc)(0,A0C,A1C             ); break;
				case PI_V_ATK:            (*cb.ccl)(  A0C,A1C,item->dat   ); break;

				case PI_V_l:
				{
					int num = 0;

					do
					{
						inf->dst = buf;
						arg[0] = parse_listentry(inf);
						if (inf->dst)
							(*cb.lst)(num, arg[0].c, inf);
						num++;
					} while (inf->dst);

					if (arg[0]._err != ARG_LEND)
					{
						const char *msg = 0;
						switch (arg[0]._err)
						{
							case ARG_MOBR: msg = MSG_cnf_missing_begbracket; break;
							case ARG_MCBR: msg = MSG_cnf_missing_endbracket; break;
						}
						if (msg)
						{
							parser_msg(inf, NULL);
							scrnlog (msg);
						}
						if (num)
							(*cb.lst)(-2, 0L, inf);
					}
					else if (num)
						(*cb.lst)(-1, 0L, inf);	/* Let cnf know no more entries are coming */

					state = 0;
					break;
				}
				case PI_R_S: case PI_R_L: case PI_R_B:
				                           p_REFF (item->type,cb._v,arg[0]); break;

				default: scrnlog(MSG_cnf_unknown_tag,
					            (int)item->type, item->key);
			}
			
			if (state)
			{   /*----- (3.3) success */
				state = 1;
			}
		}

		if (state == (0))
		{   /*---------- skip to end of line */

			while (*inf->src)
			{
				if (*(inf->src++) == '\n')
				{
					inf->line++;
					state = 1;
					break;
				}
			}
		}
	} /* end while */

	free_mem(buf);
}
