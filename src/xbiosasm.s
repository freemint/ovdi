		.globl _new_xbioswr
		.globl _new_xbios
		.globl _old_trap14

		.text
_old_trap14:	dc.l	0
_new_xbioswr:	move	sr,d1
		ori 	#0x700,sr
		movea.l	0x4a2.w,a1
		move.w	(a7)+,d0	|SR
		move.w	d0,-(a1)
		move.l	(a7)+,-(a1)	|PC
		tst.w	0x59e.w		|longframe
		beq.s	.frameok
		move.w	(a7)+,-(a1)	|type/vector
.frameok:	move.l	a7,-(a1)	|Stack itself
		move.l	a1,0x4a2.w	|savptr
		move	d1,sr
		btst	#0xd,d0		|Already in super?
		bne.s	.in_super		|yes
		move	usp,a7		|Fetch params off usp

.in_super:	move.l	sp,a1
		movem.l	d2/a0-a2,-(sp)
		move.l	a1,-(sp)
		jsr	_new_xbios
		addq.l	#4,sp
		movem.l	(sp)+,d2/a0-a2
		cmp.l	#0xfacedace,d0
		beq.s	old_xbios

		move	sr,d1
		ori 	#0x700,sr
		movea.l	0x4a2.w,a1		|savptr
		movea.l	(a1)+,a7
		tst.w	0x59e.w		|_longframe
		beq.s	.frameok1
		move.w	(a1)+,-(a7)
.frameok1:	move.l	(a1)+,-(a7)
		move.w	(a1)+,-(a7)
		move.l	a1,0x4a2.w	|savptr
		move	d1,sr
		rte

old_xbios:
		move.w	sr,d0
		or.w	#0x700,sr
	
		movea.l	0x4a2.w,a1	|savptr
		movea.l	(a1)+,a7
		tst.w	0x59e.w		|_longframe
		beq	.frameok2
	
		move.w	(a1)+,-(a7)
.frameok2:
		move.l	(a1)+,-(a7)
		move.w	(a1)+,-(a7)
		move.l	a1,0x4a2.w	|savptr

		move.w	d0,sr

		movea.l	_old_trap14,a0
		jmp	(a0)

