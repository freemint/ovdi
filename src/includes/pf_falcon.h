#ifndef _pf_falcon_h
#define _pf_falcon_h

/* Falcon palette register layout */
char pf_falcon[] =
{
	/* red bits */
	  6,  26,  1,
	 10, 255,  0,

	/* green bits */
	  6,  18,  1,
	 10, 255,  0,

	/* blue bits */
	  6,   2,  1,
	 10, 255,  0,

	/* Alpha channel bits */
	 32, 255,  0,
	/* Genlock/overlay bits */
	 32, 255,  0,

	/* Unused bits */
	  2,   0,  1,
	 10,   8,  1,
	  2,  24,  1,
	 18, 255,  0,

	  0
};

/* Falcon's 15-bit pixelformat bit layout */
char pf_15b_falc[] =
{
	  5,  11,   1,		/*  5 red bits start at bit 11. Add one to get to next red bit */
	 11, 255,   0,		/* 11 unused red bits followin the above red bits */

	  5,   6,   1,		/* 5 green bits start at bit 6 */
	 11, 255,   0,		/* 11 unused red bits */
#if 0
	  3,   8,   1,		/* 3 green bits start at bit 8 */
	  2,   6,   1,		/* 2 green bits start at bit 6 */
	 11, 255,   0,		/* 11 unused green bits */
#endif

	  5,   0,   1,		/* 5 blue bits start at bit 0 */
	 11, 255,   0,		/* 11 unused blue bits */

	 32, 255,   0,		/* 32 unused alpha channel bits */

	  1,   5,   1,		/* 1 overlay bit starting at bit 5 */
	 31, 255,   0,		/* 31 unused genlock/overlay bits */

	 32, 255,   0,

	 0			/* end of table */
};

#endif	/* _pf_falcon_h */
