#ifndef _pf_32b_motorola_h
#define _pf_32b_motorola_h

/* 32-bit Motorola pixelformat layout */
char pf_32bM[] = 
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
	/* Alpha bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,
	/* Unused bits */
	  8,  24,  1,
	 24, 255,  0,

	  0
};

#endif	/* _pf_32b_motorola_h */
