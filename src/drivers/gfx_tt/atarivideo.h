#ifndef	_atarivideo_h
#define _atarivideo_h

#define VMODE_LOW	0
#define VMODE_MED	1
#define VMODE_HI	2
#define VMODE_ST_MASK	3

#define VMODE_TT_MED	4
#define VMODE_TT_HI	6
#define VMODE_TT_LOW	7

#define VMODE_TT_MASK	7


#define VB_HI_REG		(0x00ff8201L)
#define VB_MI_REG		(0x00ff8203L)
#define VB_LO_REG		(0x00ff820dL)
#define VIDEOSYNC_REG		(0x00ff820aL)
#define LWIDTH_REG		(0x00ff820fL)
#define ST_VMODE_REG		(0x00ff8260L)
#define TT_VMODE_REG		(0x00ff8262L)
#define ST_PALETTE0_REG		(0x00ff8240L)
#define TT_PALETTE0_REG		(0x00ff8400L)

#define VB_HI		(*(volatile unsigned char *)VB_HI_REG)
#define VB_MI		(*(volatile unsigned char *)VB_MI_REG)
#define VB_LO		(*(volatile unsigned char *)VB_LO_REG)

#define VIDEOSYNC	(*(volatile unsigned char *)VIDEOSYNC_REG)


#define ST_VMODE 	(*(volatile unsigned char *)ST_VMODE_REG)
#define TT_VMODE  	(*(volatile unsigned short *)TT_VMODE_REG)
#define LWIDTH		(*(volatile unsigned char *)LWIDTH_REG)
#define ST_PALETTE0	(*(volatile unsigned short *)ST_PALETTE0_REG)
#define TT_PALETTE0	(*(volatile unsigned short *)TT_PALETTE0_REG)

#endif	/* _atarivideo_h */
