#ifndef	_pf_st_h
#define _pf_st_h

/* ST palette register layout */
char pf_st[] =
{
	/* red bits */
	  3,   8,  1,
	 13, 255,  0,

	/* green bits */
	  3,   4,  1,
	 13, 255,  0,

	/* blue bits */
	  3,   0,  1,
	 13, 255,  0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,
	/* Unused bits */
	  1,   3,  1,
	  1,   7,  1,
	  5,  11,  1,
	 25, 255,  0,

	  0
};

#endif	/* _pf_st_h */
