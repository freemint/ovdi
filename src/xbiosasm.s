		.globl _new_xbioswr
		.globl _new_xbios
		.globl _old_trap14

		.text
_old_trap14:	dc.l	0
_new_xbioswr:	move	usp,a0
		btst	#5,(sp)		|;Supervisor mode?
		beq.s	.no_trap	|;Nope

	|;Assume long stackframe.. this is not gonna run on 68000 anyway
		lea	8(sp),a0

.no_trap:	movem.l	d1-d2/a0-a2,-(sp)
		move.l	a0,-(sp)
		jsr	_new_xbios
		addq.l	#4,sp
		movem.l	(sp)+,d1-d2/a0-a2
		cmp.l	#0xfacedace,d0
		beq.s	old_xbios
		rte

old_xbios:	move.l	_old_trap14,a0
		jmp	(a0)

