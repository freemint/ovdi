#ifndef	_MODINF_H
#define _MODINF_H

#define D_VHW	0x00000001	/* Video Hardware Driver */
#define D_VBI	0x00000002	/* VBI driver */
#define D_PDV	0x00000004	/* Poinding DeVice driver */
#define D_KBD	0x00000008	/* Keyboard driver */
#define D_TIM	0x00000010	/* Timer driver */

struct module_desc
{
	long	types;
	void	*vhw;
	void	*vbi;
	void	*pdv;
	void	*kbd;
	void	*tim;
	void	*res[32-5];
};

#endif	/* _MODINF_H */
