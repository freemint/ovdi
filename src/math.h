#ifndef _OVDI_MATH_H
#define _OVDI_MATH_H

/* (x * y) / z */
//#define SMUL_DIV(x,y,z)	((short)(((short)(x)*(long)((short)(y)))/(short)(z)))
#define SMUL_DIV(x,y,z)	((int)(((long)(x)*(y))/(z)))
#define ABS(x) ((x) >= 0 ? (x) : -(x))

int Isin(unsigned int angle);
int Icos(int angle);

int isqrt(unsigned long x);

#endif	/* _OVDI_MATH_H */