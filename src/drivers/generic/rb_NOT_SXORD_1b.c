#include "ovdi_defs.h"

void _cdecl rb_NOT_SXORD_1b(ROP_PB *rpb);

void _cdecl
rb_NOT_SXORD_1b(ROP_PB *rpb)
{
	unsigned short begmask, endmask, p0;
	int shift, swords, dwords, sbpl, dbpl, height;
	unsigned short *s, *d;

	sbpl = (rpb->s_bypl >> 1);
	dbpl = (rpb->d_bypl >> 1);
	height = rpb->sy2 - rpb->sy1 + 1;
	s = (unsigned short *)rpb->s_addr + (rpb->sx1 >> 4) + (long)((long)rpb->sy1 * sbpl);
	d = (unsigned short *)rpb->d_addr + (rpb->dx1 >> 4) + (long)((long)rpb->dy1 * dbpl);
	swords	= (((rpb->sx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;
	dwords	= (((rpb->dx1 & 0xf) + (rpb->sx2 - rpb->sx1 + 1) + 15) >> 4) - 1;

	if (s < d)
	{
		s = (unsigned short *)rpb->s_addr + (rpb->sx2 >> 4) + (long)((long)rpb->sy2 * sbpl);
		d = (unsigned short *)rpb->d_addr + (rpb->dx2 >> 4) + (long)((long)rpb->dy2 * dbpl);
		sbpl -= (swords + 1);
		dbpl -= (dwords + 1);
		begmask = 0xffff << (15 - (rpb->dx2 & 0xf));
		endmask = 0xffff >> (rpb->dx1 & 0xf);
		shift	= (rpb->sx2 & 0xf) - (rpb->dx2 & 0xf);

		if (!shift)
		{
			if (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = ~(*s-- ^ *d);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							for (swords = dwords + 1; swords > 0; swords--)
								*d-- = ~(*s-- ^ *d);
							d -= dbpl, s -= sbpl;
						}
					}
				}
				else // endif (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = (*d & ~endmask) | (~(*s-- ^ *d) & endmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							for (swords = dwords; swords > 0; swords--)
								*d-- = ~(*s-- ^ *d);
							*d-- = (*d & ~endmask) | (~(*s-- ^ *d) & endmask);
							d -= dbpl, s -= sbpl;
						}
					}
				}
			}
			else // endif (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d-- = (*d & ~begmask) | (~(*s-- ^ *d) & begmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{		
						for (; height > 0; height--)
						{
							*d-- = (*d & ~begmask) | (~(*s-- ^ *d) & begmask);
							for (swords = dwords; swords > 0; swords--)
								*d-- = ~(*s-- ^ *d);
							d -= dbpl, s -= sbpl;
						}
					}
				}
				else // (endmask == 0xffff)
				{
					if (!swords)
					{
						begmask &= endmask;
						for (; height > 0; height--)
						{
							*d-- = (*d & ~begmask) | (~(*s-- ^ *d) & begmask);
							d -= dbpl, s -= sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d-- = (*d & ~begmask) | (~(*s-- ^ *d) & begmask);
							for (swords = dwords - 1; swords > 0; swords--)
								*d-- = ~(*s-- ^ *d);
							*d-- = (*d & ~endmask) | (~(*s-- ^ *d) & endmask);
							d -= dbpl, s -= sbpl;
						}
					}
				}
			}
		}
		else if (shift > 0)
		{
			int rshift = 16 - shift;

			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d-- = (*d & ~begmask) | (~((*s-- << shift) ^ *d) & begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | (~((p0 << shift) ^ *d) & begmask);
						for (dwords = swords; dwords > 1; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = ~(((p0 >> rshift) | (tmp << shift)) ^ *d);
							p0 = tmp;
						}
						*d-- = (*d & ~endmask) | ( ~(((p0 >> rshift) | (*s-- << shift)) ^ *d) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
			else /* more dest than src words */
			{
				if (!swords)
				{
					//begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | (~((p0 << shift) ^ *d) & begmask);						
						*d-- = (*d & ~endmask) | (~((p0 >> rshift) ^ *d) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | (~((p0 << shift) ^ *d) & begmask);
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = ~(((p0 >> rshift) | (tmp << shift)) ^ *d);
							p0 = tmp;
						}
						*d-- = (*d & ~endmask) | (~((p0 >> rshift) ^ *d) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
		}
		else /* shift <= 0 */
		{
			int rshift;

			shift = -shift;
			rshift = 16 - shift;

			if (dwords == swords)
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d-- = (*d & ~begmask) | (~((*s-- ^ *d) >> shift) & begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | ( ~(((p0 >> shift) | (*s << rshift)) ^ *d) & begmask);
						p0 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = ~(((p0 >> shift) | (tmp << rshift)) ^ *d);
							p0 = tmp;
						}
						*d-- = (*d & ~endmask) | (~((p0 >> shift) ^ *d) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
			else /* more src words */
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | (~(((p0 >> shift) | (*s-- << rshift)) ^ *d) & begmask);
						d -= dbpl, s -= sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s--;
						*d-- = (*d & ~begmask) | (~(((p0 >> shift) | (*s << rshift)) ^ *d) & begmask);
						p0 = *s--;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s--;
							*d-- = ~(((p0 >> shift) | (tmp << rshift)) ^ *d);
							p0 = tmp;
						}
						*d-- = (*d & ~endmask) | (~(((p0 >> shift) | (*s-- << rshift)) ^ *d) & endmask);
						d -= dbpl, s -= sbpl;
					}
				}
			}
		}			
	}
	else // endif (s < d)
	{
		sbpl -= (swords + 1);
		dbpl -= (dwords + 1);
		begmask = 0xffff >> (rpb->dx1 & 0xf);
		endmask = 0xffff << (15 - (rpb->dx2 & 0xf));
		shift	= (rpb->dx1 & 0xf) - (rpb->sx1 & 0xf);

		if (!shift)
		{
			int i;

			if (begmask == 0xffff)
			{
				if (endmask == 0xffff)
				{
					for (;height > 0; height--)
					{
						for (i = swords + 1; i > 0; i--)
							*d++ = ~(*s++ ^ *d);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (;height > 0; height--)
					{
						for (i = swords; i > 0; i--)
							*d++ = ~(*s++ ^ *d);
						*d++ = (*d & ~endmask) | (~(*s++ ^ *d) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{
				if (endmask == 0xffff)
				{
					if (!swords)
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (~(*s++ ^ *d) & begmask);
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (~(*s++ ^ *d) & begmask);
							for (i = swords; i > 0; i--)
								*d++ = ~(*s++ ^ *d);
							d += dbpl, s += sbpl;
						}
					}
				}
				else
				{
					if (!swords)
					{
						begmask &= endmask;
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (~(*s++ ^ *d) & begmask);
							d += dbpl, s += sbpl;
						}
					}
					else
					{
						for (; height > 0; height--)
						{
							*d++ = (*d & ~begmask) | (~(*s++ ^ *d) & begmask);
							for (i = swords - 1; i > 0; i--)
								*d++ = ~(*s++ ^ *d);
							*d++ = (*d & ~endmask) | (~(*s++ ^ *d) & endmask);
							d += dbpl, s += sbpl;
						}
					}
				}
			}
		}
		else if (shift > 0)
		{
			int rshift = 16 - shift;

			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d++ = (*d & ~begmask) | (~((*s++ >> shift) ^ *d) & begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | (~((p0 >> shift) ^ *d) & begmask);
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = ~(((p0 << rshift) | (tmp >> shift)) ^ *d);
							p0 = tmp;
						}
						*d++ = (*d & ~endmask) | (~(((p0 << rshift) | (*s++ >> shift)) ^ *d) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{	/* should be more destination than source */
				if (!swords)
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | (~((p0 >> shift) ^ *d) & begmask);
						*d++ = (*d & ~endmask) | (~((p0 << rshift) ^ *d) & endmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | (~((p0 >> shift) ^ *d) & begmask);	
						for (dwords = swords; dwords > 0; dwords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = ~(((p0 << rshift) | (tmp >> shift)) ^ *d);
							p0 = tmp;
						}
						*d++ = (*d & ~endmask) | (~((p0 << rshift) ^ *d) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
		}
		else /* shift <= 0 */
		{
			int rshift;

			shift = -shift;
			rshift = 16 - shift;
			if (swords == dwords)
			{
				if (!swords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						*d++ = (*d & ~begmask) | (~((*s++ << shift) ^ *d) & begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | (~(((p0 << shift) | (*s >> rshift)) ^ *d) & begmask);
						p0 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = ~(((p0 << shift) | (tmp >> rshift)) ^ *d);
							p0 = tmp;
						}
						*d++ = (*d & ~endmask) | (~((p0 << shift) ^ *d) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
			else
			{
				if (!dwords)
				{
					begmask &= endmask;
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | (~(((p0 << shift) | (*s++ >> rshift)) ^ *d) & begmask);
						d += dbpl, s += sbpl;
					}
				}
				else
				{
					for (; height > 0; height--)
					{
						p0 = *s++;
						*d++ = (*d & ~begmask) | (~(((p0 << shift) | (*s >> rshift)) ^ *d) & begmask);
						p0 = *s++;
						for (swords = dwords; swords > 1; swords--)
						{
							unsigned short tmp;
							tmp = *s++;
							*d++ = ~(((p0 << shift) | (tmp >> rshift)) ^ *d);
							p0 = tmp;
						}
						*d++ = (*d & ~endmask) | (~(((p0 << shift) | (*s++ >> rshift)) ^ *d) & endmask);
						d += dbpl, s += sbpl;
					}
				}
			}
		}
	}
}
