#ifndef _OVDI_FILLAPI_H
#define _OVDI_FILLAPI_H

struct fill16x_api
{
	unsigned long	sm;
	unsigned long	em;
	void		(*drawspan)(struct fill16x_api *);
	void		*d;
	void		*s;
	void		*m;
	short		sbpl;
	short		dbpl;
	short		words;
};
	
#endif	/* _OVDI_FILLAPI_H */
