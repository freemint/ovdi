/*
 * $Id$
 *
 * XaAES - XaAES Ain't the AES
 *
 * A multitasking AES replacement for MiNT
 *
 * This file is part of XaAES.
 *
 * XaAES is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * XaAES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XaAES; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * _setstack - changes the stack pointer
 *
 * Prototype:
 * void _setstack(void *newsp);
 *
 * WARNING: after this local variables may no longer be valid!
 *          destroys a0 and a7 too
 */

	.globl	__setstack

	.text

__setstack:
	move.l	(sp)+,a0
	move.l	(sp),sp
	subq.w	#4,sp
	jmp	(a0)
