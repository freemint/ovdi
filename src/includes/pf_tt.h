#ifndef	_pf_tt_h
#define _pf_tt_h

/* TT palette register layout */
char pf_tt[] =
{
	/* red bits */
	  4,   8,  1,
	 12, 255,  0,

	/* green bits */
	  4,   4,  1,
	 12, 255,  0,

	/* blue bits */
	  4,   0,  1,
	 12, 255,  0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,
	/* Unused bits */
	  5,  11,  1,
	 27, 255,  0,
	  0
};

#endif	/* _pf_tt_h */