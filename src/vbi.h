#ifndef _VBI_H
#define _VBI_H

struct vbiapi
{
	short	(*get_tics)(void);
	short	(*add_func)(unsigned long func, unsigned long tics);
	void	(*del_func)(unsigned long func);
	void	(*reset)(void);
	void	(*enable)(void);
	void	(*disable)(void);
};
typedef struct vbiapi VBIAPI;
	
VBIAPI * init_vbi(void);

#endif	/* _VBI_H */
