
#define ET6K_SETSEG(seg) \
	outb(0x3CB, ((seg) & 0x30) | ((seg) >> 4)); \
	outb(0x3CD, ((seg) & 0x0f) | ((seg) << 4));

#define outb(loc, val) \
	ioutb(b, (long)loc, (long)val);
#define inb(loc) \
	iinb(b, (long)loc);

static short
et6000_check_videoram(ScrnInfoPtr pScrn, long ram)
{
	unsigned char oldSegSel1, oldSegSel2, oldGR5, oldGR6, oldSEQ2, oldSEQ4;
	int segment, i;
	long real_ram = 0;
	pointer check_vgabase;
	short fooled = 0;
	long save_vidmem;

	if (ram > 4096)
	{
		ram = 4096;
	}

	/*
	 * We need to set the VGA controller in VGA graphics mode, or else we won't
	 * be able to access the full 4MB memory range. First, we save the
	 * registers we modify, of course.
	 */

	oldSegSel1 = inb(0x3CD);
	oldSegSel2 = inb(0x3CB);
	outb(0x3CE, 5);
	oldGR5 = inb(0x3CF);
	outb(0x3CE, 6);
	oldGR6 = inb(0x3CF);
	outb(0x3C4, 2);
	oldSEQ2 = inb(0x3C5);
	outb(0x3C4, 4);
	oldSEQ4 = inb(0x3C5);

	/* set graphics mode */
	outb(0x3CE, 6);
	outb(0x3CF, 5);
	outb(0x3CE, 5);
	outb(0x3CF, 0x40);
	outb(0x3C4, 2);
	outb(0x3C5, 0x0f);
	outb(0x3C4, 4);
	outb(0x3C5, 0x0e);

	/*
	 * count down from presumed amount of memory in SEGSIZE steps, and
	 * look at each segment for real RAM.
	 *
	 * To select a segment, we cannot use ET4000W32SetReadWrite(), since
	 * that requires the ScreenPtr, which we don't have here.
	 */

	for (segment = (ram / SEGSIZE) - 1; segment >= 0; segment--)
	{
		/* select the segment */
		ET6K_SETSEG(segment);

		/* save contents of memory probing location */
		save_vidmem = *(VIDMEM);

		/* test with pattern */
		*VIDMEM = 0xAAAA5555;
		if (*VIDMEM != 0xAAAA5555)
		{
			*VIDMEM = save_vidmem;
			continue;
		}
		/* test with inverted pattern */
		*VIDMEM = 0x5555AAAA;
		if (*VIDMEM != 0x5555AAAA)
		{
			*VIDMEM = save_vidmem;
			continue;
		}
		/*
		 * If we get here, the memory seems to be writable/readable
		 * Now check if we aren't fooled by address wrapping (mirroring)
		 */
		fooled = FALSE;
		for (i = segment - 1; i >= 0; i--)
		{
			/* select the segment */
			ET6K_SETSEG(i);
			outb(0x3CB, (i & 0x30) | (i >> 4));
			outb(0x3CD, (i & 0x0f) | (i << 4));
			if (*VIDMEM == 0x5555AAAA)
			{
				/*
				 * Seems like address wrap, but there could of course be
				 * 0x5555AAAA in here by accident, so we check with another
				 * pattern again.
				 */
				ET6K_SETSEG(segment);
				/* test with other pattern again */
				*VIDMEM = 0xAAAA5555;
				ET6K_SETSEG(i);
				if (*VIDMEM == 0xAAAA5555)
				{
					/* now we're sure: this is not real memory */
					fooled = TRUE;
					break;
				}
			}
		}
		if (!fooled)
		{
			real_ram = (segment + 1) * SEGSIZE;
			break;
		}
		/* restore old contents again */
		ET6K_SETSEG(segment);
		*VIDMEM = save_vidmem;
	}

	/* restore original register contents */
	outb(0x3CD, oldSegSel1);
	outb(0x3CB, oldSegSel2);
	outb(0x3CE, 5);
	outb(0x3CF, oldGR5);
	outb(0x3CE, 6);
	outb(0x3CF, oldGR6);
	outb(0x3C4, 2);
	outb(0x3C5, oldSEQ2);
	outb(0x3C4, 4);
	outb(0x3C5, oldSEQ4);

	return real_ram;
}
