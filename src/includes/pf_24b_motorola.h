#ifndef	_pf_24b_motorola_h
#define _pf_24b_motorola_h

/* 24-bit Motorola pixelformat layout */
char pf_24bM[] =
{
	/* red bits */
	  8,  16,  1,
	  8, 255,  0,

	/* green bits */
	  8,   8,  1,
	  8, 255,  0,

	/* blue bits */
	  8,   0,  1,
	  8, 255,  0,

	/* Alpha + genlock/overlay bits */
	 64, 255,  0,
	/* Unused bits */
	 32, 255,  0,

	  0
};

#endif	/* _pf_24b_motorola_h */
