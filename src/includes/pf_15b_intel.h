#ifndef	_pf_15b_intel_h
#define _pf_15b_intel_h

/* 15-bit Intel pixelformat layout */
/* gggbbbbb.0rrrrrgg */
char pf_15bI[] =
{
	/* red bits */
	  5,   2,  1,
	 11, 255,  0,

	/* green bits */
	  3,  13,  1,
	  2,   0,  1,
	 11, 255,  0,

	/* blue bits */
	  5,   8,  1,
	 11, 255,  0,

	/* Alpha bits */
	 32, 255,  0,

	/* Genlock/overlay bits */
	  1,   7,  1,
	 31, 255,  0,

	/* Unused bits */
	 32, 255,  0,

	  0
};

#endif	/* _pf_15b_intel_h */
