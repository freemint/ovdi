#ifndef	_OVDIFILE_H
#define _OVDIFILE_H

/* structure for getxattr */
struct xattr
{
	unsigned short	mode;
	long		index;
	unsigned short	dev;
	unsigned short	rdev;		/* "real" device */
	unsigned short	nlink;
	unsigned short	uid;
	unsigned short	gid;
	long		size;
	long		blksize;
	long		nblocks;
	unsigned short	mtime, mdate;
	unsigned short	atime, adate;
	unsigned short	ctime, cdate;
	short		attr;
	short		reserved2;
	long		reserved3[2];
};
typedef	struct xattr XATTR;

long get_file_size( char *filename );
long load_file( char *filename, long bytes, char *buffer);


#endif /* _OVDIFILE_H */
