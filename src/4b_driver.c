#include "ovdi_defs.h"
#include "ovdi_dev.h"
#include "4b_driver.h"

void dlp_TRNS_c00_4b(unsigned char *addr, long data);
void dlp_TRNS_c01_4b(unsigned char *addr, long data);
void dlp_TRNS_c02_4b(unsigned char *addr, long data);
void dlp_TRNS_c03_4b(unsigned char *addr, long data);
void dlp_TRNS_c04_4b(unsigned char *addr, long data);
void dlp_TRNS_c05_4b(unsigned char *addr, long data);
void dlp_TRNS_c06_4b(unsigned char *addr, long data);
void dlp_TRNS_c07_4b(unsigned char *addr, long data);
void dlp_TRNS_c08_4b(unsigned char *addr, long data);
void dlp_TRNS_c09_4b(unsigned char *addr, long data);
void dlp_TRNS_c10_4b(unsigned char *addr, long data);
void dlp_TRNS_c11_4b(unsigned char *addr, long data);
void dlp_TRNS_c12_4b(unsigned char *addr, long data);
void dlp_TRNS_c13_4b(unsigned char *addr, long data);
void dlp_TRNS_c14_4b(unsigned char *addr, long data);
void dlp_TRNS_c15_4b(unsigned char *addr, long data);

void dlp_XOR_c00_4b(unsigned char *addr, long data);

void dlp_REP_c00_4b(unsigned char *addr, long data);
void dlp_REP_c01_4b(unsigned char *addr, long data);
void dlp_REP_c02_4b(unsigned char *addr, long data);
void dlp_REP_c03_4b(unsigned char *addr, long data);
void dlp_REP_c04_4b(unsigned char *addr, long data);
void dlp_REP_c05_4b(unsigned char *addr, long data);
void dlp_REP_c06_4b(unsigned char *addr, long data);
void dlp_REP_c07_4b(unsigned char *addr, long data);
void dlp_REP_c08_4b(unsigned char *addr, long data);
void dlp_REP_c09_4b(unsigned char *addr, long data);
void dlp_REP_c10_4b(unsigned char *addr, long data);
void dlp_REP_c11_4b(unsigned char *addr, long data);
void dlp_REP_c12_4b(unsigned char *addr, long data);
void dlp_REP_c13_4b(unsigned char *addr, long data);
void dlp_REP_c14_4b(unsigned char *addr, long data);
void dlp_REP_c15_4b(unsigned char *addr, long data);

void dp_TRANSPARENT_4b	(unsigned char *addr, long data);
void dp_XOR_4b		(unsigned char *addr, long data);
void dp_OR_4b		(unsigned char *addr, long data);
void dp_REPLACE_4b	(unsigned char *addr, long data);

/* typedef void (*dlp_draw)(unsigned char *addr, short bit); */

draw_pixel dlp_4b_TRANS_func[] =
{
	dlp_TRNS_c00_4b,
	dlp_TRNS_c01_4b,
	dlp_TRNS_c02_4b,
	dlp_TRNS_c03_4b,
	dlp_TRNS_c04_4b,
	dlp_TRNS_c05_4b,
	dlp_TRNS_c06_4b,
	dlp_TRNS_c07_4b,
	dlp_TRNS_c08_4b,
	dlp_TRNS_c09_4b,
	dlp_TRNS_c10_4b,
	dlp_TRNS_c11_4b,
	dlp_TRNS_c12_4b,
	dlp_TRNS_c13_4b,
	dlp_TRNS_c14_4b,
	dlp_TRNS_c15_4b
};

draw_pixel dlp_4b_OR_func[] =
{
	dlp_TRNS_c15_4b,
	dlp_TRNS_c14_4b,
	dlp_TRNS_c13_4b,
	dlp_TRNS_c12_4b,
	dlp_TRNS_c11_4b,
	dlp_TRNS_c10_4b,
	dlp_TRNS_c09_4b,
	dlp_TRNS_c08_4b,
	dlp_TRNS_c07_4b,
	dlp_TRNS_c06_4b,
	dlp_TRNS_c05_4b,
	dlp_TRNS_c04_4b,
	dlp_TRNS_c03_4b,
	dlp_TRNS_c02_4b,
	dlp_TRNS_c01_4b,
	dlp_TRNS_c00_4b
};

