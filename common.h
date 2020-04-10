#ifndef _COMMON_H_
#define	_COMMON_H_
unsigned int pullbit (unsigned int data, unsigned int bitNumber);
unsigned int pullbits (unsigned int source, int lsb, int msb);
unsigned int populationCount (void *buffer, unsigned int byteCount);
char  	highestSetBit (void  *buffer, char  byteCount);
char 	*buildDataList (int argc, char *argv[], int dataSize, u64 *data, int *elements);
unsigned	samePattern (u64 *pattern, __u8 *buffer, unsigned patternSize);
char *hextoint64 (char *position, u64 *result, int maxDigits);
void printMultiChar (char *Buffer);

#endif
