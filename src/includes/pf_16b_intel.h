#ifndef	_pf_16b_intel_h
#define _pf_16b_intel_h

/* 16-bit Intel pixelformat layout */
char pf_16bI[] =
{
	/* red bits */
	  5,   3,  1,
	 11, 255,  0,

	/* green bits */
	  3,  13,  1,
	  3,   0,  1,
	 10, 255,  0,

	/* blue bits */
	  5,   8,  1,
	 11, 255,  0,

	/* Alpha + genlock/overlay bits */
	 64, 255,  0,

	/* Unused bits */
	 32, 255,  0,

	  0
};

#endif	/* _pf_16b_intel_h */
