		.globl _ikbd_mouse
		.globl _IKBD_Mouse
		
		.text

_ikbd_mouse:	movem.l	d0-d1/a0-a1,-(sp)
		move.l	a0,-(sp)
		jsr	_IKBD_Mouse
		addq.w	#4,sp
		movem.l	(sp)+,d0-d1/a0-a1
		rts

