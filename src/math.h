#ifndef _OVDI_MATH_H
#define _OVDI_MATH_H

/* (x * y) / z */
#define SMUL_DIV(x,y,z)	((short)(((short)(x)*(long)((short)(y)))/(short)(z)))
#define ABS(x) ((x) >= 0 ? (x) : -(x))

short Isin(unsigned short angle);
short Icos(short angle);

short isqrt(unsigned long x);

#endif	/* _OVDI_MATH_H */