#ifndef _ET6K_ACL_H
#define _ET6K_ACL_H

#define MEMMAP_CTRL		0x40
#define PERFRM_CTRL		0x41
#define MCLK_CTRL		0x42
#define SCLK_CTRL		0x43
#define RASCAS_CNF		0x44
#define MEM_CNF			0x45
#define DISP_FEATURE		0x46
#define TEST_SETUP		0x47

/*
 * VESA DCC bus control
 */
#define VESA_DCC_CTRL		0x4e

/*
 * General purpose output register
 */
#define GPIO			0x4f

/*
 * Image port registers
 */
#define IMG_STRTADR_L		0x50
#define IMG_STRTADR_M		0x51
#define IMG_STRTADR_H		0x52
#define IMG_LEN_L		0x53
#define IMG_LEN_H		0x54
#define IMGROW_OFFSET_L		0x55
#define IMGROW_OFFSET_H		0x56
#define IMGPORT_CTRL		0x57

/*
 * Video control registers
 */
#define VIDEO_CTRL1		0x58
#define VIDEO_CTRL2		0x59
#define CHRM_KEY_CMP0		0x5c
#define CHRM_KEY_CMP1		0x5d
#define CHRM_KEY_CMP2		0x5e

/*
 * CLKDAC registers
 */
#define POWER_CTRL		0x66
#define CLKDAC_INDEX		0x68
#define CLKDAC_DATA		0x69
 
#define CLKDAC_CLK0_F0		0
#define CLKDAC_CLK0_F1		1
#define CLKDAC_CLK0_F2		2
#define CLKDAC_CLK0_F3		3
#define CLKDAC_CLK0_F4		4
#define CLKDAC_CLK0_F5		5
#define CLKDAC_CLK0_F6		6
#define CLKDAC_CLK0_F7		7
#define CLKDAC_CRC_DATA		8
#define CLKDAC_SPRITE_COLOR	9
#define CLKDAC_CLK1_FA		10
#define CLKDAC_RES0		11
#define CLKDAC_RES1		12
#define CLKDAC_CMD		13
#define CLKDAC_CRC_CTRL		14
#define CLKDAC_ID		15

/*
 * Display list registers
 */
#define DL_ADR_L		0x80
#define DL_ADR_H		0x81

/*
 * Sprite registers
 */
#define SPT_HOR_PRESET		0x82
#define SPT_VERT_PRESET		0x83
#define SPT_HOR_POS_L		0x84
#define SPT_HOR_POS_H		0x85
#define SPT_VERT_POS_L		0x86
#define SPT_VERT_POS_H		0x87
/*
 * ------------------------------------------------------------------
 */
/*
 * General I/O registers
 */

#define MISC_READ		0x3cc
#define MISC_WRITE		0x3c2

#define INP_STAT0		0x3c2
#define INP_STATM1		0x3ba
#define INP_STATC1		0x3da
#define FEATURE_CTRL		0x3ca
#define KEY_CTRL		0x3b8

/*
 * LUT palette I/O registers
 */
#define PIXEL_MASK		0x3c6
#define RMODE_LUT_ADR		0x3c7
#define DAC_STATE		0x3c7
#define WMODE_LUT_ARD		0x3c8
#define LUT_COL_VALUE		0x3c9

/*
 * ATC (Attribute controller) registers
 */
#define ATC_INDEX		0x3c0
#define ATC_DATA		0x3c1

#define ATC_PAL0	0
#define ATC_PAL1	1
#define ATC_PAL2	2
#define ATC_PAL3	3
#define ATC_PAL4	4
#define ATC_PAL5	5
#define ATC_PAL6	6
#define ATC_PAL7	7
#define ATC_PAL8	8
#define ATC_PAL9	9
#define ATC_PAL10	10
#define ATC_PAL11	11
#define ATC_PAL12	12
#define ATC_PAL13	13
#define ATC_PAL14	14
#define ATC_PAL15	15

#define ATC_MODE_CTRL		0x10
#define ATC_OVRSCN_COLOR	0x11
#define ATC_PLANE_ENABLE	0x12
#define ATC_HOR_PIXELPAN	0x13
#define ATC_COLOR_SEL		0x14

#define ATC_MISC0		0x16
#define ATC_MISC1		0x17

/*
 * CRT Control I/O registers
 */
#define CRTC_MINDEX		0x3b4
#define CRTC_MDATA		0x3b5
#define CRTC_CINDEX		0x3d4
#define CRTC_CDATA		0x3d5

