		text
install_cookie:	move.l	a2,-(a7)
		move.l	a3,-(a7)
		movea.l	a0,a2
		move.l	_p_cookies.w,d0
		bne.s	.jar_exists
		moveq	#$0A,d1
		clr.w	d0
		bsr.w	new_cookiejar
		bra.s	.exit

.jar_exists:	movea.l	_p_cookies.w,a3
		bra.s	.search_cookie

.nxt_cookie:	addq.w	#8,a3
.search_cookie:	move.l	(a3),d0
		beq.s	.found
		cmp.l	(a2),d0
		bne.s	.nxt_cookie
.found:		move.l	(a3),d0
		cmp.l	(a2),d0
		bne.s	.all_new
	;Cookie already existed, just overwrite its value
		move.l	$0004(a2),$0004(a3)
		bra.s	.exit

.all_new:	move.l	a3,d0
		sub.l	_p_cookies.w,d0
		divs.l	#$8,d0
		moveq	#-$01,d1
		add.l	$0004(a3),d1
		cmp.l	d1,d0
		blt.s	.inst_new
	;Gotta install new jar, this one's too small
		moveq	#$0A,d1
		add.w	$0006(a3),d1
		addq.w	#1,d0
		movea.l	a2,a0
		bsr.w	new_cookiejar
		bra.s	.exit

	;Found free spot, install the cookie
.inst_new:	movea.l	a3,a0
		lea 	$0008(a3),a1
		move.l	(a0)+,(a1)+
		move.l	(a0)+,(a1)+
		movea.l	a2,a0
		movea.l	a3,a1
		move.l	(a0)+,(a1)+
		move.l	(a0)+,(a1)+
.exit:		movea.l	(a7)+,a3
		movea.l	(a7)+,a2
		rts

	;D0 = slots in old jar
	;D1 = slots in new jar
	;A0 = Install this cookie
new_cookiejar:	movem.l	d3-d4/a2-a4,-(a7)
		movea.l	a0,a3
		move.w	d0,d4
		move.w	d1,d3
		bsr.w	install_resvector
		move.w	d3,d0
		lsl.w	#3,d0
		ext.l	d0
		bsr.w	malloc
		movea.l	d0,a2
		move.l	a2,d0
		move.l	a2,d1
		beq.s	.exit
		movea.l	_p_cookies.w,a0
		move.l	d0,_p_cookies.w
		tst.w	d4
		ble.s	.nxt
		clr.w	d0
		bra.s	.cpy_loop

.copy_old_jar:	movea.l	a0,a1
		addq.w	#8,a0
		movea.l	a2,a4
		addq.w	#8,a2
		move.l	(a1)+,(a4)+
		move.l	(a1)+,(a4)+
		addq.w	#1,d0
.cpy_loop:	cmp.w	d0,d4
		bgt.s	.copy_old_jar
		bra.s	.inst_num

.nxt:		addq.w	#8,a2
.inst_num:	movea.l	a3,a0
		lea 	-$0008(a2),a1
		move.l	(a0)+,(a1)+
		move.l	(a0)+,(a1)+
		clr.l	(a2)
		move.w	d3,d0
		ext.l	d0
		move.l	d0,$0004(a2)
.exit:		movem.l	(a7)+,d3-d4/a2-a4
		rts

