| Things needed by the wrappers
	.globl _linea_fonts
	.globl _linea_functab
	.globl _linea_vars
	.globl _linea_functab
	
	.globl _LineA_Handler

| Function wrappers located in this file
	.globl _LineA_Initialize
	.globl _LineA_PlotPixel
	.globl _LineA_GetPixel
	.globl _LineA_ArbLine
	.globl _LineA_HorLine
	.globl _LineA_FilledRect
	.globl _LineA_FilledPoly
	.globl _LineA_BitBlt
	.globl _LineA_TextBlt
	.globl _LineA_ShowMouse
	.globl _LineA_HideMouse
	.globl _LineA_TransformMouse
	.globl _LineA_UndrawSprite
	.globl _LineA_DrawSprite
	.globl _LineA_CopyRaster
	.globl _LineA_SeedFill
	
| Functions called by the wrappers here
	.globl _linea_handler
	.globl _linea_plot_pixel
	.globl _linea_get_pixel
	.globl _linea_arb_line
	.globl _linea_hor_line
	.globl _linea_filled_rect
	.globl _linea_filled_poly
	.globl _linea_bitblt
	.globl _linea_textblt
	.globl _linea_showmouse
	.globl _linea_hidemouse
	.globl _linea_transformmouse
	.globl _linea_undrawsprite
	.globl _linea_drawsprite
	.globl _linea_copyraster
	.globl _linea_seedfill
	
	.text

_LineA_Handler:
	movea.l	2(sp),a1
	move.w	(a1)+,d2
	and.w	#0xffff,d2
	move.l	a1,2(sp)
	cmp.w	#15,d2
	bhi.s	.exit

	movem.l	d0-d2/a0-a2,-(sp)
	move.w	d2,-(sp)
	jsr	_linea_handler
	addq.w	#2,sp
	movem.l	(sp)+,d0-d2/a0-a2

	lsl.w	#2,d2
	lea	_linea_functab,a1
	move.l	(a1,d2.w),a1
	jsr	(a1)

.exit:	rte	
	


_LineA_Initialize:
	move.l	_linea_vars,d0
	add.l	#910,d0
	move.l	d0,a0
	lea	_linea_fonts,a1
	lea	_linea_functab,a2
	rts

_LineA_PlotPixel:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_plot_pixel
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_GetPixel:
	rts
	movem.l	d1/a0-a1,-(sp)
	jsr	_linea_get_pixel
	movem.l	(sp)+,d1/a0-a1
	rts

_LineA_ArbLine:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_arb_line
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_HorLine:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_hor_line
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_FilledRect:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_filled_rect
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_FilledPoly:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_filled_poly
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_BitBlt:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	move.l	a6,-(sp)
	jsr	_linea_bitblt
	addq.w	#4,sp
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_TextBlt:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_textblt
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_ShowMouse:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_showmouse
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_HideMouse:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_hidemouse
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_TransformMouse:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_transformmouse
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_UndrawSprite:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	move.l	a2,-(sp)
	jsr	_linea_undrawsprite
	addq.l	#4,sp
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_DrawSprite:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	move.l	a2,-(sp)
	move.w	d1,-(sp)
	move.w	d0,-(sp)
	move.l	a0,-(sp)
	jsr	_linea_drawsprite
	lea	12(sp),sp
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_CopyRaster:
	rts
	movem.l	d0-d1/a0-a1,-(sp)
	jsr	_linea_copyraster
	movem.l	(sp)+,d0-d1/a0-a1
	rts

_LineA_SeedFill:
	rts
	movem.l d0-d1/a0-a1,-(sp)
	jsr	_linea_seedfill
	movem.l	(sp)+,d0-d1/a0-a1
	rts
