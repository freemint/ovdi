#ifndef	_pf_32b_intelbs_h
#define _pf_32b_intelbs_h

/* 32-bit byteswapped Intel pixelformat layout */
/* 00000000.bbbbbbbb.gggggggg.rrrrrrrr */
char pf_32bIbs[] =
{
	/* red bits */
	  8,   0, 1,
	  8, 255, 0,
	/* green bits */
	  8,   8, 1,
	  8, 255, 0,
	/* blue bits */
	  8,  16, 1,
	  8, 255, 0,
	/* Alpha bits */
	 32, 255, 0,
	/* genlock/overlay bits */
	 32, 255, 0,
	/* Unused bits */
	  8,  24, 1,
	 24, 255, 0,

	  0
};

#endif	/* _pf_32b_intelbs_h */
