		.globl	_New_Trap2
		.globl	_old_trap2_vec
		.globl	_oVDI
		
		.text

		dc.l	0x58425241	|"XBRA"
		dc.l	0x494d4e45	|"IMNE"
_old_trap2_vec:	dc.l	0
_New_Trap2:	movem.l	d1-d7/a0-a6,-(sp)
		cmp.w	#0x73,d0
		bne.s	.not_vdi
		move.l	d1,-(sp)			| vdi pb
		jsr	_oVDI
		addq.w	#4,sp
		tst.l	d0
		bmi.s	.return				| .call_old
		bra.s	.return
.not_vdi:	cmp.w	#-2,d0
		beq.s	.ret_gdos
		cmp.w	#-1,d0
		bne.s	.call_old
		move.l	#_oVDI,d0
		bra.s	.return
.call_old:	movem.l	(sp)+,d1-d7/a0-a6
		move.l	_old_trap2_vec,-(sp)
		rts
.ret_gdos:	move.l	#0x00000001,d0
.return:
.fine:		movem.l	(sp)+,d1-d7/a0-a6
		rte

