#ifndef _VBI_H
#define _VBI_H

void	init_vbi(void);
short	get_vbitics(void);
short	add_vbi_function(unsigned long function, unsigned long tics);
void	remove_vbi_function(unsigned long function);
void	reset_vbi(void);
void	enable_vbi(void);
void	disable_vbi(void);


#endif	/* _VBI_H */
