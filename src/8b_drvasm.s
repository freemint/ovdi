		.globl	_asm_rb_S_ONLY_08

		.text
		
_asm_rb_S_ONLY_08:
		movem.l	d2-d7/a2-a6,-(sp)

		lea	((6+5+1)*4)(sp),a2

		move.l	(a2)+,a0
		move.w	(a2)+,d0
		move.l	(a2)+,a1
		move.w	(a2)+,d1

		move.w	(a2)+,d2
		move.w	(a2)+,d3
		subq.w	#1,d3

		move.w	d2,d5
		and.w	#3,d5
		subq.w	#1,d5

		move.w	d2,d4
		lsr	#6,d4
		
		lsr.w	#1,d2
		not.w	d2
		and.w	#30,d2
		addq.w	#2,d2
		
		tst.w	(a2)
		beq.s	.up

		addq.w	#1,a0
		addq.w	#1,a1

.nxt_dline:	move.l	a0,a3
		move.l	a1,a4
		move.w	d4,d6
		jmp	.down64(pc,d2.w)
.down64:	move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		move.l	-(a3),-(a4)
		dbra	d6,.down64

		move.w	d5,d6
		bmi.s	.no_d_tail
.d_tail:	move.b	-(a3),-(a4)
		dbra	d6,.d_tail

.no_d_tail:	sub.w	d0,a0
		sub.w	d1,a1
		dbra	d3,.nxt_dline
		bra	.exit

.up:
.nxt_uline:	move.l	a0,a3
		move.l	a1,a4
		move.w	d4,d6
		jmp	.up64(pc,d2.w)
.up64:		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		move.l	(a3)+,(a4)+
		dbra	d6,.up64

		move.w	d5,d6
		bmi.s	.no_u_tail
.u_tail:	move.b	(a3)+,(a4)+
		dbra	d6,.u_tail
.no_u_tail:	add.w	d0,a0
		add.w	d1,a1
		dbra	d3,.nxt_uline

.exit:		movem.l	(sp)+,d2-d7/a2-a6
		rts
