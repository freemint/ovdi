
;------------------------------------------------------------------
;PCI configuration registers

MEMMAP_CTRL:			= $40
PERFRM_CTRL:			= $41
MCLK_CTRL:			= $42
SCLK_CTRL:			= $43
RASCAS_CNF:			= $44
MEM_CNF:			= $45
DISP_FEATURE:			= $46
TEST_SETUP:			= $47

;VESA DCC bus control
VESA_DCC_CTRL:			= $4e

;General purpose output register
GPIO:				= $4f

;Image port registers
IMG_STRTADR_L:			= $50
IMG_STRTADR_M:			= $51
IMG_STRTADR_H:			= $52
IMG_LEN_L:			= $53
IMG_LEN_H:			= $54
IMGROW_OFFSET_L:		= $55
IMGROW_OFFSET_H:		= $56
IMGPORT_CTRL:			= $57

;Video control registers
VIDEO_CTRL1:			= $58
VIDEO_CTRL2:			= $59
CHRM_KEY_CMP0:			= $5c
CHRM_KEY_CMP1:			= $5d
CHRM_KEY_CMP2:			= $5e

;CLKDAC registers
POWER_CTRL:			= $66
CLKDAC_INDEX:			= $68
CLKDAC_DATA:			= $69

CLKDAC_CLK0_F0:			= 0
CLKDAC_CLK0_F1:			= 1
CLKDAC_CLK0_F2:			= 2
CLKDAC_CLK0_F3:			= 3
CLKDAC_CLK0_F4:			= 4
CLKDAC_CLK0_F5:			= 5
CLKDAC_CLK0_F6:			= 6
CLKDAC_CLK0_F7:			= 7
CLKDAC_CRC_DATA:		= 8
CLKDAC_SPRITE_COLOR:		= 9
CLKDAC_CLK1_FA			= 10
CLKDAC_RES0:			= 11
CLKDAC_RES1:			= 12
CLKDAC_CMD:			= 13
CLKDAC_CRC_CTRL:		= 14
CLKDAC_ID:			= 15

;Display list registers
DL_ADR_L:			= $80
DL_ADR_H:			= $81

;Sprite registers
SPT_HOR_PRESET:			= $82
SPT_VERT_PRESET:		= $83
SPT_HOR_POS_L:			= $84
SPT_HOR_POS_H:			= $85
SPT_VERT_POS_L:			= $86
SPT_VERT_POS_H:			= $87
;------------------------------------------------------------------
;General I/O registers

MISC_READ:			= $3cc
MISC_WRITE:			= $3c2

INP_STAT0:			= $3c2
INP_STATM1:			= $3ba
INP_STATC1:			= $3da
FEATURE_CTRL:			= $3ca
KEY_CTRL:			= $3b8

;LUT palette I/O registers
PIXEL_MASK:			= $3c6
RMODE_LUT_ADR:			= $3c7
DAC_STATE:			= $3c7
WMODE_LUT_ARD:			= $3c8
LUT_COL_VALUE:			= $3c9

;ATC (Attribute controller) registers
ATC_INDEX:			= $3c0
ATC_DATA:			= $3c1

ATC_PAL0			= 0
ATC_PAL1			= 1
ATC_PAL2			= 2
ATC_PAL3			= 3
ATC_PAL4			= 4
ATC_PAL5			= 5
ATC_PAL6			= 6
ATC_PAL7			= 7
ATC_PAL8			= 8
ATC_PAL9			= 9
ATC_PAL10			= 10
ATC_PAL11			= 11
ATC_PAL12			= 12
ATC_PAL13			= 13
ATC_PAL14			= 14
ATC_PAL15			= 15

ATC_MODE_CTRL:			= $10
ATC_OVRSCN_COLOR:		= $11
ATC_PLANE_ENABLE:		= $12
ATC_HOR_PIXELPAN:		= $13
ATC_COLOR_SEL:			= $14

ATC_MISC0:			= $16
ATC_MISC1:			= $17

