		.globl _old_timeint
		.globl _time_interrupt
		.globl _time_interruptw

		.text
_old_timeint:	dc.l	0
_time_interruptw:
		movem.l	d0-d1/a0-a1,-(sp)
		jsr	_time_interrupt
		movem.l	(sp)+,d0-d1/a0-a1
		rts

|		move.l	_old_timeint,-(sp)
|		rts
