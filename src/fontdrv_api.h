#ifndef _fontdrv_api_h
#define _fontdrv_api_h

#include "vdi_font.h"

#define FONTTYPE_GEMDOS	1

struct fontapi
{
	struct fontapi	*nxtapi;
	long		version;
	char		*sname;
	char		*lname;
	char		*filename;
	char		*pathname;

	long		fonttype;

	long		(*open)			(void);

	long		(*load_fonts)		(long handle, char *path, char *names);
	void		(*unload_fonts)		(long handle);
	
	long		(*open_face)		(long handle, char *filename, long ind, VF_FACE **vff);
	long		(*open_face_by_index)	(long handle, long index, VF_FACE **vff);
	long		(*open_face_by_id)	(long handle, long id, VF_FACE **vff);

	void		(*set_char_pointsize)	(VF_FACE *, long point);
	void		(*set_char_size)	(VF_FACE *, long width, long height);
	void		(*set_dev_size)		(VF_FACE *, long horz_res, long vert_res);
	void		(*set_pixel_size)	(VF_FACE *, long width, long height);
};
typedef struct fontapi FONTAPI;


#endif	/* _fontdrv_api_h */
