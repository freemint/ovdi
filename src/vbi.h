#ifndef _VBI_H
#define _VBI_H

void	init_vbi(void);
short	get_vbitics(void);
short	add_vbi_function(unsigned long function, unsigned long tics);
void	remove_vbi_function(unsigned long function);


#endif	/* _VBI_H */