draw_pixel dlp_4b_XOR_func[] =
{
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b,
	dlp_XOR_c00_4b
};

draw_pixel dlp_4b_REP_func[] =
{
	dlp_REP_c00_4b,
	dlp_REP_c01_4b,
	dlp_REP_c02_4b,
	dlp_REP_c03_4b,
	dlp_REP_c04_4b,
	dlp_REP_c05_4b,
	dlp_REP_c06_4b,
	dlp_REP_c07_4b,
	dlp_REP_c08_4b,
	dlp_REP_c09_4b,
	dlp_REP_c10_4b,
	dlp_REP_c11_4b,
	dlp_REP_c12_4b,
	dlp_REP_c13_4b,
	dlp_REP_c14_4b,
	dlp_REP_c15_4b
};

draw_pixel dpf_4b[] =
{
	dp_REPLACE_4b,
	0,
	dp_OR_4b,
	0,
	dp_XOR_4b,
	0,
	dp_TRANSPARENT_4b,
	0
};

void
dp_TRANSPARENT_4b(unsigned char *addr, long data)
{
	(*dlp_4b_TRANS_func[(short)(data & 0xff)])(addr, (long)((data >> 16) & 0xf));
}
void
dp_XOR_4b(unsigned char *addr, long data)
{
	(*dlp_4b_XOR_func[(short)(data & 0xff)])(addr, (long)((data >> 16) & 0xf));
}
void
dp_OR_4b(unsigned char *addr, long data)
{
	(*dlp_4b_OR_func[(short)(data & 0xff)])(addr, (long)((data >> 16) & 0xf));
}
void
dp_REPLACE_4b(unsigned char *addr, long data)
{
	(*dlp_4b_REP_func[(short)(data & 0xff)])(addr, (long)((data >> 16) & 0xf));
}

void
dlp_TRNS_c00_4b(unsigned char *addr, long data)
{
	*(short *)addr		|= (short)data;
	*(short *)(addr+2)	|= (short)data;
	*(short *)(addr+4)	|= (short)data;
	*(short *)(addr+6)	|= (short)data;
}
void
dlp_TRNS_c01_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_TRNS_c02_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_TRNS_c03_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_TRNS_c04_4b(unsigned char *addr, long data)
{
	*(short *)addr 		|=  (short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_TRNS_c05_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_TRNS_c06_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_TRNS_c07_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_TRNS_c08_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_TRNS_c09_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_TRNS_c10_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_TRNS_c11_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_TRNS_c12_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_TRNS_c13_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_TRNS_c14_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_TRNS_c15_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	&= ~(short)data;
}

/* **************** XOR ********** */

void
dlp_XOR_c00_4b(unsigned char *addr, long data)
{
	*(short *)addr		^= (short)data;
	*(short *)(addr+2)	^= (short)data;
	*(short *)(addr+4)	^= (short)data;
	*(short *)(addr+6)	^= (short)data;
}
/* *************** OR ************* */
/* Same as transparent, only opposite */

/* ************* REPLACE ********** */
void
dlp_REP_c00_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_REP_c01_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_REP_c02_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_REP_c03_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_REP_c04_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_REP_c05_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_REP_c06_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_REP_c07_4b(unsigned char *addr, long data)
{
	*(short *)addr		&= ~(short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_REP_c08_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_REP_c09_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_REP_c10_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_REP_c11_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	&= ~(short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_REP_c12_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	&= ~(short)data;
}
void
dlp_REP_c13_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	&= ~(short)data;
	*(short *)(addr+6)	|=  (short)data;
}
void
dlp_REP_c14_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	&=  ~(short)data;
}
void
dlp_REP_c15_4b(unsigned char *addr, long data)
{
	*(short *)addr		|=  (short)data;
	*(short *)(addr+2)	|=  (short)data;
	*(short *)(addr+4)	|=  (short)data;
	*(short *)(addr+6)	|=  (short)data;
}
