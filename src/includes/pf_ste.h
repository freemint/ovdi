#ifndef _pf_ste_h
#define _pf_ste_h

/* STe palette register layout */
char pf_ste[] =
{
	/* red bits */
	  1,  11,  1,
	  3,   8,  1,
	 12, 255,  0,

	/* green bits */
	  1,   7,  1,
	  3,   4,  1,
	 12, 255,  0,

	/* blue bits */
	  1,   3,  1,
	  3,   0,  1,
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

#endif	/* _pf_ste_h */
