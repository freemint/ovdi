#ifndef _inlines_h
#define _inlines_h

static inline unsigned short	BSw (unsigned short n);
static inline unsigned long	BSl (unsigned long n);

#define outb(p, ofset, value) { *(volatile unsigned char  *)(p + ofset) = (unsigned char)value;		}
#define outw(p, ofset, value) { *(volatile unsigned short *)(p + ofset) = BSw((unsigned short)value);	}
#define outl(p, ofset, value) { *(volatile unsigned long  *)(p + ofset) = BSl((unsigned long)value);	}

static volatile char *et6kptr = (volatile char *)0x803fff00UL;
	
static inline unsigned short
BSw(unsigned short n)
{
	register unsigned short ret __asm__ ("d0");

	ret = n;
	__asm__
	("
		ror.w	#8,%0
	"	:
		: "d"(ret)
	);
	return ret;
}

static inline unsigned long
BSl(unsigned long n)
{
	register unsigned long ret __asm__ ("d0");

	ret = n;
	__asm__
	("
		ror.w	#8,%0
		swap	%0
		ror.w	#8,%0
	"	:
		: "d"(ret)
	);
	return ret;
}

#define INb(p, ofset)						\
__extension__							\
({								\
	register unsigned char ret __asm__ ("d0");		\
								\
	ret = *(volatile unsigned char *)(p + ofset);		\
	ret;							\
})

#define INw(p, ofset)						\
__extension__							\
({								\
	register unsigned short ret __asm__ ("d0");		\
								\
	ret = *(volatile unsigned short *)(p + ofset);		\
	__asm__							\
	("							\
		ror.w #8,%0					\
	"	:						\
		: "d"(ret)					\
	);							\
	ret;							\
})

#define INl(p, ofset)						\
__extension__							\
({								\
	register unsigned long ret __asm__ ("d0");		\
								\
	ret = *(volatile unsigned long *)(p + ofset);		\
	__asm__							\
	("							\
		ror.w #8,%0	;				\
		swap %0		;				\
		ror.w #8,%0	;				\
	"	:						\
		: "d"(ret)					\
	);							\
	ret;							\
})

#define inb(p, ofset) (unsigned char)INb(p, ofset)
#define inw(p, ofset) (unsigned short)INw(p, ofset)
#define inl(p, ofset) (unsigned long)INl(p, ofset)

#endif /* _inlines_h */
