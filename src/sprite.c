#include "linea.h"
void
draw_sprite(register MFORM *mf, register MSAVE *ms, register short x, register short y, short mx, short my)
{

	register short width, height;

	width = height = 15;

	if ((x - mf->xhot) < 0)
	{
		if ( (width += x) < 0)
		{
			ms->valid = 0;
			return;
		}
		x = 0;
	}
	else
	{
		if ((x + width) > mx)
		{
			if ((width -= (x - mx)) < 0)
			{
				ms->valid = 0;
				return;
			}
		}
	}

	if ((y - mf->yhot) < 0)
	{
		if ((height += y) < 0)
		{
			ms->valid = 0;
			return;
		}
		y = 0;
	}
	else
	{
		if ((y + height) > my)
		{
			if ((height -= (y + height) - my) < 0)
			{
				ms->valid = 0;
				return;
			}
		}
	}

}
	