/*
 * GDC (Graphics Display Controller) registers
 */
#define GDC_SEGSEL_1		0x3cd
#define GDC_SEGSEL_2		0x3cb
#define GDC_INDEX		0x3ce
#define GDC_DATA		0x3cf

#define GDC_setreset		0
#define GDC_enable_setreset	1
#define GDC_colorcompare	2
#define GDC_datarotate		3
#define GDC_readplane_select	4
#define GDC_mode		5
#define GDC_misc		6
#define GDC_colorcompare_enable	7
#define GDC_bitmask		8


/*
 * TS (Timing Sequencer) registers
 */
#define TS_INDEX		0x3c4
#define TS_DATA			0x3c5

#define TS_setreset		0
#define TS_mode			1
#define TS_writeplane_mask	2
#define TS_fontselect		3
#define TS_memmode		4
#define TS_state_ctrl		6



/*
 * ------------------------------------------------------------------
 */
#define MMU_CONTROL			0x13

#define ACL_SUSPEND_TERMINATE		0x30 
#define ACL_OPERATION_STATE		0x31

/*
 *  for ET6000, ACL_SYNC_ENABLE becomes ACL_6K_CONFIG
 */
#define ACL_SYNC_ENABLE			0x32

#define ACL_WRITE_INTERFACE_VALID	0x33
#define ACL_INTERRUPT_MASK		0x34
#define ACL_INTERRUPT_STATUS		0x35
#define ACL_ACCELERATOR_STATUS		0x36

/*
 *  and this is only for the ET6000
 */
#define ACL_POWER_CONTROL		0x37

/*
 *  non-queued for w32p's and ET6000
 */
#define ACL_NQ_X_POSITION		0x38
#define ACL_NQ_Y_POSITION		0x3A
/*
 *  queued for w32 and w32i
 */
#define ACL_X_POSITION			0x94
#define ACL_Y_POSITION			0x96

#define ACL_PATTERN_ADDRESS 		0x80
#define ACL_SOURCE_ADDRESS		0x84

#define ACL_PATTERN_Y_OFFSET		0x88
#define ACL_SOURCE_Y_OFFSET		0x8A
#define ACL_DESTINATION_Y_OFFSET	0x8C

/*
 *  W32i
 */
#define ACL_VIRTUAL_BUS_SIZE 		0x8E
/*
 *  w32p
 */
#define ACL_PIXEL_DEPTH 		0x8E

/*
 *  w32 and w32i
 */
#define ACL_XY_DIRECTION 		0x8F


#define ACL_PATTERN_WRAP		0x90
#define ACL_TRANSFER_DISABLE		0x91 /* ET6000 only */
#define ACL_SOURCE_WRAP			0x92

#define ACL_X_COUNT			0x98
#define ACL_Y_COUNT			0x9A
#define ACL_XY_COUNT			ACL_X_COUNT /* shortcut. not a real register */

/*
 *  for ET6000, ACL_ROUTING_CONTROL becomes ACL_MIX_CONTROL
 */
#define ACL_ROUTING_CONTROL		0x9C
#define ACL_MIX_CONTROL			ACL_ROUTING_CONTROL
/*
 *  for ET6000, ACL_RELOAD_CONTROL becomes ACL_STEPPING_INHIBIT
 */
#define ACL_RELOAD_CONTROL		0x9D
#define ACL_STEPPING_INHIBIT		ACL_RELOAD_CONTROL

#define ACL_BACKGROUND_RASTER_OPERATION	0x9E 
#define ACL_FOREGROUND_RASTER_OPERATION	0x9F

#define ACL_DESTINATION_ADDRESS 	0xA0

/*
 *  the following is for the w32p's only
 */
#define ACL_MIX_ADDRESS 		0xA4

#define ACL_MIX_Y_OFFSET 		0xA8
#define ACL_ERROR_TERM 			0xAA
#define ACL_DELTA_MINOR 		0xAC
#define ACL_DELTA_MAJOR 		0xAE

/*
 *  ET6000 only (trapezoids)
 */
#define ACL_SECONDARY_EDGE		0x93
#define ACL_SECONDARY_ERROR_TERM	0xB2
#define ACL_SECONDARY_DELTA_MINOR	0xB4
#define ACL_SECONDARY_DELTA_MAJOR	0xB6

#endif	/* _ET6K_ACL_H */
