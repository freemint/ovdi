#ifndef	_OVDI_PARSER_H
#define _OVDI_PARSER_H

/*============================================================================*/
/* The  parser's  item  table contains for every keyword (comand/variable) one
 * entry  with  a key string, a type tag and a pointer to a callback. The type
 * tags  starts  all  with 'PI_' but differs then in 'C_' for comands and 'V_'
 * and  ' R_'  for variable equations. While 'R_' means that the ptr reffers a
 * variable  that  will be written directly, the other both means that the ptr
 * refers a callback.
 * The  following part (e.g. 'L', 'TA') are the same as of the interface types
 * (see above) and has to correspond with in each table entry (expect of 'x').
 *
 * These tag types are defined:
 */
typedef enum
{
#define PI_C__     0x0000 /*--- comand callbacks */
        PI_C_L   = 0x0002,   /* comand gets long                      */
        PI_C_B   = 0x0003,   /* comand gets bool                      */
        PI_C_T   = 0x0004,   /* comand gets path (or token) e.g. cd   */
        PI_C_TT  = 0x0044,   /* comand gets two pathes                */
        PI_C_0TT = 0x0144,   /* comand gets zero and two pathes       */
        PI_C_A   = 0x0005,   /* comand gets line as string, e.g. echo */
        PI_C_TA  = 0x0054,   /* comand gets path and line, e.g. exec  */
        PI_C_D   = 0x0006,   /* comand gets drive list                */
#define PI_V__     0x1000 /*--- variable callbacks */
        PI_V_L   = 0x1002,   /* variable gets long                   */
        PI_V_B   = 0x1003,   /* variable gets bool                   */
        PI_V_T   = 0x1004,   /* variable gets path                   */
        PI_V_A   = 0x1005,   /* variable gets line                   */
        PI_V_ATK = 0x1254,   /* variable gets path, line plus const. */
        PI_V_D   = 0x1006,   /* variable gets drive list             */
	PI_V_l   = 0x1007,   /* variable gets line, then a newline-separated list */
#define PI_R__   = 0x3000 /*--- references */
        PI_R_S   = 0x3001,   /* reference gets short */
        PI_R_L   = 0x3002,   /* reference gets long  */
        PI_R_B   = 0x3003,   /* reference gets bool  */
} PITYPE;

struct parser_item
{	char	*key;
	PITYPE	type;
	void	*cb;
	long	dat;
};

#endif	/* _OVDI_PARSER_H */
