#ifndef _MEMORY_H_
#define _MEMORY_H_

char *buildDataList (int argc, char *argv[], int dataSize, u64 *data, int *elements);
static void enterCmosByte (int index, int value);
static unsigned int readCmosByte (int index);
char *dumpCmos (void);
void clearCmos (void);
char *cmosEnter (int argc, char *argv []);
char *cmosFill (int argc, char *argv []);
char *hexAsciiDump (__u8 *mmIoBase, u64 physicalAddress, u64 count);
char *mapPhysicalMemory (void **virtualAddress, u64 physicalAddress, u64 size, int writable);
void unmapPhysical (u64 physicalAddress, u64 size);
static char *memDump8 (__u8 *mmIoBase, u64 physicalAddress, u64 count);
static char *memDump16 (__u16 *mmIoBase, u64 physicalAddress, u64 count);
static char *memDump32 (__u32 *mmIoBase, u64 physicalAddress, u64 count);
static char *memDump64 (u64 *mmIoBase, u64 physicalAddress, u64 count);
char *memEnter (int argc, char *argv [], int dataSize);
char *memDump (int argc, char *argv [], int dataSize, u64 *nextDumpAddress);
char *memFill (int argc, char *argv [], int dataSize);
char *memSearch (int argc, char *argv [], int match);
static int isAtmelOrSt (int chipId);
static char *waitForFlash (int chipId, volatile unsigned char *mmIoData, int offset);
static char *sectorEraseFlash (int chipId, volatile unsigned char *mmIoData, int byteOffset);
static char *blockEraseFlash (int chipId, volatile unsigned char *mmIoData, int byteOffset, int dryRun);
static char *setLockState (int deviceSize, volatile unsigned char *mmIoBase, int byteOffset, int value);
static char *softwareUnlockFlash (int chipId, int deviceSize, volatile unsigned char *mmIoBase, int byteOffset);
static char *softwareLockFlash (int chipId, int deviceSize, volatile unsigned char *mmIoBase, int byteOffset);
static void freeResources (void *newImage, void *oldImage, FILE *stream);
static char *byteProgramFlash (int chipId, volatile unsigned char *mmIoData, unsigned char *newImage, int byteOffset, int force);
static char *programFlashCore (int chipId, volatile unsigned char *mmIoData, unsigned char *newImage, int byteOffset, int count, int force);
static char *programFlash (int chipId, volatile unsigned char *mmIoData, unsigned char *newImage, int byteOffset, int count, int force, int *retries);
void writeSioConfig (unsigned int configPort, unsigned int reg, unsigned int value);
unsigned int readSioConfig (unsigned int configPort, unsigned int reg);
static char *find8111 (int *device);
static char *findIch7 (int *device);
static char *findHT1000 (int *device);
static char *findGromit (int *device);
static char *findNvidia (int *device);
static char *findMCP55 (int *device);
char *_flash (int argc, char *argv []);

char *readPciConfig (int bus, int device, int function, int offset, int bytesRequested, int *bytesReturned, __u32 *data);
char *writePciConfig (int bus, int device, int function, int offset, int bytes, __u32 data);
char *ioRead (int argc, char *argv [], int ioSize);
char *ioWrite (int argc, char *argv [], int ioSize);
static char *dumpFunctions (int bus, int device, int dataSize, int *busFound);
static char *dumpAllPci (int dataSize);
char *pciDump (int argc, char *argv [], int dataSize);
char *pciEnter (int argc, char *argv [], int dataSize);
//__u32 ReadIoPort(__u32 _port , int  _width );
//void WriteIoPort (__u32 _port, int _width, __u32 data);
u64 mem_remap(u64 base_addr, u64 map_size);
void unmapPhysical (u64 physicalAddress, u64 size);


#endif