;CRT Control I/O registers
CRTC_MINDEX:			= $3b4
CRTC_MDATA:			= $3b5
CRTC_CINDEX:			= $3d4
CRTC_CDATA:			= $3d5

;GDC (Graphics Display Controller) registers
GDC_SEGSEL_1:			= $3cd
GDC_SEGSEL_2:			= $3cb
GDC_INDEX:			= $3ce
GDC_DATA:			= $3cf

GDC_setreset:			= 0
GDC_enable_setreset:		= 1
GDC_colorcompare:		= 2
GDC_datarotate:			= 3
GDC_readplane_select:		= 4
GDC_mode:			= 5
GDC_misc:			= 6
GDC_colorcompare_enable:	= 7
GDC_bitmask:			= 8


;TS (Timing Sequencer) registers
TS_INDEX:			= $3c4
TS_DATA:			= $3c5

TS_setreset:			= 0
TS_mode:			= 1
TS_writeplane_mask:		= 2
TS_fontselect:			= 3
TS_memmode:			= 4
TS_state_ctrl:			= 6



;------------------------------------------------------------------
MMU_CONTROL			= $13

ACL_SUSPEND_TERMINATE		= $30 
ACL_OPERATION_STATE		= $31

; for ET6000, ACL_SYNC_ENABLE becomes ACL_6K_CONFIG
ACL_SYNC_ENABLE			= $32

ACL_WRITE_INTERFACE_VALID	= $33
ACL_INTERRUPT_MASK		= $34
ACL_INTERRUPT_STATUS		= $35
ACL_ACCELERATOR_STATUS		= $36

; and this is only for the ET6000 */
ACL_POWER_CONTROL		= $37

; non-queued for w32p's and ET6000
ACL_NQ_X_POSITION		= $38
ACL_NQ_Y_POSITION		= $3A
; queued for w32 and w32i
ACL_X_POSITION			= $94
ACL_Y_POSITION			= $96

ACL_PATTERN_ADDRESS 		= $80
ACL_SOURCE_ADDRESS		= $84

ACL_PATTERN_Y_OFFSET		= $88
ACL_SOURCE_Y_OFFSET		= $8A
ACL_DESTINATION_Y_OFFSET	= $8C

; W32i
ACL_VIRTUAL_BUS_SIZE 		= $8E
; w32p
ACL_PIXEL_DEPTH 		= $8E

; w32 and w32i
ACL_XY_DIRECTION 		= $8F


ACL_PATTERN_WRAP		= $90
ACL_TRANSFER_DISABLE		= $91 ;ET6000 only */
ACL_SOURCE_WRAP			= $92

ACL_X_COUNT			= $98
ACL_Y_COUNT			= $9A
ACL_XY_COUNT			= ACL_X_COUNT ; shortcut. not a real register */

; for ET6000, ACL_ROUTING_CONTROL becomes ACL_MIX_CONTROL
ACL_ROUTING_CONTROL		= $9C

; for ET6000, ACL_RELOAD_CONTROL becomes ACL_STEPPING_INHIBIT
ACL_RELOAD_CONTROL		= $9D
ACL_STEPPING_INHIBIT:		= ACL_RELOAD_CONTROL

ACL_BACKGROUND_RASTER_OPERATION	= $9E 
ACL_FOREGROUND_RASTER_OPERATION	= $9F

ACL_DESTINATION_ADDRESS 	= $A0

; the following is for the w32p's only
ACL_MIX_ADDRESS 		= $A4

ACL_MIX_Y_OFFSET 		= $A8
ACL_ERROR_TERM 			= $AA
ACL_DELTA_MINOR 		= $AC
ACL_DELTA_MAJOR 		= $AE

; ET6000 only (trapezoids)
ACL_SECONDARY_EDGE		= $93
ACL_SECONDARY_ERROR_TERM	= $B2
ACL_SECONDARY_DELTA_MINOR	= $B4
ACL_SECONDARY_DELTA_MAJOR	= $B6
