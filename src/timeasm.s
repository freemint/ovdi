		.globl _old_timeint
		.globl _time_interrupt
		.globl _time_interruptw

		.text
		dc.l	0x58425241	|"XBRA"
		dc.l	0x6f564449
_old_timeint:	dc.l	0
_time_interruptw:
		movem.l	d0-d2/a0-a2,-(sp)
		jsr	_time_interrupt
		movem.l	(sp)+,d0-d2/a0-a2
|		rts

		move.l	_old_timeint,-(sp)
		rts
