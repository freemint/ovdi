		.globl _new_vbi_wrapper
		.globl _old_vbi
		.globl _new_vbi

		.text

_new_vbi_wrapper:
		movem.l	d0-d1/a0-a1,-(sp)
		jsr	_new_vbi
		movem.l	(sp)+,d0-d1/a0-a1
		rts
