		text
	|A0 = Address of mouse_data
	|A2 = Address of save data area
	|D0 = Screen X at which to render mouse
	|D1 = Screen Y at which to render mouse
draw_sprite_8b:
		move.l	linea_vartab,a5

		lea	m_form(a0),a6

		moveq	#15,d2				|Max width -1
		moveq	#15,d3				|Max height -1

		sub.w	m_pos_hx(a0),d0			|X hotspot offscreen left?
		bcs.s	.hotX_left			|Yes
		
		moveq	#0,d7				|No shift count (screen X is within screen)
		move.w	la_dev_tab+devtab_xres(a5),d4	|Get current screen X res
		and.w	#~15,d4				|
		cmp.w	d4,d0				|X hotspot offscreen to the right?
		bls.s	.hotX_ok			|No, its within
		sub.w	d0,d4				|Pixels that are offscreen to the right
		add.w	d4,d2				|Pixels that are visible
		bge.s	.hotX_ok			|Some pixels are onscreen
		bra.s	.hot_offscreen			|No pixels are onscreen

.hotX_left:	move.w	d0,d7				|Pixels offscreen = shift count
		neg.w	d7
		add.w	d0,d2				|D2 = remaining visible pixels to draw
		bmi.s	.hot_offscreen			|no remaining pixels to draw - sprite totally offscreen
		moveq	#0,d0				|Screen X of sprite

.hotX_ok:	sub.w	m_pos_hy(a0),d1			|Hot Y above screen?
		bcs.s	.hotY_above			|Yes

		move.w	la_dev_tab+devtab_yres(a5),d4	|Get current screen Y res
		sub.w	d3,d4				|Screen Y res - hight of sprite
		cmp.w	d4,d1				|Hole sprite is on-screen?
		bls.s	.hotY_ok			|Yes
		sub.w	d1,d4				|D4 = offscreen lines
		add.w	d4,d3				|D3 = onscreen lines
		bge.s	.hotY_ok			|Some lines are onscreen
		bra.s	.hot_offscreen

.hotY_above:	add.w	d1,d3				|D3 = Lines on-screen
		bmi.s	.hot_offscreen			|No lines on-screen - sprite is above screen
		asl.w	#2,d1
		suba.w	d1,a6				|A6 now points to right line in m_form
		moveq	#0,d1

	|D0 = Screen X position of sprite
	|D1 = Screen Y position of sprite
	|D2 = Width of sprite in pixels
	|D3 = Height of sprite in lines
	|A6 = Mouse form address
	
.hotY_ok:	movea.l	_v_bas_ad.w,a1
		adda.w	d0,a1				|Add screen X offset
		moveq	#0,d4
		move.w	la_bytes_lin(a5),d4		|Bytes per scanline
		mulu.w	d4,d1
		add.l	d1,a1				|Add screen Y offset

	|Save the background..
		move.b	d2,(a2)+			|Width of area saved
		sub.w	d2,d4				|Screen wrap-to-next-scan offset
		move.b	d3,(a2)+			|Height of area saved
		subq.w	#1,d4
		move.l	a1,(a2)+			|Address in video-mem of the saved area
		move.l	a1,a3
		move.w	#-1,(a2)+			|Not restored flag
		move.w	d3,d1
		cmp.w	#15,d2
		bne.s	.save_anything
.save_16:	move.l	(a3)+,(a2)+
		move.l	(a3)+,(a2)+
		move.l	(a3)+,(a2)+
		move.l	(a3)+,(a2)+
		add.l	d4,a3
		dbra	d1,.save_16
		bra.s	.draw_mouse

.hot_offscreen:	clr.w	msav_valid(a2)
		rts

.save_anything:	move.w	d2,d5
		addq.w	#1,d5
.save_uneven:	move.w	d5,d0
		and.w	#3,d0
		beq.s	.save_only_long
		subq.w	#1,d0
