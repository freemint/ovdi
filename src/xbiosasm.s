		.globl _new_xbioswr
		.globl _new_xbios
		.globl _old_trap14

longframe	= 0x59e
savptr		= 0x4a2

		.text
		dc.l	0x58425241	|"XBRA"
		dc.l	0x6f564449
_old_trap14:	dc.l	0
_new_xbioswr:	move.w	sr,d1
		or.w	#0x700,sr

		movea.l	savptr,a1
		move.w	(sp)+,d0
		move.w	d0,-(a1)
		move.l	(sp)+,-(a1)

		| Test for cputype - for now assume long stack frames
		move.w	(sp)+,-(a1)

		move.l	sp,-(a1)
		move.l	a1,savptr

		move.w	d1,sr

		btst	#13,d0
		bne.s	.already_super
		move.l	usp,sp

.already_super:	move.l	sp,a0
		movem.l	d1-d2/a0-a2,-(sp)
		move.l	a0,-(sp)
		jsr	_new_xbios
		addq.w	#4,sp
		movem.l	(sp)+,d1-d2/a0-a2
		cmp.l	#0xfacedace,d0
		beq.s	old_xbios

		move.w	sr,d1
		or.w	#0x700,sr

		movea.l	savptr,a1
		movea.l	(a1)+,sp

		| CPU type assumed to be long stackframe
		move.w	(a1)+,-(sp)

		move.l	(a1)+,-(sp)
		move.w	(a1)+,-(sp)
		move.l	a1,savptr

		move.w	d1,sr
		rte

old_xbios:	move.w	sr,d0
		or.w	#0x700,sr

		movea.l	savptr,a1
		movea.l	(a1)+,sp

		| CPU type assumed to be long stackframe
		move.w	(a1)+,-(sp)

		move.l	(a1)+,-(sp)
		move.w	(a1)+,-(sp)
		move.l	a1,savptr
		move.w	d0,sr
		movea.l	_old_trap14,a0
		jmp	(a0)
