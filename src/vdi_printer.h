#ifndef _vdi_printer_h
#define _vdi_printer_h


struct prn_settings
{
	long		magic;			/**< 'pset' */
	long		length;			/**< Structure length */
	long		format;			/**< Structure type */
	long		reserved;		/**< TODO */

	long		page_flags; 	/**< Flags, inc. even pages, odd pages */
	short		first_page; 	/**< First page to be printed */
	short		last_page;		/**< Last page to be printed */
	short		no_copies;		/**< Number of copies */
	short		orientation;	/**< Orientation */
	long		scale;			/**< Scaling: 0x10000L corresponds to 100%  */

	short		driver_id;		/**< VDI device number */
	short		driver_type;	/**< Type of driver set */
	long		driver_mode;	/**< Flags, inc. for background printing */
	long		reserved1;		/**< Reserved */
	long		reserved2;		/**< Reserved */
	
	long		printer_id; 	/**< Printer number */
	long		mode_id; 		/**< Mode number */
	short		mode_hdpi;		/**< Horizontal resolution in dpi */
	short		mode_vdpi;		/**< Vertical resolution in dpi */
	long		quality_id; 	/**< Print mode (hardware-dependent quality,
                                     e.g. Microweave or Econofast) */

	long		color_mode; 	/**< Colour mode */
	long		plane_flags;	/**< Flags for colour planes to be output
                                     (e.g. cyan only) */
	long		dither_mode;	/**< Dither process */
	long		dither_value;	/**< Parameter for the dither process */

	long		size_id; 		/**< Paper format */
	long		type_id; 		/**< Paper type (normal, glossy) */
	long		input_id;		/**< Paper feed channel */
	long		output_id;		/**< Paper output channel */

	long		contrast;		/**< Contrast: 0x10000L corresponds to the
                                     normal setting */
	long		brightness; 	/**< Brightness: 0x1000L corresponds to the
                                     normal setting */
	long		reserved3;		/**< Reserved */
	long		reserved4;		/**< Reserved */

	long		reserved5;		/**< Reserved */
	long		reserved6;		/**< Reserved */
	long		reserved7;		/**< Reserved */
	long		reserved8;		/**< Reserved */

	char		device[128];	/**< File name to be printed  */

	struct	             		/**< Settings of the Mac printer driver */
	{
		char	inside[120];	/**< TODO */
	} mac_settings;             /**< Settings of the Mac printer driver */
};
typedef struct prn_settings PRN_SETTINGS;

#endif	/* _vdi_printer_h */