.cpy_bytes:	move.b	(a3)+,(a2)+
		dbra	d0,.cpy_bytes

		move.w	d5,d0
		lsr.w	#2,d0
		beq.s	.nxt_line
		subq.w	#1,d0
.cpy_longs:	move.l	(a3)+,(a2)+
		dbra	d0,.cpy_longs
.nxt_line:	add.l	d4,a3
		dbra	d1,.save_uneven
		bra.s	.draw_mouse

.save_only_long:lsr.w	#2,d5
		subq.w	#1,d5
.only_longs:	move.w	d5,d0
.cpy_olongs:	move.l	(a3)+,(a2)+
		dbra	d0,.cpy_olongs
		add.l	d4,a3
		dbra	d1,.only_longs

.draw_mouse:	move.w	m_cdb_bg(a0),d1
		move.l	d4,a4
		move.w	m_cdb_fg(a0),d6
.next_line:	move.w	d2,d0
		move.w	(a6)+,d4			|Mask
		rol.w	d7,d4
		move.w	(a6)+,d5			|Image
		rol.w	d7,d5
.write_loop:	lsl.w	#1,d5
		bcc.s	.no_image
		move.b	d6,(a1)+			|Store Image (forground color)
		lsl.w	#1,d4
.draw_loop:	dbra	d0,.write_loop
		add.l	a4,a1
		dbra	d3,.next_line
		bra.s	.exit

.no_image:	lsl.w	#1,d4
		bcc.s	.no_mask
		move.b	d1,(a1)+
		bra.s	.draw_loop
.no_mask:	addq.l	#1,a1
		bra.s	.draw_loop

.exit:		rts
|-------------------------------------------------------------------------
|	A2 = background save structure
undraw_sprite_8b:
		tst.w	msav_valid(a2)		|This background is valid?
		beq.s	.exit			|No - exit
		moveq	#0,d0
		move.b	(a2)+,d0		|Width of saved area
		moveq	#0,d1
		move.b	(a2)+,d1		|Height of save area
		move.l	(a2)+,a0		|Address in video-ram this background belongs
		clr.w	(a2)+			|Clear valid flag
		move.l	linea_vartab,a1
		moveq	#0,d2
		move.w	la_bytes_lin(a1),d2	|Width of scanline
		sub.w	d0,d2			| - with of area saved = offset_2_nxt_scan
		subq.w	#1,d2
		cmp.w	#15,d0			|Saved the full 16-pixels?
		bne.s	.cpy_anything		|No
.fast:		move.l	(a2)+,(a0)+
		move.l	(a2)+,(a0)+
		move.l	(a2)+,(a0)+
		move.l	(a2)+,(a0)+
		add.l	d2,a0
		dbra	d1,.fast
.exit:		rts

.cpy_anything:	addq.w	#1,d0			|Cause we do some calcs that need 1..n 
.cpy_uneven:	move.w	d0,d3
		and.w	#3,d3			|Uneven width?
		beq.s	.cpy_only_long		|No, we can copy a bit faster
		subq.w	#1,d3			|Number of single pixels..
.cpy_bytes:	move.b	(a2)+,(a0)+		|Write single pixels
		dbra	d3,.cpy_bytes

		move.w	d0,d3
		lsr.w	#2,d3
		beq.s	.nxt_line
		subq.w	#1,d3
.cpy_longs:	move.l	(a2)+,(a0)+
		dbra	d3,.cpy_longs
.nxt_line:	add.l	d2,a0
		dbra	d1,.cpy_uneven
		rts

.cpy_only_long:	lsr.w	#2,d0
		subq.w	#1,d0
.only_longs:	move.w	d0,d3
.cpy_olongs:	move.l	(a2)+,(a0)+
		dbra	d3,.cpy_olongs
		add.l	d2,a0
		dbra	d1,.only_longs
		rts
|------------------------------------------------------------------------------
		
		
