		.globl	_cs_output_character

		.globl	_do_con_state
		.globl	_old_con_state
		.globl	_do_xconout_console
		.globl	_old_xconout_console
		.globl	_do_xconout_raw
		.globl	_old_xconout_raw

		.globl	_do_txtcurs_blnk
		
		.globl	_set_constate
		.globl	_set_xconout_raw

		.globl	_call_bellhook

		.globl	_textcursor_blink

	| This function is installed in 'csout_char' of the console structure
	| and is the function 'outsiders' use to output characters to the
	| console.
_cs_output_character:
		movem.l	d0-d1/a0-a1,-(sp)
		move.w	(2+2*4)+4(sp),-(sp)	| char
		move.l	currconsole,-(sp)
		move.l	currconstate,a0
		jsr	(a0)
		addq.w	#6,sp
		movem.l	(sp)+,d0-d1/a0-a1
		rts
		
	| This is installed at the con_state vector (0x4a8), and
	| calls the function 'currconstate', with argument
	| 'currconsole'.
		dc.l	0x58425241	|"XBRA"
		dc.l	0x6f564449	|oVDI
_old_con_state:	dc.l	0
_do_con_state:	movem.l	d0-a6,-(sp)
		move.w	d0,-(sp)
		move.l	currconsole,-(sp)
		move.l	currconstate,a0
		jsr	(a0)
		addq.w	#6,sp
		movem.l	(sp)+,d0-a6
		rts

	| This is installed at the xconout_console vector (0x586)
	| can just calls the do_con_state function.
		dc.l	0x58425241	|"XBRA"
		dc.l	0x6f564449	|oVDI
_old_xconout_console:
		dc.l	0
_do_xconout_console:
		move.w	6(sp),d0
		bra.s	_do_con_state

	| This is installed at the xconout_raw vector (0x592)
	| and calls the function pointed to by 'currxconraw'.
		dc.l	0x58425241	|"XBRA"
		dc.l	0x6f564449
_old_xconout_raw:
		dc.l	0
_do_xconout_raw:
		movem.l	d0-a6,-(sp)
		move.w	((8+7+1)*4)+2(sp),-(sp)
		move.l	currconsole,-(sp)
		move.l	currxconraw,a0
		jsr	(a0)
		addq.w	#6,sp
		movem.l	(sp)+,d0-a6
		rts
		

_call_bellhook:	movem.l	d0-a6,-(sp)
		move.l	0x5ac.w,a0
		jsr	(a0)
		movem.l	(sp)+,d0-a6
		rts

_set_constate:	move.l	4(sp),currconsole
		move.l	8(sp),currconstate
		rts

_do_txtcurs_blnk:
		movem.l	d0-d2/a0-a2,-(sp)
		move.l	currconsole,-(sp)
		jsr	_textcursor_blink
		addq.w	#4,sp
		movem.l	(sp)+,d0-d2/a0-a2
		rts
_set_xconout_raw:
		move.l	4(sp),currconsole
		move.l	8(sp),currxconraw
		rts

currconstate:	dc.l	0
currxconraw:	dc.l	0
currconsole:	dc.l	0
