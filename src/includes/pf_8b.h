#ifndef _pf_8b_h
#define _pf_8b_h

char pf_8b[] =
{
	  8,  16,   1,
	  8, 255,   0,

	  8,   8,   1,
	  8, 255,   0,

	  8,   0,   1,
	  8, 255,   0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,

	/* Unused bits */
	 32, 255,  0,

	  0
};

#endif /* _pf_8b_h */
