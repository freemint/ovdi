	.globl _do_p_chres
	.globl _do_p_setscr
	.globl _do_p_setcol
	.globl _do_p_vsync
	.globl _do_p_chng_vrt
	
	.text
_do_p_chres:
	movem.l	d0-d7/a0-a6,-(sp)
	move.l	16*4(sp),a1		| Address of routine
	move.l	(16*4)+4(sp),a0		| RES adress
	move.l	(16*4)+8(sp),d0		| screen offset
	jsr	(a1)
	movem.l	(sp)+,d0-d7/a0-a6
	rts

_do_p_setscr:
	movem.l	d0-d7/a0-a6,-(sp)
	move.l	16*4(sp),a1		| Address of routine
	move.l	(16*4)+4(sp),a0		| screen address
	jsr	(a1)
	movem.l	(sp)+,d0-d7/a0-a6
	rts

_do_p_setcol:
	movem.l	d0-d7/a0-a6,-(sp)
	move.l	16*4(sp),a1		| Address of routine
	move.w	(16*4)+4(sp),d0		| Pen
	move.l	(16*4)+6(sp),a0		| Address of *colors
	jsr	(a1)
	movem.l	(sp)+,d0-d7/a0-a6
	rts

_do_p_vsync:
	movem.l	d0-d7/a0-a6,-(sp)
	move.l	16*4(sp),a1		| Address of routine
	jsr	(a1)
	movem.l	(sp)+,d0-d7/a0-a6
	rts

_do_p_chng_vrt:
	movem.l	d0-d7/a0-a6,-(sp)
	move.l	16*4(sp),a1		| Address of routine
	move.w	(16*4)+4(sp),d0		| X
	move.w	(16*4)+6(sp),d1		| Y
	jsr	(a1)
	movem.l	(sp)+,d0-d7/a0-a6
	rts
