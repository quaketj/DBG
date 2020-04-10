#ifndef __MMAP_H__
#define __MMAP_H__

typedef struct region {
	int index;
	unsigned long start_pfn;
	unsigned long end_pfn;
	int nid;
	int type;
} region_t;

#define MMTEST_MAGIC	'K'
#define GET_BREGION_NUM	_IOR(MMTEST_MAGIC, 1, int)
#define GET_BREGION	_IOWR(MMTEST_MAGIC, 2, region_t)
#define FREE_BREGIONS	_IO(MMTEST_MAGIC, 3)
#define FLUSH_CACHE	_IO(MMTEST_MAGIC, 4)

extern int get_bregion_num(void);
extern int get_bregion(int index, unsigned long *start_pfn, unsigned long *end_pfn, int *nid, int *type);
extern int set_bregion(int index, unsigned long start_pfn, unsigned long end_pfn, int nid, int type);

#endif //__MMAP_H__

