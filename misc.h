#ifndef _MISC_H_
#define _MISC_H_

#define CONFADD  0x0CF8
#define CONFDATA 0x0CFC

static __u32 pciConfigMask [] = {0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF};
int kbhit ( void );
char *hextoint64 (char *position, u64 *result, int maxDigits);
char *ioRead (int argc, char *argv [], int ioSize);
char *ioWrite (int argc, char *argv [], int ioSize);
char *pciDump (int argc, char *argv [], int dataSize);
char *pciEnter (int argc, char *argv [], int dataSize);
//__u32 ReadIoPort(__u32 _port , int  _width );
//void WriteIoPort (__u32 _port, int _width, __u32 data);


#endif
