	.globl _mouse_absolute_move
	.globl _mouse_relative_move
	.globl _mouse_buttons_change
	.globl _mouse_interrupt

	.globl _m_abs_move
	.globl _m_rel_move
	.globl _m_but_chg
	.globl _m_int

	.text

_m_abs_move:	movem.l	d0-d1/a0-a1,-(sp)
		move.w	((1+2+2)*4)+2(sp),-(sp)
		move.w	((1+2+2)*4)+2(sp),-(sp)
		jsr	_mouse_absolute_move
		addq.w	#4,sp
		movem.l	(sp)+,d0-d1/a0-a1
		rts

_m_rel_move:	movem.l	d0-d1/a0-a1,-(sp)
		move.w	((1+2+2)*4)+2(sp),-(sp)
		move.w	((1+2+2)*4)+2(sp),-(sp)
		jsr	_mouse_relative_move
		addq.w	#4,sp
		movem.l	(sp)+,d0-d1/a0-a1
		rts

_m_but_chg:	movem.l	d0-d1/a0-a1,-(sp)
		move.w	((1+2+2)*4)(sp),-(sp)
		jsr	_mouse_buttons_change
		addq.w	#2,sp
		movem.l	(sp)+,d0-d1/a0-a1
		rts

_m_int:		movem.l	d0-d1/a0-a1,-(sp)
		jsr	_mouse_interrupt
		movem.l	(sp)+,d0-d1/a0-a1
		rts


		