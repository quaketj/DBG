#include "project.h"

typedef enum
{
   _system_unknown    ,
   _system_matrix     ,
   _system_blackbeard ,
   _system_k8we       ,
   _system_bridgeport ,
   _system_memphis    ,
   _system_thunderbolt,
   _system_concrete	  
}PLATFORM_TYPE;

typedef struct
{
   PLATFORM_TYPE type;
   int pmbase, smiControl, ioControlReg1, ioControlReg2, lpcDevice;
   int gromitRegD0, gromitDevice;
   int gpsel, gpcfg1, gpioSave, gpioBase, writeEnablePort, direction;
   int gpioWp, gpioTbl, saveWp, saveTbl;
   int save1, save2, save3;
}PLATFORM_INFO;



static void enterCmosByte (int index, int value)
{
      char *error;
      int device;
      int indexPort = 0x70, dataPort = 0x71;
      if (index >= 128)
         {
         index -= 128;
         indexPort = 0x72;
         dataPort = 0x73;
//	error = findMCP55(&device);
//       if(device != -1) index +=128;
         }
      outb_p(index,indexPort);
      outb_p(value, dataPort);
  
}

static unsigned int readCmosByte (int index)
{
      int indexPort = 0x70, dataPort = 0x71;
      if (index >= 128)
         {
         index -= 128;
         indexPort = 0x72;
         dataPort = 0x73;
         }
      outb_p(index, indexPort);
      return inb_p(dataPort);
}


char *dumpCmos (void)
{
   unsigned int index, data, sumlo = 0, sumhi = 0;
   for (index = 0; index < 256; index++)
      {
      if ((index % 16) == 0)
      	{
      	    scrollok(stdscr,TRUE);
            printw ("\n%02X:  ", index);
			scrollok(stdscr,FALSE);
      	} 	
      data = readCmosByte (index);
      printw ("%02X ", data);
      }
   			scrollok(stdscr,TRUE);
			printw ("\n");
			scrollok(stdscr,FALSE);
   return NULL;
}

void clearCmos (void)
{
	unsigned int index;

   	for (index = 0x10; index < 256; index++)
	{
    	if (index == 0x32) continue; // do not clear century byte
      		outb_p (index,0x72);
      		outb_p (0x0,0x73);
	}
}


char *cmosEnter (int argc, char *argv [])
{
	u64 data [MAX_ARGS], startAddress;
    int elements, index, endAddress;
    char *error;
  
    if (argc < 4) return formatMessage ("invalid argument count");
    error = hextoint64 (argv [2], &startAddress, 2);                 if (error) return error;
    error = buildDataList (argc - 3, &argv [3], 8, data, &elements); if (error) return error;
    endAddress = startAddress + elements - 1;
    for (index = 0; index < elements; index++)
		enterCmosByte (startAddress + index, data [index]);
    return NULL;
}
//---------------------------------------------------------------------------
char *cmosFill (int argc, char *argv [])
{
	u64 data [MAX_ARGS], startAddress, endAddress, bytesRemaining;
    int element, elements, index;
    char *error;

    if (argc < 5) return formatMessage ("invalid argument count");
    error = hextoint64 (argv [2], &startAddress, 2);  if (error) return error;
    error = hextoint64 (argv [3], &endAddress, 2);    if (error) return error;
   
    //debug & symdeb treat start address less than end address as an error. Here, treat end address as byte count instead
    if (endAddress < startAddress) endAddress = endAddress + startAddress - 1;
    bytesRemaining = endAddress - startAddress + 1;
    error = buildDataList (argc - 4, &argv [4], 8, data, &elements);
    if (error) return error;

    element = 0, index = 0;
  
    while (bytesRemaining)
	{
    	enterCmosByte (startAddress + index, data [element]);
      	element++, index++, bytesRemaining--;
      	if (element == elements) element = 0;
    }
   
    return NULL;
}


#define	INDEX_LEN	9
#define	ASCII_OFFSET	(16*3+2*2+INDEX_LEN)
char *hexAsciiDump (__u8 *mmIoBase, u64 physicalAddress, u64 count)
{
	u64 maxAddress = physicalAddress + count * sizeof mmIoBase [0] - 1;
   	int    addressColumns = (highestSetBit (&maxAddress, sizeof maxAddress)) / 4 + 1;

   // do hex-ascii dump
	//scrollok(stdscr,TRUE);
   	//printw("cnt:0x%x\n",count);
   	//printw("add:0x%x\n\n",physicalAddress);
   	//printw("colum:0x%x\n\n",addressColumns);
	//scrollok(stdscr,FALSE);
   	while (count)
	{
    	int offset=0, index;
      	char buffer [256];
      	memset (buffer, ' ', sizeof (buffer));
      	//sprintf (buffer, "%02X-",physicalAddress);
      	sprintf (buffer, "%08X-",physicalAddress);
        offset += INDEX_LEN;
      	//for (index = 0, offset = addressColumns - 2; index < 16; index++)
      	for (index = 0;index < 16; index++)
		{
        	unsigned char value = *mmIoBase++, ascii;
         	//offset += 3;
         	if (index == 8) offset += 2;
         	sprintf (buffer + offset, "%02X", value);
         	buffer [offset + 2] = ' '; /* remove NULL */
         	if (isprint (value))
            	ascii = value;
         	else
            	ascii = '.';
         	//buffer [index + addressColumns + 53] = ascii;  
         	buffer [index + ASCII_OFFSET] = ascii;  
			offset +=3;
         	physicalAddress++;
         	count--;
         	if (count == 0) break;
      	}
      	//buffer [index + addressColumns + 53] = '\0';
      	buffer [index + ASCII_OFFSET] = '\0';

	  	scrollok(stdscr,TRUE);
      	printw ("%s\n", buffer);
	  	scrollok(stdscr,FALSE);
      	if (kbhit ()) return formatMessage ("dump canceled");
   	}
   	return NULL;
}

#if 0
int nmemFile = 0;
char *mapPhysicalMemory (void **virtualAddress, u64 physicalAddress, u64 size, int writable)
{
  	unsigned char *mem_addr = NULL;
	int offset = 0;
	u64 membase;
	membase = physicalAddress & 0xfffffffffffff000;
	offset = physicalAddress & 0xfff;
	nmemFile = open("/dev/mem",O_RDWR);
	if (nmemFile != -1)
	{
		mem_addr = (unsigned char *)mmap(0,
			//0x1000000,
			0x100000,
			PROT_READ |PROT_WRITE,
			MAP_SHARED,
			nmemFile,
			membase);
		*virtualAddress = (void *) (mem_addr+offset);
		return NULL;
	}
	
	return formatMessage ("\nTry to malloc the memory fail!\n");
}

void unmapPhysical (u64 physicalAddress, u64 size)
{
	u64 membase;
	membase = physicalAddress & 0xfffffffffffff000;
	munmap(&membase, 0x1000);
	close(nmemFile);
}

#else

//#define PHYSICAL_ADDRESS_T 	u64
//#define SYSTEM_ADDRESS_T   	u64

#define	MMAP_SIZE	(0x100000) /* 1M */

u64 mmap_page_base;
u64 mmap_page_offset;
u64 mmap_map_base;
int mmap_fd;


u64 mem_remap(u64 base_addr, u64 map_size)
{
    void *map_addr;
    u64 addr;

    if (mmap_map_base != 0 &&
        mmap_page_base == (base_addr & ((u64) ~(MMAP_SIZE - 1)))) {
        return (mmap_map_base + ((u64)(base_addr & (MMAP_SIZE - 1))));
    }

    if (mmap_map_base != 0) {
        // unmap previous mapped page
        munmap((void*)mmap_map_base, MMAP_SIZE);
        mmap_map_base = (u64)NULL;
    } else {
        mmap_fd = open("/dev/wmem", O_RDWR | O_SYNC);
        if (mmap_fd < 0) {
            return 0;
        }
    }
    map_addr = mmap(0, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                            mmap_fd, (base_addr & ~(MMAP_SIZE - 1)));
    if (map_addr == NULL) {
        close(mmap_fd);
        return 0;
    }
    mmap_map_base = (u64) map_addr;
    mmap_page_base = base_addr & ~(MMAP_SIZE - 1);
    addr = mmap_map_base + ((u64)(base_addr & (MMAP_SIZE - 1)));

    return addr;
}

char *mapPhysicalMemory (void **virtualAddress, u64 physicalAddress, u64 size, int writable)
{
	u64 paddr;

	if((paddr = mem_remap(physicalAddress,size)) != (u64)NULL) {
		*virtualAddress = (void *)paddr;
		return NULL;
	} else {
		return formatMessage ("\nTry to malloc the memory fail!\n");
	}
}

void unmapPhysical (u64 physicalAddress, u64 size)
{
    if (mmap_fd != 0) {
		if(mmap_map_base != 0) {
        	munmap((void*)mmap_map_base, MMAP_SIZE);
        	mmap_map_base = (u64)NULL;
        	mmap_page_base = (u64)NULL;
		}
		close(mmap_fd);
		mmap_fd = 0;
	} else {
		formatMessage ("\nTry to unmalloc the memory fail!\n");
	}
}
#endif

static char *memDump8 (__u8 *mmIoBase, u64 physicalAddress, u64 count)
{
	// do hex-ascii dump
   	hexAsciiDump (mmIoBase, physicalAddress, count);
   	return NULL;
}

//---------------------------------------------------------------------------

static char *memDump16 (__u16 *mmIoBase, u64 physicalAddress, u64 count)
{
   u64 maxAddress = physicalAddress + count * sizeof mmIoBase [0] - 1;
   int    addressColumns = (highestSetBit (&maxAddress, sizeof maxAddress)) / 4 + 1;
   
   while (count)
      {
      int offset, index;
      char buffer [81];
      
      memset (buffer, ' ', sizeof (buffer));
      sprintf (buffer, "%02X-",physicalAddress);
      for (index = 0, offset = addressColumns - 4; index < 8; index++)
         {
         unsigned short value = *mmIoBase++;
		 offset += 5;
         if (index == 4) offset += 2;
         sprintf (buffer + offset, "%04X", value);
         buffer [offset + 4] = ' '; /* remove NULL */
         physicalAddress += 2;
         count--;
         if (count == 0) break;
         }
      buffer [offset + 4] = '\0';
	  scrollok(stdscr,TRUE);
      printw ("%s\n", buffer);
	  scrollok(stdscr,FALSE);
      if (kbhit ()) return formatMessage ("dump canceled");
      }
   return NULL;
}

//---------------------------------------------------------------------------
static char *memDump32 (__u32 *mmIoBase, u64 physicalAddress, u64 count)
{
   u64 maxAddress = physicalAddress + count * sizeof mmIoBase [0] - 1;
   int    addressColumns = highestSetBit (&maxAddress, sizeof maxAddress) / 4 + 1;
   int    itemsPerLine = 8;

   while (count)
      {
      int offset, index;
      char buffer [90];
      
      memset (buffer, ' ', sizeof (buffer));
      sprintf (buffer, "%02X-",physicalAddress);

      // prevent wrap in DOS 80-column display
      if (addressColumns >= 8) itemsPerLine = 4;

      for (index = 0, offset = addressColumns - 8; index < itemsPerLine; index++)
         {
         unsigned long value = *mmIoBase++;
         offset += 9;
         sprintf (buffer + offset, "%08lX", value);
         buffer [offset + 8] = ' '; /* remove NULL */
         physicalAddress += 4;
         count--;
         if (count == 0) break;
         }
      buffer [offset + 8] = '\0';
	  scrollok(stdscr,TRUE);
      printw ("%s\n", buffer);
	  scrollok(stdscr,FALSE);
      if (kbhit ()) return formatMessage ("dump canceled");
      }
   return NULL;
}

//---------------------------------------------------------------------------
static char *memDump64 (u64 *mmIoBase, u64 physicalAddress, u64 count)
{
	u64 maxAddress = physicalAddress + count * sizeof mmIoBase [0] - 1;
    int    addressColumns = (highestSetBit (&maxAddress, sizeof maxAddress)) / 4 + 1;

	while (count) 
    {
    	int offset, index;
      	char buffer [81];
      
      	memset (buffer, ' ', sizeof (buffer));
      	sprintf (buffer, "%02X-",physicalAddress);
      	for (index = 0, offset = addressColumns - 16; index < 4; index++)
        {
    //     __m64 value = *(volatile __m64 *) mmIoBase++;
     	 	u64 value = *mmIoBase++; 
         	offset += 17;
         	sprintf (buffer + offset, "%016X", value);
         	buffer [offset + 16] = ' '; /* remove NULL */
         	physicalAddress += 8;
         	count--;
         	if (count == 0) break;
         }
  //    _m_empty ();
      	buffer [offset + 16] = '\0';
	  	scrollok(stdscr,TRUE);
      	printw ("%s\n", buffer);
	  	scrollok(stdscr,FALSE);
      	if (kbhit ()) return formatMessage ("dump canceled");
	}
   	return NULL;
}


char* memEnter (int argc, char *argv [], int dataSize)
{
   u64 data [MAX_ARGS], startAddress;
   int    index, elements;
   char   *error;
   volatile __u8  *mmIoBase8;
   volatile __u16 *mmIoBase16;
   volatile __u32 *mmIoBase32;
   volatile u64 *mmIoBase64;

   if (argc < 4) return formatMessage ("invalid argument count");
   error = hextoint64 (argv [2], &startAddress, maxAddressDigits);         if (error) return error;
   error = buildDataList (argc - 3, &argv [3], dataSize, data, &elements); if (error) return error;
   error = mapPhysicalMemory ((void **)&mmIoBase8, startAddress, (u64)(elements * dataSize / 8), 1);
   if (error) return error;
   mmIoBase16 = (void *) mmIoBase8;
   mmIoBase32 = (void *) mmIoBase8;
   mmIoBase64 = (void *) mmIoBase8;
   if (dataSize == 8)
      for (index = 0; index < elements; index++)
         mmIoBase8 [index] = data [index];
   else if (dataSize == 16)
      for (index = 0; index < elements; index++)
         mmIoBase16 [index] = data [index];
   else if (dataSize == 32)
      for (index = 0; index < elements; index++)
         mmIoBase32 [index] = data [index];
   else if (dataSize == 64)
      for (index = 0; index < elements; index++)
	  	 mmIoBase64 [index] = data [index++];
         //*(volatile u64 *) (mmIoBase64 + index) = integerToMmx (data [index]);
 //  _m_empty ();
   unmapPhysical (startAddress, elements * dataSize / 8);
   return NULL;
}


char *memDump (int argc, char *argv [], int dataSize, u64 *nextDumpAddress)
{
	u64 startAddress, endAddress, transferCount;
   	char   *error;
   	void   *mmIoBase;
	unsigned char val;
	unsigned char val123;


   	if (argc == 2) 
	{
    	startAddress = *nextDumpAddress;
      	transferCount = 128 * 8 / dataSize;
    } else if (argc == 3) {
    	error = hextoint64 (argv [2], &startAddress, maxAddressDigits);  if (error) return error;
    	transferCount = 128 * 8 / dataSize;
   	} else if (argc == 4) {
    	error = hextoint64 (argv [2], &startAddress, maxAddressDigits);  if (error) return error;
      	error = hextoint64 (argv [3], &endAddress, maxAddressDigits);    if (error) return error;
      	if (endAddress < startAddress) endAddress = endAddress + startAddress - 1;
      	transferCount = (endAddress - startAddress + 1) * 8 / dataSize;
   	} else return formatMessage ("INvalid argument count");

  	*nextDumpAddress = startAddress + transferCount * dataSize / 8;
   	error = mapPhysicalMemory (&mmIoBase, startAddress, (__u32) transferCount * dataSize / 8, 0);
   	if (error) return error;
   	if (dataSize == 8) 
    	error = memDump8 (mmIoBase, startAddress, transferCount);
   	else if (dataSize == 16)
    	error = memDump16 (mmIoBase, startAddress, transferCount);
   	else if (dataSize == 32)
    	error = memDump32 (mmIoBase, startAddress, transferCount);
   	else if (dataSize == 64)
    	error = memDump64 (mmIoBase, startAddress, transferCount);
   	unmapPhysical (startAddress,transferCount * dataSize / 8);
   	return error;
}

//---------------------------------------------------------------------------

char *memFill (int argc, char *argv [], int dataSize)
   {
   u64 data [MAX_ARGS], startAddress, endAddress, offset, maxMap = 0x10000000;
   long  bytesRemaining;
   int    index, elements, element;
   char   *error;
   void   *mmIoBase;

   if (argc < 5) return formatMessage ("invalid argument count");
   error = hextoint64 (argv [2], &startAddress, maxAddressDigits);  if (error) return error;
   error = hextoint64 (argv [3], &endAddress, maxAddressDigits);    if (error) return error;
   
   // debug & symdeb treat start address less than end address as an error. Here, treat end address as byte count instead
   if (endAddress < startAddress) endAddress = endAddress + startAddress - 1;
   bytesRemaining = endAddress - startAddress + 1;
   error = buildDataList (argc - 4, &argv [4], dataSize, data, &elements);
   if (error) return error;

   element = 0;
   offset = 0;

   while (bytesRemaining > 0)
      {
       u64 mapSize = min (bytesRemaining, maxMap);
       u64 transferCount = mapSize * 8 / dataSize;

      error = mapPhysicalMemory (&mmIoBase, startAddress + offset, mapSize, 1);
      if (error) return error;
      if (dataSize == 8)
         {
         u8 *mmIoBase8 = mmIoBase;
         for (index = 0; index < transferCount; index++)
            {
            mmIoBase8 [index] = data [element++];
            if (element == elements) element = 0;
            }
         }
      else if (dataSize == 16)
         {
         u16 *mmIoBase16 = mmIoBase;
         for (index = 0; index < transferCount; index++)
            {
            mmIoBase16 [index] = data [element++];
            if (element == elements) element = 0;
            }
         }
      else if (dataSize == 32)
         {
         u32 *mmIoBase32 = mmIoBase;
         for (index = 0; index < transferCount; index++)
            {
            mmIoBase32 [index] = data [element++];
            if (element == elements) element = 0;
            }
         }
      else if (dataSize == 64)
         {
         u64 *mmIoBase64 = mmIoBase;
         for (index = 0; index < transferCount; index++)
            {
            mmIoBase64 [index] = data [element++];
            //_mm_stream_pi ((__m64*)&mmIoBase64 [index], *(__m64*)&data [element++]);
            if (element == elements) element = 0;
            }
        // _mm_empty ();
         }
      unmapPhysical (startAddress + offset, mapSize);
      offset += mapSize;
      bytesRemaining -= mapSize;
      if (kbhit ()) return formatMessage ("fill canceled");
      }
   return NULL;
   }


char *memSearch (int argc, char *argv [], int match)
{
   u64 startAddress, endAddress;
   int    index, elements, maxMap;
   char   *error;
   __u8  *mmIoBase;
   u64 bytesRemaining, offset = 0;
   u64 pattern [MAX_ARGS];



   maxMap = 0x10000; // make small to minimize skipped ranges, but big enough to avoid excessive map calls
   
   if (argc < 5 || argc > DIMENSION (pattern) + 4) return formatMessage ("incorrect argument count");
   error = hextoint64 (argv [2], &startAddress, maxAddressDigits);  if (error) return error;
   error = hextoint64 (argv [3], &endAddress, maxAddressDigits);    if (error) return error;
   
   // debug & symdeb treat start address less than end address as an error. Here, treat end address as byte count instead
   if (endAddress < startAddress) endAddress = endAddress + startAddress - 1;
   bytesRemaining = endAddress - startAddress + 1;
   error = buildDataList (argc - 4, &argv [4], 8, pattern, &elements);
   if (error) return error;
   while (bytesRemaining)
   {
		u64 mapSize = (__u32) min (bytesRemaining, maxMap);
       	error = mapPhysicalMemory ((void **)&mmIoBase, startAddress + offset, mapSize + elements, 0);
      	if (!error) {
        	for (index = 0; index < mapSize; index++) {
            	if (samePattern (pattern, mmIoBase + index, elements) == match) {
               		scrollok(stdscr,TRUE);
                	printw ("%08X\n", startAddress + offset + index);
			   		scrollok(stdscr,FALSE);
                	usleep (200); // give time to cancel
               		if (kbhit ()) return formatMessage ("search canceled");
            	}
        	}
        	unmapPhysical (startAddress + offset, mapSize + elements);
      	} else printw ("(%s, skipping)\n", error);
      	if (kbhit ()) return formatMessage ("search canceled");
      	offset += mapSize;
      	bytesRemaining -= mapSize;
   }
   return NULL;
}

//---------------------------------------------------------------------------

static void resetFlash (volatile unsigned char *mmIoData)
   {
   mmIoData [0x5555] = 0xAA;
   mmIoData [0x2AAA] = 0x55;
   mmIoData [0x5555] = 0xF0;
   }

//---------------------------------------------------------------------------

static int isAtmelOrSt (int chipId)
   {
   return chipId == 0x1FE1 || chipId == 0x202F || chipId == 0x202D || chipId == 0x2080 || chipId == 0x2081 || chipId == 0x202C || chipId == 0x202E || chipId == 0xBF4C;
   }

//---------------------------------------------------------------------------

static char *waitForFlash (int chipId, volatile unsigned char *mmIoData, int offset)
   {
   int previous, consecutive = 0, loops = 0;
   
   if (isAtmelOrSt (chipId))
      {
      int error, status;
      mmIoData [0] = 0x70;
      for (;;)
         {
         status = mmIoData [0];
         if (pullbit (status, 7) == 1) break; // wait for ready status
         }
      error = status & 0x7E;
      //*QUK resetFlash (mmIoData);
	  mmIoData [0] = 0xFF; // Read-Array --Reset *QUK 
      mmIoData [0] = 0x50; // clear status for nexrt time
      if (error) printw ("error %02X\n", error);
      return NULL;
      }

   previous = mmIoData [offset];
   for (;;)
      {
      int temp, diff;

      temp = mmIoData [offset];
      diff = (temp ^ previous) & 0x40;
      previous = temp;
      loops++;

      if (diff) consecutive = 0;
      if (!diff) consecutive++;
      //if (consecutive == 3) printf ("%u loops\n", loops);
      if (consecutive == 3) break;
      if (loops == 1000000) return formatMessage ("write complete status timeout");
      }
   return NULL;
   }

//---------------------------------------------------------------------------

static char *sectorEraseFlash (int chipId, volatile unsigned char *mmIoData, int byteOffset)
   {
   int eraseCommand [2] = {0x30, 0x50};
   if (isAtmelOrSt (chipId))
      {
	  mmIoData [byteOffset] = 0x20;
	  mmIoData [byteOffset] = 0xD0;
      }
   else
      {
      
      mmIoData [0x5555] = 0xAA;
      mmIoData [0x2AAA] = 0x55;
      mmIoData [0x5555] = 0x80;
      mmIoData [0x5555] = 0xAA;
      mmIoData [0x2AAA] = 0x55;
      mmIoData [byteOffset] = eraseCommand [chipId == 0xDAD3 || chipId == 0xDA34];
      }
   // wait for operation complete
   return waitForFlash (chipId, mmIoData, byteOffset);
   }

//---------------------------------------------------------------------------

static char *blockEraseFlash (int chipId, volatile unsigned char *mmIoData, int byteOffset, int dryRun)
   {
   int eraseCommand [2] = {0x50, 0x30};
   if (dryRun) return NULL;

   if (chipId == 0x01F6)
      {
      mmIoData [0xAAAA] = 0xAA;
      mmIoData [0x5555] = 0x55;
      mmIoData [0xAAAA] = 0x80;
      mmIoData [0xAAAA] = 0xAA;
      mmIoData [0x5555] = 0x55;
      mmIoData [byteOffset] = 0x30;
      }
   else 
      {
      mmIoData [0x5555] = 0xAA;
      mmIoData [0x2AAA] = 0x55;
      mmIoData [0x5555] = 0x80;
      mmIoData [0x5555] = 0xAA;
      mmIoData [0x2AAA] = 0x55;
      mmIoData [byteOffset] = eraseCommand [chipId == 0xDAD3 || chipId == 0xDA34 || chipId == 0xDA50];

      }
   // wait for operation complete
   return waitForFlash (chipId, mmIoData, byteOffset);
   }


//---------------------------------------------------------------------------

static char *setLockState (int deviceSize, volatile unsigned char *mmIoBase, int byteOffset, int value)
   {
   int temp, offset, retry;
   int chipId=1;

   for (retry = 0; retry < 5; retry++)
      {
      offset = 0xC00002 - deviceSize + byteOffset;
	  mmIoBase [offset] = value;
	  // Add the below unlock sector for SST49LF160C
	  mmIoBase [0xBF8002] = value;
	  mmIoBase [0xBFA002] = value;
	  mmIoBase [0xBFC002] = value;   
      temp = mmIoBase [offset];
      temp &= 7;
      value &= 7;
      if (temp == value) break;
      scrollok(stdscr,TRUE);
      printw ("offset %x lock, write %02Xh, read %02X\n", byteOffset, value, temp);
      scrollok(stdscr,FALSE);
      return NULL;
      }


   return NULL;
   }

static char *softwareUnlockFlash (int chipId, int deviceSize, volatile unsigned char *mmIoBase, int byteOffset)
   {

   if (chipId == 0x01F6) return NULL; // not needed or supported on Memphis
   return setLockState (deviceSize, mmIoBase, byteOffset, 0);
   }

//---------------------------------------------------------------------------

static char *softwareLockFlash (int chipId, int deviceSize, volatile unsigned char *mmIoBase, int byteOffset)
   {
   return NULL;  //*QUK workaround for all project, flash BIOS will lead to AMIDEDOS couldn't flash serial number
   if (chipId == 0x01F6) return NULL; // not needed or supported on Memphis
   return setLockState (deviceSize, mmIoBase, byteOffset, 1);
   }


//------------------------------------------------------------------------------------


static void freeResources (void *newImage, void *oldImage, FILE *stream)
   {
   if (newImage) free (newImage);
   if (oldImage) free (oldImage);
   if (stream) fclose (stream);
   }

//---------------------------------------------------------------------------

static char *byteProgramFlash (int chipId, volatile unsigned char *mmIoData, unsigned char *newImage, int byteOffset, int force)
   {
   if (mmIoData [byteOffset] == newImage [byteOffset] && force == 0) return NULL;
   if (isAtmelOrSt (chipId))
      {
      mmIoData [byteOffset] = 0x40;
      mmIoData [byteOffset] = newImage [byteOffset];
      }
   else if (chipId == 0x01F6)
      {
      mmIoData [0xAAAA] = 0xAA;
      mmIoData [0x5555] = 0x55;
      mmIoData [0xAAAA] = 0xA0;
      mmIoData [byteOffset] = newImage [byteOffset];
      }
   else
      {
      mmIoData [0x5555] = 0xAA;
      mmIoData [0x2AAA] = 0x55;
      mmIoData [0x5555] = 0xA0;
      mmIoData [byteOffset] = newImage [byteOffset];
      }

   // wait for operation complete
   return waitForFlash (chipId, mmIoData, byteOffset);
   }

//---------------------------------------------------------------------------

static char *programFlashCore (int chipId, volatile unsigned char *mmIoData, unsigned char *newImage, int byteOffset, int count, int force)
   {
   int index;
   char *error;

   for (index = 0; index < count; index++)
      {
      error = byteProgramFlash (chipId, mmIoData, newImage, byteOffset + index, force);
      if (error) return error;
      }

   resetFlash (mmIoData);
   for (index = 0; index < count; index++)
      if (mmIoData [byteOffset + index] != newImage [byteOffset + index]) 
      	{
	  printw ("verify error, offset=%X expected=%02X actual=%02X", byteOffset + index, newImage [byteOffset + index], mmIoData [byteOffset + index]);
	  return NULL;
	  }
   return NULL;
   }

//---------------------------------------------------------------------------

static char *programFlash (int chipId, volatile unsigned char *mmIoData, unsigned char *newImage, int byteOffset, int count, int force, int *retries)
   {
   int retry;
   char *error;

   for (retry = 0; retry < 5; retry++)
      {
      error = programFlashCore (chipId, mmIoData, newImage, byteOffset, count, force);
      if (!error) break;
      }
   *retries = retry;
   return error;
   }


//----------------------------------------------------------------------------

void writeSioConfig (unsigned int configPort, unsigned int reg, unsigned int value)
   {
   WriteIoPort (configPort + 0, 8, reg);
   WriteIoPort (configPort + 1, 8, value);
   }

//----------------------------------------------------------------------------

unsigned int readSioConfig (unsigned int configPort, unsigned int reg)
   {
   WriteIoPort (configPort + 0, 8, reg);
   return (unsigned char) ReadIoPort (configPort + 1, 8);
   }


//---------------------------------------------------------------------------


static char *find8111 (int *device)
   {
   int index;
   unsigned int bytesReturned;
   __u32 pcireg;
   
   *device = -1;
   // find the 8111 LPC bridge device
   for (index = 0; index < 32; index++)
      {
      char *error = readPciConfig (0, index, 0, 0, 4, &bytesReturned, &pcireg);
      if (error) return error;
      if (pcireg == 0x74681022) break;
      }
   if (index != 32) *device = index;
   return NULL;
   }

//---------------------------------------------------------------------------

static char *findIch7 (int *device)
   {
   int index;
   unsigned int bytesReturned;
   __u32 pcireg;
   
   *device = -1;
   // find the ICH7 LPC bridge device
   for (index = 0; index < 32; index++)
      {
      char *error = readPciConfig (0, index, 0, 0, 4, &bytesReturned, &pcireg);
      if (error) return error;
      if (pcireg == 0x27B08086 || pcireg == 0x27B88086) break;
      }
   if (index != 32) *device = index;
   return NULL;
   }

//---------------------------------------------------------------------------

static char *findHT1000 (int *device)
   {
   int index;
   unsigned int bytesReturned;
   __u32 pcireg;
   
   *device = -1;
   // find the HT1000 LPC bridge device
   for (index = 0; index < 32; index++)
      {
      char *error = readPciConfig (0, index, 0, 0, 4, &bytesReturned, &pcireg);
      if (error) return error;
      if (pcireg == 0x02051166) break;
      }
   if (index != 32) *device = index;
   return NULL;
   }

//---------------------------------------------------------------------------

static char *findGromit (int *device)
   {
   int index;
   unsigned int bytesReturned;
   __u32 pcireg;
   
   *device = -1;
   // find Compaq Gromit
   for (index = 0; index < 32; index++)
      {
      char *error = readPciConfig (0, index, 0, 0, 4, &bytesReturned, &pcireg);
      if (error) return error;
      if (pcireg == 0xB2030E11) break;
      }
   if (index != 32) *device = index;
   return NULL;
   }

//---------------------------------------------------------------------------

static char *findNvidia (int *device)
   {
   int index;
   unsigned int bytesReturned;
   __u32 pcireg;
   
   *device = -1;
   // find the NVIDIA LPC bridge device
   for (index = 0; index < 32; index++)
      {
      char *error = readPciConfig (0, index, 0, 0, 4, &bytesReturned, &pcireg);
      if (error) return error;
      if (pcireg == 0x005110DE) break;
      }
   if (index != 32) *device = index;
   return NULL;
   }

//---------------------------------------------------------------------------

static char *findMCP55 (int *device)
   {
   int index;
   unsigned int bytesReturned;
   __u32 pcireg;
   
   *device = -1;
   // find the NVIDIA MPC55 LPC bridge device
   for (index = 0; index < 32; index++)
      {
      char *error = readPciConfig (0, index, 0, 0, 4, &bytesReturned, &pcireg);
      if (error) return error;
      if (pcireg == 0x036010DE) break;
      }
   if (index != 32) *device = index;
   return NULL;
   }

//---------------------------------------------------------------------------

static char *findESB2 (int *device)
   {
   int index;
   unsigned int bytesReturned;
   __u32 pcireg;
   
   *device = -1;
   // find the ESB2 LPC bridge device
   for (index = 0; index < 32; index++)
      {
      char *error = readPciConfig (0, index, 0, 0, 4, &bytesReturned, &pcireg);
      if (error) return error;
      if (pcireg == 0x26708086) break;
      }
   if (index != 32) *device = index;
   return NULL;
   }

//---------------------------------------------------------------------------

static allFF (void *buffer, size_t size)
   {
   unsigned char *position = buffer;
   size_t index;

   for (index = 0; index < size; index++)
      if (position [index] != 0xFF) return 0;
   return 1;
   }

char *identifySystem (PLATFORM_INFO *platformInfo)
   {
   char *error;
   int device;

   platformInfo->type = _system_unknown;

   // find system type from SMBIOS
   error = find8111 (&device);
   if (error) return error;
   if (device != -1) platformInfo->type = _system_matrix;

   error = findNvidia (&device);
   if (error) return error;
   if (device != -1) platformInfo->type = _system_k8we;

   error = findESB2 (&device);
   if (error) return error;
   if (device != -1) platformInfo->type = _system_bridgeport;

   error = findHT1000 (&device);
   if (error) return error;
   if (device != -1) platformInfo->type = _system_concrete;

   error = findIch7 (&device);
   if (error) return error;
   if (device != -1) platformInfo->type = _system_blackbeard;
   
   error = findMCP55 (&device);
   if (error) return error;
   if (device != -1) platformInfo->type = _system_thunderbolt;
   
   return NULL;
   }
//---------------------------------------------------------------------------

char *prepareForFlash (PLATFORM_INFO *platformInfo,int verbose)
{
   unsigned int bytesReturned;
   __u32 pcireg;
   int device;
   char *error;

   if (platformInfo->type == _system_matrix)
      {
      error = find8111 (&device);
      if (error) return error;
      if (device == -1) return formatMessage ("AMD8111 not found");
      if (verbose) printw ("AMD8111 found at device %Xh\n", device);
      platformInfo->lpcDevice = device; // save for cleanup
      
      // setup for full flash access with write enabled
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0x40, 4, &bytesReturned, &pcireg);
      if (error) return error;
      platformInfo->ioControlReg1 = pcireg;
      pcireg |= 1;          // enable LPC ROM write
      pcireg |= 0xC0000000; // map upper 5 MB
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x40, 4, pcireg);
      if (error) return error;

      // turn off SMI, this may not be necessary but why not
      error = readPciConfig (0, platformInfo->lpcDevice, 3, 0x58, 4, &bytesReturned, &platformInfo->pmbase);
      if (error) return error;
      platformInfo->pmbase &= 0xFF00;
      if (verbose) printw ("AMD8111 pmbase: %Xh\n", platformInfo->pmbase);
      platformInfo->smiControl = ReadIoPort (platformInfo->pmbase + 0x2C, 16);
      // skip SMI disable until the restore code works in all cases
      ////WriteIoPort (platformInfo->pmbase + 0x2C, 16, platformInfo->smiControl & ~1);
   
      // set the chip enable gpio (20) and the write enable gpio (21)
      {
      int portNumber, gpioBase, reg, id;
      static char outputIndex [] = {0, 2, 6, 8, 10, 14, 16}; // table 25, page 72

      id = readSioConfig (0x4E, 0x20);
      if (id != 0xEE) return formatMessage ("NS417 super I/O ID=%02Xh, expected EEh");
      writeSioConfig (0x4E, 7, 7); // select gpio logical device
      platformInfo->gpsel  = readSioConfig (0x4E, 0xF0);
      platformInfo->gpcfg1 = readSioConfig (0x4E, 0xF1);
      gpioBase = readSioConfig (0x4E, 0x61) + readSioConfig (0x4E, 0x60) * 256;
      if (!gpioBase) return formatMessage ("NS417 gpio base is not set");
      if (verbose) printf ("gp I/O base: %04Xn\n", gpioBase);
      portNumber = 2; // port for gpio 20 and 21 (not documented)
      
      writeSioConfig (0x4E, 0xF0, 0x20);
      writeSioConfig (0x4E, 0xF1, 3); // setup gpio 20 (CE#, VSS) as output, push-pull

      writeSioConfig (0x4E, 0xF0, 0x21);
      writeSioConfig (0x4E, 0xF1, 3); // setup gpio 21 (WP#) as output, push-pull

      platformInfo->writeEnablePort = gpioBase + outputIndex [portNumber];
      platformInfo->gpioSave = ReadIoPort (platformInfo->writeEnablePort, 8);
      reg = platformInfo->gpioSave;
      reg |= (0 << 0); // GPIO 20: CE# (VSS for SST49LF004B), keep low always (chip always enabled)
      reg |= (1 << 1); // GPIO 21: WP#, temporarily make high to allow write
      WriteIoPort (platformInfo->writeEnablePort, 8, reg);
      }
      }

   if (platformInfo->type == _system_blackbeard)
      {
      error = findIch7 (&device);
      if (error) return error;
      if (device == -1) return formatMessage ("ICH7 not found");
      if (verbose) printw ("ICH7 found at device %Xh\n", device);
      platformInfo->lpcDevice = device; // save for cleanup
      
      // setup for full flash access with write enabled
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0xDC, 4, &bytesReturned, &pcireg);
      if (error) return error;
      platformInfo->ioControlReg1 = pcireg;
      pcireg |= 1;          // enable LPC ROM write
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0xDC, 4, pcireg);
      
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0xD8, 4, &bytesReturned, &pcireg);
      if (error) return error;
      platformInfo->ioControlReg2 = pcireg;
      pcireg |= 0xC000;          // map top 5MB
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0xD8, 4, pcireg);
      if (error) return error;
      
      // turn off BIOS write protect (GPIO14), also, make sure is is set to output
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0x48, 4, &bytesReturned, &platformInfo->gpioBase);
      if (error) return error;
      platformInfo->gpioBase &= 0xFFFE;
      platformInfo->direction = ReadIoPort (platformInfo->gpioBase + 0x04, 32);
      platformInfo->gpioSave = ReadIoPort (platformInfo->gpioBase + 0x0C, 32);
      WriteIoPort (platformInfo->gpioBase + 0x04, 32, platformInfo->direction & ~(1 << 14)); // make sure output
      WriteIoPort (platformInfo->gpioBase + 0x0C, 32, platformInfo->gpioSave | (1 << 14));   // set output value
      }

   if (platformInfo->type == _system_k8we)
      {
      error = findNvidia (&device);
      if (error) return error;
      if (device == -1) return formatMessage ("NVIDIA not found");
      if (verbose) printw ("NVIDIA MCP found at device %Xh\n", device);
      platformInfo->lpcDevice = device; // save for cleanup
      
      // setup for full flash access with write enabled
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0x6C, 4, &bytesReturned, &pcireg);
      if (error) return error;
      platformInfo->ioControlReg1 = pcireg;
      pcireg |= 0x100;          // enable LPC ROM write
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x6C, 4, pcireg);
      if (error) return error;
      }

   if (platformInfo->type == _system_concrete)
      {
      error = findHT1000 (&device); // look for HT1000 legacy device
      if (error) return error;
      if (device == -1) return formatMessage ("HT1000 not found");
      if (verbose) printw ("HT1000 found at device %Xh\n", device);
      platformInfo->lpcDevice = device; // save for cleanup
      
      // setup for full flash access with write enabled
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0x43, 1, &bytesReturned, &pcireg);
      if (error) return error;
      platformInfo->ioControlReg1 = pcireg;
      pcireg |= (1 << 4);          // set bit 4 (flash write enable) of additional feature control register
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x43, 1, pcireg);
      if (error) return error;
      
      // set LPC DMA address decoder control register 1 (don't know why, but AMI does it)
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0x48, 1, &bytesReturned, &pcireg);
      if (error) return error;
      platformInfo->ioControlReg2 = pcireg;
      pcireg = 1;
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x48, 1, pcireg);
      if (error) return error;


	  platformInfo->gpioBase = 0x0CD6;
	   WriteIoPort (platformInfo->gpioBase, 8, 0x46); // make sure output
      platformInfo->gpioSave = ReadIoPort (platformInfo->gpioBase+1 , 8);
      WriteIoPort (platformInfo->gpioBase, 8, 0x46); // make sure output
      WriteIoPort (platformInfo->gpioBase + 1, 8, platformInfo->gpioSave | (1 << 2));   // set output value  for WP#

	  WriteIoPort (platformInfo->gpioBase, 8, 0x44); // make sure output
	  platformInfo->gpioSave = ReadIoPort (platformInfo->gpioBase +1 , 8);
      WriteIoPort (platformInfo->gpioBase, 8, 0x44); // make sure output
      WriteIoPort (platformInfo->gpioBase + 1, 8, platformInfo->gpioSave | (1 << 7));   // set output value  for TBL#
      }

   if (platformInfo->type == _system_bridgeport)
      {
      error = findESB2 (&device);
      if (error) return error;
      if (device == -1) return formatMessage ("ESB2 not found");
      if (verbose) printw ("ESB2 LPC found at device %Xh\n", device);
      platformInfo->lpcDevice = device; // save for cleanup
      // setup for full flash access with write enabled
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0xD0, 4, &bytesReturned, &platformInfo->save1);
      if (error) return error;
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0xD8, 2, &bytesReturned, &platformInfo->save2);
      if (error) return error;
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0xDC, 1, &bytesReturned, &platformInfo->save3);
      if (error) return error;
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0xD0, 4, 0x1122);
      if (error) return error;
      // send all possible memory mapped I/O address space ranges to LPC bus (flash chip)
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0xD8, 2, 0xFFCF);
      if (error) return error;
      // set BIOS Write Enable (BIOSWE)
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0xDC, 1, platformInfo->save3 | 1);

      // turn off BIOS write protect (GPIO20), also, make sure is is set to output
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0x48, 4, &bytesReturned, &platformInfo->gpioBase);
      if (error) return error;
      platformInfo->gpioBase &= 0xFFFE;
      platformInfo->direction = ReadIoPort (platformInfo->gpioBase + 0x04, 32);
      platformInfo->gpioSave = ReadIoPort (platformInfo->gpioBase + 0x0C, 32);
      WriteIoPort (platformInfo->gpioBase + 0x04, 32, platformInfo->direction & ~(1 << 20)); // make sure output
      WriteIoPort (platformInfo->gpioBase + 0x0C, 32, platformInfo->gpioSave | (1 << 20));   // set output value
      if (error) return error;
      }

   if (platformInfo->type == _system_thunderbolt)
     { 
      platformInfo->gpioTbl = 2; // from schematic (1-64)
      platformInfo->gpioWp = 45;  // from schematic (1-64)
      error = findMCP55 (&device);
      if (error) return error;
      if (device == -1) return formatMessage ("MCP55 not found");
      if (verbose) printw ("MCP55 LPC found at device %Xh\n", device);
      platformInfo->lpcDevice = device; // save for cleanup
      
      // setup for full flash access with write enabled
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0x6C, 4, &bytesReturned, &platformInfo->save1);
      if (error) return error;
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0x8C, 4, &bytesReturned, &platformInfo->save2);
      if (error) return error;
      error = readPciConfig (0, platformInfo->lpcDevice, 0, 0x90, 4, &bytesReturned, &platformInfo->save3);
      if (error) return error;
      // allow BIOS memory writes to LPC bus devices (RWR_ENABLE)
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x6C, 4, platformInfo->save1 | (1 << 8));
      if (error) return error;
      // make sure that FF800000-FFFFFFFF is mapped
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x8C, 4, platformInfo->save2 | 0xFF);
      if (error) return error;
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x90, 4, platformInfo->save3 | 0x7F00);
      if (error) return error;      
      // turn off BIOS write protect (GPIO45) and top block lock
      error = readPciConfig (0, platformInfo->lpcDevice, 1, 0x64, 4, &bytesReturned, &pcireg);
      if (error) return error;
      platformInfo->gpioBase = pullbits (pcireg, 8, 15) * 256 + 0xC0 - 1; // first gpio is 1, so subtract 1 (24BF)
      platformInfo->saveWp = ReadIoPort (platformInfo->gpioBase + platformInfo->gpioWp, 8);
      platformInfo->saveTbl = ReadIoPort (platformInfo->gpioBase + platformInfo->gpioTbl, 8);
      WriteIoPort (platformInfo->gpioBase + platformInfo->gpioWp, 8, 5);  // set output value to high (no write protect)
      WriteIoPort (platformInfo->gpioBase + platformInfo->gpioTbl, 8, 5); // set output value to high (no top block lock)
      if (error) return error;
      }

   return NULL;

   }

//---------------------------------------------------------------------------

static char *cleanupAfterFlash (PLATFORM_INFO *platformInfo, volatile unsigned char *mmIoBase)
   {
   // get flash into known state (reset, exit ID mode)
   resetFlash (mmIoBase + 0x00FC0000);
 
   if (platformInfo->type == _system_matrix)
      {
      // restore SMI
      WriteIoPort (platformInfo->pmbase + 0x2C, 16, platformInfo->smiControl);

      // restore the GPIOs
      WriteIoPort (platformInfo->writeEnablePort, 8, platformInfo->gpioSave);

      // restore LPC flash settings
      return writePciConfig (0, platformInfo->lpcDevice, 0, 0x40, 4, platformInfo->ioControlReg1);
      }

   if (platformInfo->type == _system_blackbeard)
      {
      char *error;
 
      // put write protect gpio back the way it was
      WriteIoPort (platformInfo->gpioBase + 0x0C, 32, platformInfo->gpioSave);
      
      // restore LPC flash settings
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0xDC, 4, platformInfo->ioControlReg1);
      if (error) return error;
      return writePciConfig (0, platformInfo->lpcDevice, 0, 0xD8, 4, platformInfo->ioControlReg2);
      }

   if (platformInfo->type == _system_concrete)
      {
      char *error;
 
      // put write protect gpio back the way it was
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x43, 1, platformInfo->ioControlReg1);
      if (error) return error;
      
      // put Gromit back the way it was
      //error = writePciConfig (0, platformInfo->gromitDevice, 0, 0xD0, 1, platformInfo->gromitRegD0);
      //if (error) return error;

      // put LPC DMA address decoder control register 1 back the way it was
      return writePciConfig (0, platformInfo->lpcDevice, 0, 0x48, 1, platformInfo->ioControlReg2);

	  platformInfo->gpioBase = 0x0CD6;
	  WriteIoPort (platformInfo->gpioBase, 8, 0x46); // make sure output
      platformInfo->gpioSave = ReadIoPort (platformInfo->gpioBase+1 , 8);
      WriteIoPort (platformInfo->gpioBase, 8, 0x46); // make sure output
      WriteIoPort (platformInfo->gpioBase + 1, 8, platformInfo->gpioSave & 0x0FB);   // set output value  for WP#

	  WriteIoPort (platformInfo->gpioBase, 8, 0x44); // make sure output
	  platformInfo->gpioSave = ReadIoPort (platformInfo->gpioBase +1 , 8);
      WriteIoPort (platformInfo->gpioBase, 8, 0x44); // make sure output

      WriteIoPort (platformInfo->gpioBase + 1, 8, platformInfo->gpioSave & 0x7F);   // set output value  for TBL#
   
   }

   if (platformInfo->type == _system_bridgeport)
      {
      char *error;
 
      // put write protect gpio back the way it was
      WriteIoPort (platformInfo->gpioBase + 0x0C, 32, platformInfo->gpioSave);
      
      // restore LPC flash settings
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0xD0, 4, platformInfo->save1);
      if (error) return error;
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0xD8, 2, platformInfo->save2);
      if (error) return error;
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0xDC, 1, platformInfo->save3);
      if (error) return error;
      WriteIoPort (platformInfo->gpioBase + 0x04, 32, platformInfo->direction); // restore direction
      WriteIoPort (platformInfo->gpioBase + 0x0C, 32, platformInfo->gpioSave);  // restore output value
      return NULL;
      }

   if (platformInfo->type == _system_thunderbolt)
      {
      char *error;
      // restore LPC flash settings
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x6C, 4, platformInfo->save1);
      if (error) return error;
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x8C, 4, platformInfo->save2);
      if (error) return error;
      error = writePciConfig (0, platformInfo->lpcDevice, 0, 0x90, 4, platformInfo->save3);
      if (error) return error;
      WriteIoPort (platformInfo->gpioBase + platformInfo->gpioWp, 8, platformInfo->saveWp);   // restore wp gpio state
      WriteIoPort (platformInfo->gpioBase + platformInfo->gpioTbl, 8, platformInfo->saveTbl); // restore tbl gpio state
      return NULL;
      }

   return NULL;
   }



//---------------------------------------------------------------------------
static int isRepeatingData (volatile unsigned char *mmIoBase, long base)
   {
   int index, qwordCount = 10;
   volatile unsigned long *mmio;
   unsigned long previous;

   mmio = (void *) (mmIoBase + base);
   previous = mmio [0];
   
   for (index = 1; index < qwordCount; index++)
      if (mmio [index] != previous) break;
   return index == qwordCount;
   }


static unsigned int getFlashId (volatile unsigned char *mmIoBase)
   {
   unsigned int id, idByte [2];
   long base;
   int repeatingDataFound;
   base = 0x00FC0000;  // mmIoData for 256KB part
   mmIoBase [base + 0x5555] = 0xAA;
   mmIoBase [base + 0x2AAA] = 0x55;
   mmIoBase [base + 0x5555] = 0x90;
   idByte [0] = mmIoBase [base + 0];
   idByte [1] = mmIoBase [base + 1];
   repeatingDataFound = isRepeatingData (mmIoBase, base);
   mmIoBase [base + 0] = 0xF0; // reset
   id = idByte [1] + idByte [0] * 256;
   if (repeatingDataFound && id != 0 && id != 0xFFFF) return id;

   // try method for 16-bit part in 8-bit mode, such as AM29LV320
   mmIoBase [base + 0xAAAA] = 0xAA;
   mmIoBase [base + 0x5555] = 0x55;
   mmIoBase [base + 0xAAAA] = 0x90;
   idByte [0] = mmIoBase [base + 0];
   idByte [1] = mmIoBase [base + 3];
   repeatingDataFound = isRepeatingData (mmIoBase, base);
   mmIoBase [base + 0] = 0xF0; // reset
   id = idByte [1] + idByte [0] * 256;
   if (repeatingDataFound && id != 0 && id != 0xFFFF) return id;

   // try a different method (for SST)
  base = 0x00BC0000;

   idByte [0] = mmIoBase [base + 0];
   idByte [1] = mmIoBase [base + 1];
   mmIoBase [base + 0] = 0xF0; // reset

   id = idByte [1] + idByte [0] * 256;

   return id;
   }



char *formatFlashInfo (int *deviceSize, int *sectorSize, int *blockSize, int chipId)
   {
   char idMessage [60];
   char buffer[81];
   sprintf (idMessage, "flash chip ID %04X:", chipId);
    
  if (chipId == 0xBF60)
      {
      printw("%sSST49LF004A or SST49LF004B\n", idMessage);
      *deviceSize = 512 * 1024;
      *sectorSize = 4096;
      *blockSize = 65536;
      }
   if (chipId == 0xBF50)
      {
      printw("%sSST49LF040B\n", idMessage);  
      *deviceSize = 512 * 1024;
      *sectorSize = 4096;
      *blockSize = 65536;
      }
   else if (chipId == 0xBF5B)
      {
      printw("%sSST49LF080A\n", idMessage);   
      *deviceSize = 1024 * 1024;
      *sectorSize = 4096;
      *blockSize = 65536;
      }
   else if (chipId == 0xBF5A)
      {
      printw("%sSST49LF008A\n", idMessage);
      *deviceSize = 1024 * 1024;
      *sectorSize = 4096;
      *blockSize = 65536;
      }
   else if (chipId == 0x1FE1)
      {
      printw("%sAT49LW080\n", idMessage);
      *deviceSize = 1024 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }
   else if (chipId == 0xDAD3)
      {
      printw("%sW39V080FA\n", idMessage);	  
      *deviceSize = 1024 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }
   else if (chipId == 0xDA34)
      {
      printw("%sW39V040FA\n", idMessage);	  
      *deviceSize = 512 * 1024;
      *sectorSize = 8192;
      *blockSize = 65536;
      }
   else if (chipId == 0xDA50)
      {
      printw("%sW39V040CPZ\n", idMessage);	  
      *deviceSize = 512 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }
   else if (chipId == 0x202C)
      {
      printw("%sM50FW040\n", idMessage);	  
      *deviceSize = 512 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }
   else if (chipId == 0x202F)
      {
      printw("%sM50LPW080\n", idMessage);	  
      *deviceSize = 1024 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }
   else if (chipId == 0x202D)
      {
      printw("%sM50FW080\n", idMessage);	  
      *deviceSize = 1024 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }
   else if (chipId == 0x2080)
      {
      printw("%sM50FLW080A\n", idMessage);	  
      *deviceSize = 1024 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }
   else if (chipId == 0x2081)
      {
      printw("%sM50FLW080B\n", idMessage);
      *deviceSize = 1024 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }
   else if (chipId == 0x9D6E)
      {
      printw("%sPM49LF004\n", idMessage);
      *deviceSize = 512 * 1024;
      *sectorSize = 4096;
      *blockSize = 65536;
      }
   else if (chipId == 0x9D7F)
      {
      printw("%sPM49LF004T\n", idMessage);
      *deviceSize = 512 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }


   else if (chipId == 0x202E)
      {
      printw("%sM50FW016\n", idMessage);
      *deviceSize = 2 * 1024 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }

      else if (chipId == 0xBF4C)
      {
         printw("%s49LF160C\n", idMessage);	  
      *deviceSize = 2 * 1024 * 1024;
      *sectorSize = 4096;
      *blockSize = 65536;
      }

   else if (chipId == 0x01F6)
      {
      printw("%sAM29LV320DT\n", idMessage);
      *deviceSize = 4 * 1024 * 1024;
      *sectorSize = 65536;
      *blockSize = 65536;
      }
   else if (chipId == 0xC22249)
      {
      printw("%sMX29LV160AB\n", idMessage);
      *deviceSize = 2 * 1024 * 1024;
      *sectorSize = 8192;
      *blockSize = 65536;
      }
   else if (chipId == 0xC222C4)
      {
      printw("%sMX29LV160AT\n", idMessage);
      *deviceSize = 2 * 1024 * 1024;
      *sectorSize = 8192;
      *blockSize = 65536;
      }
   else if (chipId == 0x012249)
      {
      printw("%sS29AL016D,02\n", idMessage);
      *deviceSize = 2 * 1024 * 1024;
      *sectorSize = 8192;
      *blockSize = 65536;
      }
   else if (chipId == 0x0122C4)
      {
      printw("%sS29AL016D,01\n", idMessage);	 
      *deviceSize = 2 * 1024 * 1024;
      *sectorSize = 8192;
      *blockSize = 65536;
      }


   else if (chipId == 0x0122F9)
      {
      printw("%sS29AL032D,04\n", idMessage);
      *deviceSize = 4 * 1024 * 1024;
      *sectorSize = 8192;
      *blockSize = 65536;
      }
   else if (chipId == 0x0122F6)
      {
      printw("%sS29AL032D,03\n", idMessage);
      *deviceSize = 4 * 1024 * 1024;
      *sectorSize = 8192;
      *blockSize = 65536;
      }
    else if (chipId == 0x01227E)
      {
      printw("%sS29GL032M,01\n", idMessage);
      *deviceSize = 4 * 1024 * 1024;
      *sectorSize = 8192;
      *blockSize = 65536;
      }
   else 
   	{
   	printw("%snot supported\n", idMessage);
        return NULL;
	}
   return NULL;
   }


   
static char *getFlashInfo (unsigned char *mmIoBase, int *deviceSize, int *sectorSize, int *blockSize, char **chipName, int *chipId)
   {
   *chipId = getFlashId (mmIoBase);
   return formatFlashInfo (deviceSize, sectorSize, blockSize,  *chipId);
   }


char *identifyFlashCore (int *deviceSizeReturn, int *errorReturn)
   {
   int verbose = 0;
   volatile unsigned char *mmIoBase;
   unsigned int chipId;
   int deviceSize, sectorSize, blockSize, columns = 32, totalRetries = 0;
   unsigned int mapSize, mapBase;
   char *error, *chipName;
   char buffer [81];
   PLATFORM_INFO *platformInfo = malloc (sizeof platformInfo [0]);

   *errorReturn = 1;
   if (!platformInfo) return formatMessage ("malloc failed");
   error = identifySystem (platformInfo);
   if (error) return error;
   if (platformInfo->type == _system_unknown) return formatMessage ("unrecognized platform");
     error = prepareForFlash (platformInfo, verbose);
     if (error) return error;

   // map the upper 16 MB
   mapSize = 0x1000000;
   mapBase = 0x100000000 - mapSize;
   error = mapPhysicalMemory ((void **)&mmIoBase, mapBase, mapSize , 0);
   if (error) return error;
   error = getFlashInfo ((unsigned char*)mmIoBase, &deviceSize, &sectorSize, &blockSize, &chipName, &chipId);
   if (error) return error;
   // put things back the way they were
  // error = cleanupAfterFlash (platformInfo, mmIoBase);
  // if (error) return error;

   if (deviceSizeReturn) *deviceSizeReturn = deviceSize;
   *errorReturn = 0;

   unmapPhysical (mapBase, mapSize);
   
   return NULL;
   }


static void printStatus (char ascii, int verbose, int columns, int index)
   {
   if (verbose)
      {
      printw ("%c", ascii);
      if (index % columns == columns - 1) printw ("\n");
      }
   }

//---------------------------------------------------------------------------
//
// Two different erase sizes are support, sector and block. Block is the larger. If only
// one erase size is needed, make sector size and block size the same.
//

static char *flashCore (int imageSize, int verbose, int force, int dryRun, int noBootBlock, unsigned char *diskFileData, unsigned char *oldImage, volatile unsigned char *mmIoBase)
{
	volatile unsigned char *mmIoData, sectorDiff [512], sectorProgram [512], blockDiff [64];
	unsigned int chipId, index, block, sector, errorCount, sectorsPerBlock;
	int deviceSize, sectorSize, blockSize, sectorCount, blockCount, columns = 32, totalRetries = 0;
	int firstBootBlockSector, bootBlockSize = 65536;
	unsigned long byteOffset;
	char *error, *chipName;
	unsigned char *newImage;
	int nCycle = 1;
	scrollok(stdscr,TRUE);
	PLATFORM_INFO *platformInfo =malloc (sizeof platformInfo [0]);
        if (!platformInfo) return formatMessage ("malloc failed");
	error = identifySystem (platformInfo);
	if (error) return error;
	if (platformInfo->type == _system_unknown) return formatMessage ("unrecognized platform");
        error = prepareForFlash (platformInfo,verbose);

	if (error) return error; // remove for serenade
	if (error) printw ("warning: %s\n", error);
	error = getFlashInfo ((unsigned char*)mmIoBase, &deviceSize, &sectorSize, &blockSize, &chipName, &chipId);
	if (error) return error;
//	if (verbose) printw ("found %s\n", chipName);
	if( chipId == 0xBF4C)
		nCycle = 2;

	do 
	{

		sectorCount = deviceSize / sectorSize;
		blockCount = deviceSize / blockSize;
		sectorsPerBlock = blockSize / sectorSize;

		if (sectorCount > DIMENSION (sectorDiff) || blockCount > DIMENSION (blockDiff)) return formatMessage ("allocation");
		newImage = diskFileData;
		if (imageSize != deviceSize && populationCount (&imageSize, sizeof imageSize) != 1)
			{
			  printw  ("expected file length %Xh, actual %Xh\n", deviceSize, imageSize);	
			  return NULL;
			}
		else if (imageSize > deviceSize)
		{
			int excess = imageSize - deviceSize;

			int ffCount = strspn (newImage, "\xFF");
			if (ffCount < excess) return formatMessage ("file is too big for device");
			// remove enough leading FFs from the start of the file to make it fit the device
			imageSize = deviceSize;
			newImage += excess;

		}
		else if (imageSize < deviceSize)
		{
			int shortage = deviceSize - imageSize;

			// add enough leading FFs from the start of the file to make it fit the device
			newImage = malloc (deviceSize);
			if (!newImage) return formatMessage ("malloc failed");
			memset (newImage, 0xFF, shortage);
			memcpy (newImage + shortage, diskFileData, imageSize);
		}

		// mmIoBase is the entire 16 MB area, mmIoData is just the flash data portion
		mmIoData = mmIoBase + 16 * 1024 * 1024 - deviceSize;
		// read the current image
		memcpy ((void *)oldImage, (void *)mmIoData, deviceSize);

		// find the sectors that differ
		if (verbose) printw ("checking sectors for differences:\n");

		for (byteOffset = 0, sector = 0; byteOffset < deviceSize; byteOffset += sectorSize, sector++)
		{
			sectorDiff [sector] = memcmp (oldImage + byteOffset, newImage + byteOffset, sectorSize) != 0;

			// assume all differing sectors will be programmed
			sectorProgram [sector] = sectorDiff [sector];
		}

		// if force programming option is selected, select all sectors for programming
		if (force) memset ((void *)sectorProgram, 1, sectorCount);

		// if no bootblock programming option is selected, deselect bootblock programming
		firstBootBlockSector = sectorCount - bootBlockSize / sectorSize;
		if (noBootBlock) for (index = firstBootBlockSector; index < sectorCount; index++) sectorProgram [index] = 0;

		// derive differing block list from differing sector list
		for (index = 0; index < blockCount; index++) blockDiff [index] = 0;
		for (index = 0; index < sectorCount; index++) if (sectorProgram [index]) blockDiff [index / sectorsPerBlock] = 1;

		// display sector difference list
		for (sector = 0; sector < sectorCount; sector++)
			printStatus ("-d" [sectorDiff [sector]], verbose, columns, sector);
		if (verbose) printw ("\n");

		// unlock the whole device
		for (byteOffset = 0; byteOffset < deviceSize; byteOffset += blockSize)
		{
			error = softwareUnlockFlash (chipId, deviceSize, mmIoBase, byteOffset);
		        if (error)
				return error;
			if (chipId == 0x2080 || chipId == 0x2081)
			{
				int sector;
				for (sector = 1; sector < 16; sector++)
				{
				error = softwareUnlockFlash (chipId, deviceSize, mmIoBase, byteOffset + sector * 4096);
				if (error) return error;
				}
			}
		}

		// make sure erase/programming will succeed by programming the first byte of each differing sector
		// program with the same value so that the system will still boot if something goes wrong
		if (verbose) printw ("testing write enable (e=enabled, p=protected):\n");
		errorCount = 0;
		for (byteOffset = 0; byteOffset < deviceSize; byteOffset += blockSize)
		{
			int retries;

			// if all sectors in the block match, write access is not needed
			if (!blockDiff [byteOffset / blockSize])
			{
				printStatus ('-', verbose, columns, byteOffset / blockSize);
				continue;
			}

			error = programFlash (chipId, mmIoData, oldImage, byteOffset, 1, 1, &retries);
			if (!error)
			{
				printStatus ('e', verbose, columns, byteOffset / blockSize);
				continue;
			}

			printStatus ('p', verbose, columns, byteOffset / blockSize);
			errorCount++;
		}
		if (verbose) printw ("\n");
		if (errorCount) return formatMessage ("write protected sectors found, enable write and try again\n");

		// erase the sectors that are selected for programming
		if (verbose) printw ("erasing sectors (s=sector erase, b=block erase):\n");
		for (block = 0, sector = 0; block < blockCount; block++, sector += sectorsPerBlock)
		{
			// if each sector in a block differs, use block erase to save time
			if (strlen ((char *)(sectorProgram + sector)) >= sectorsPerBlock && !isAtmelOrSt (chipId))
			{
				int offset = block * blockSize;
				if (verbose)
					for (index = 0; index < sectorsPerBlock; index++)
					{
						if (sectorsPerBlock == 1) printw ("b");
						else if (index == 0) printw ("[");
						else if (index == sectorsPerBlock - 1) printw ("]");
						else printw ("b");
						if ((sector + index) % columns == columns - 1) printw ("\n");
					}

					if (allFF ((void *)(mmIoData + offset), blockSize)) continue;
					error = blockEraseFlash (chipId, mmIoData, offset, dryRun);
					if (error) return error;
					if (chipId == 0x01F6) // AM29LV320DT
						if (offset == deviceSize - 0x10000) // if this is where the sectors get smaller, erase each small one
						{
							int index;
							for (index = 1; index < 8; index++)
							{
								int smallBlockSize = 0x2000;
								int totalOffset = offset + index * smallBlockSize;
								error = blockEraseFlash (chipId, mmIoData, totalOffset, dryRun);
								if (error) return error;
								if (!allFF ((void *)(mmIoData + totalOffset), smallBlockSize)) 
								{
								printw("block erase %X failed", totalOffset);
								return NULL;
								}

							}

						}
						if (!allFF ((void *) (mmIoData + offset), blockSize)) 
							{
							printw ("block erase failed offset %X blockSize %X\n",offset,blockSize);
							return NULL;
							}
			}
			else // erase individual sectors that have been selected for programming
				for (index = 0; index < sectorsPerBlock; index++)
				{
					printStatus ("-s" [sectorProgram [sector + index]], verbose, columns, sector + index);
					if (sectorProgram [sector + index])
					{
						int offset = (sector + index) * sectorSize;
						if (!allFF ((void *)(mmIoData + offset), sectorSize))
						{
							if (dryRun) continue;
							error = sectorEraseFlash (chipId, mmIoData, offset);
							if (error) return error;
							if (!allFF ((void *)(mmIoData + offset), sectorSize)) 
							{
							printw("sector erase failed\n");
							return NULL;
							}
						}

					}
				}
		}
		if (verbose) printw ("\n");

		// reprogram the erased sectors
		if (verbose) printw ("programming sectors:\n");
		for (sector = 0; sector < sectorCount; sector++)
		{
			int retries;
			printStatus ("-p" [sectorProgram [sector]], verbose, columns, sector);
			if ((sectorProgram [sector] == 0 || dryRun) && !force) continue;
			error = programFlash (chipId, mmIoData, newImage, sector * sectorSize, sectorSize, force, &retries);
			totalRetries += retries;
			if (error) return error;
		}
		if (verbose) printw ("\n");

		// relock the whole device
		for (byteOffset = 0; byteOffset < deviceSize; byteOffset += blockSize)
		{
			error = softwareLockFlash (chipId, deviceSize, mmIoBase, byteOffset);
			if (error) return error;
		}

		// verify the programmed sectors
		errorCount = 0;
		resetFlash (mmIoData);
		if (!dryRun)
		{
			int breakFlag = 0;
			for (sector = 0; sector < sectorCount; sector++)
			{
				if (breakFlag) break;
				// skip verify if sector difference could be expected
				if (!sectorProgram [sector]) continue;
				for (index = 0; index < sectorSize; index++)
				{
					int byteOffset = sector * sectorSize + index;
					if (breakFlag) break;
					if (mmIoData [byteOffset] != newImage [byteOffset])
					{
						errorCount++;
						if (verbose)
						{
							printw ("offset %05X: expected %02X, actual %02X\n", byteOffset, newImage [byteOffset], mmIoData [byteOffset]);
							if (kbhit ()) breakFlag++;
						}
					}
				}
			}
		}

		if (imageSize < deviceSize) free (newImage);

		if (errorCount) printw ("*** %u bytes do not match, try again! ***\n", errorCount);
		else if (totalRetries) printw ("NOTE: %u retries were required\n", totalRetries);
	}while ( --nCycle >0);
	// put things back the way they were
	scrollok(stdscr,FALSE);
	return cleanupAfterFlash (platformInfo, mmIoBase);
}




char *identifyFlash (void)
   {
   int error, deviceSize;
   return identifyFlashCore (&deviceSize, &error);
   }

//---------------------------------------------------------------------------
// flash eb offset xx xx xx ...

char *flashByteList (int argc, char *argv [])
   {
   unsigned char *newImage, *oldImage;
   int deviceSize, index;
   int verbose = 1, force = 0, dryRun = 0, noBootBlock = 0;
   char *error;
   volatile unsigned char *mmIoBase, *mmIoData;
   u64 data [MAX_ARGS], startAddress;
   u64 mapSize, mapBase;
   int    elements;
   
   error = hextoint64 (argv [3], &startAddress, 6);                 if (error) return error;
   error = buildDataList (argc - 4, &argv [4], 8, data, &elements); if (error) return error;
     
   {
   int error;
   char *retval = identifyFlashCore (&deviceSize, &error);
   if (error) return retval;
   }
  
   if (startAddress + elements > deviceSize) return formatMessage ("data extends past end of flash chip");

   newImage = malloc (deviceSize);
   oldImage = malloc (deviceSize);
   if (!newImage || !oldImage) 
      {
      freeResources (newImage, oldImage, NULL);
      return formatMessage ("malloc failed");
      }

   // map the upper 16 MB
   mapSize = 0x1000000;
   mapBase = 0x100000000 - mapSize;
   error = mapPhysicalMemory ((void **)&mmIoBase, mapBase, mapSize ,1);
   if (error) return error;

   // mmIoBase is the entire 16 MB area, mmIoData is just the flash data portion
   mmIoData = mmIoBase + 16 * 1024 * 1024 - deviceSize;

   // read the current flash data
   memcpy ((void *)oldImage, (void *)mmIoData, deviceSize);
   memcpy ((void *)newImage, (void *)oldImage, deviceSize);

   // put data modifications into buffer to flash
   for (index = 0; index < elements; index++)
      newImage [startAddress + index] = data [index];

   error = flashCore (deviceSize, verbose, force, dryRun, noBootBlock, newImage, oldImage, mmIoBase);

   freeResources (newImage, oldImage, NULL);
   unmapPhysical (mapBase, mapSize );
   return error;
   }

//---------------------------------------------------------------------------
// flash fb [range] x x x...

char *flashFill (int argc, char *argv [])
   {
   unsigned char *newImage, *oldImage;
   int deviceSize, index;
   int verbose = 1, force = 0, dryRun = 0, noBootBlock = 0;
   char *error;
   volatile unsigned char *mmIoBase, *mmIoData;
   u64 data [MAX_ARGS], startAddress, endAddress, bytesRemaining;
   u64 mapSize, mapBase;
   int    element, elements;

   if (argc < 6) return formatMessage ("invalid argument count");
   error = hextoint64 (argv [3], &startAddress, maxAddressDigits);  if (error) return error;
   error = hextoint64 (argv [4], &endAddress, maxAddressDigits);    if (error) return error;
   
   // debug & symdeb treat start address less than end address as an error. Here, treat end address as byte count instead
   if (endAddress < startAddress) endAddress = endAddress + startAddress - 1;
   bytesRemaining = endAddress - startAddress + 1;
   error = buildDataList (argc - 5, &argv [5], 8, data, &elements);
   if (error) return error;
   
   
   {
   int error;
   char *retval = identifyFlashCore (&deviceSize, &error);
   if (error) return retval;
   }
   
   if (endAddress >= deviceSize) return formatMessage ("data extends past end of flash chip");

   newImage = malloc (deviceSize);
   oldImage = malloc (deviceSize);
   if (!newImage || !oldImage) 
      {
      freeResources (newImage, oldImage, NULL);
      return formatMessage ("malloc failed");
      }

   // map the upper 16 MB
   mapSize = 0x1000000;
   mapBase = 0x100000000 - mapSize;
   error = mapPhysicalMemory ((void **)&mmIoBase, mapBase, mapSize ,1);
   if (error) return error;

   // mmIoBase is the entire 16 MB area, mmIoData is just the flash data portion
   mmIoData = mmIoBase + 16 * 1024 * 1024 - deviceSize;

   // read the current flash data
   memcpy ((void *)oldImage, (void *)mmIoData, deviceSize);
   memcpy ((void *)newImage, (void *)oldImage, deviceSize);

   // put data modifications into buffer to flash
   element = 0;
   for (index = 0; index < bytesRemaining; index++)
      {
      newImage [startAddress + index] = data [element++];
      if (element == elements) element = 0;
      }

   error = flashCore (deviceSize, verbose, force, dryRun, noBootBlock, newImage, oldImage, mmIoBase);

   freeResources (newImage, oldImage, NULL);

   unmapPhysical (mapBase, mapSize );
   return error;
   }

//---------------------------------------------------------------------------

char *mtr (int argc, char *argv [])
{
    unsigned char *newImage = NULL, *oldImage = NULL;
    int deviceSize, device;
    int verbose = 1, force = 0, dryRun = 0, noBootBlock = 0;
    char *error = NULL;
    volatile unsigned char *mmIoBase = NULL, *mmIoData = NULL;
    u64 startAddress, mtrSize;
    u64 mapSize, mapBase;
    struct stat stmp;
    char *filename = NULL;
    int length, count;
    FILE *stream = NULL;

    if (argc < 6) return formatMessage ("flash mtr <filename> address       program flash\n");
    error = hextoint64 (argv [4], &startAddress, maxAddressDigits);  if (error) return error;
    error = hextoint64 (argv [5], &mtrSize, maxAddressDigits);  if (error) return error;
    filename = argv [3];
    printw("mtr file %s, offset = 0x%06lX, mtr length = %ld\n", filename, startAddress, mtrSize);

    /*{
        int error;
        char *retval = identifyFlashCore (&deviceSize, &error);
        if (error) return retval;
    }*/

   error = findMCP55 (&device);
   if (error) return error;
   if (device != -1) 
   {
	   deviceSize = 0x80000;
   }
   else
   {
	   deviceSize = 0x100000;
   }

   if (startAddress + mtrSize >= deviceSize) return formatMessage ("data extends past end of flash chip");

    stream = fopen (filename, "rb");
    if (!stream)
    {
        printw ("file %s not found \n", filename);
	return NULL;
    }
    if(stat(filename,&stmp)<0)
    {
         fclose(stream);
         printw("Couldn't get the file length \n");
         return NULL;
    }
   
    length = lseek((int)stream,0L,SEEK_END);
    if (length != mtrSize)
    {
        fclose(stream);
        printw ("file(%s) size unequal %d bytes", filename, mtrSize);
	  return NULL;
    }

    newImage = malloc (deviceSize);
    oldImage = malloc (0x200000); // handle up to 2MB device
    if (!newImage || !oldImage)
    {
        freeResources (newImage, oldImage, stream);
        return formatMessage ("malloc failed");
    }

    count = fread (newImage + startAddress, 1, length, stream);
    fclose (stream);
    if ( count != length )
    {
        freeResources (newImage, oldImage, NULL);
	  printw ("read from %s only %u of %u bytes", filename, count, length);
	  return NULL;
    }

    // map the upper 16 MB
    mapSize = 0x1000000;
    mapBase = 0x100000000 - mapSize;
    error = mapPhysicalMemory ((void **)&mmIoBase, mapBase, mapSize ,1);
    if (error)
    {
        freeResources (newImage, oldImage, NULL);
        return error;
    }

    // mmIoBase is the entire 16 MB area, mmIoData is just the flash data portion
    mmIoData = mmIoBase + 16 * 1024 * 1024 - deviceSize;

    // read the current flash data
    memcpy ((void *)oldImage, (void *)mmIoData, deviceSize);
    memcpy ((void *)newImage, (void *)oldImage, startAddress);
    memcpy ((void *)(newImage + startAddress + mtrSize), (void *)(oldImage + startAddress + mtrSize), deviceSize - startAddress - mtrSize);

    error = flashCore (deviceSize, verbose, force, dryRun, noBootBlock, newImage, oldImage, mmIoBase);

    freeResources (newImage, oldImage, NULL);

    unmapPhysical (mapBase, mapSize );
    return error;
}
char *_flash (int argc, char *argv [])
   {
   void  *mmIoBase;
   char *error, *filename = argv [2];
   int index, length, count, deviceSize;
   unsigned int mapSize, mapBase;
   struct stat stmp;
   unsigned char *newImage = NULL, *oldImage = NULL;
   FILE *stream = NULL;
   int verbose = 1, force = 0, dryRun = 0, noBootBlock = 0;
   char buffer [81];
   if (argc == 2)
      {
      printw ("flash eb address x x x...    program flash bios with one or more byte values\n");
      printw ("flash fb range x x x...      program flash bios with fill pattern\n");
      printw ("flash <filename> [q] [f] [n] program flash bios f=force n=no bootblock\n");
      printw ("flash mtr <filename> address length\n");
      printw ("\nidentifying flash device... \n");
      return identifyFlash ();;
      }
   // add command to flash a list of byte values: flash eb offset xx xx xx ...
   if (argc >= 5)
      {
      if (strcmp (argv [2], "eb") == 0)
         return flashByteList (argc, argv);
      else if (strcmp (argv [2], "fb") == 0)
         return flashFill (argc, argv);
      else if (strcmp (argv [2], "mtr") == 0)
         return mtr (argc, argv);
      }
   for (index = 3; index < argc; index++)
      if (strcmp (argv [index], "q") == 0) verbose = 0;
      else if (strcmp (argv [index], "d") == 0) dryRun++;
      else if (strcmp (argv [index], "f") == 0) force++;
      else if (strcmp (argv [index], "n") == 0) noBootBlock++;
      else 
      	{
      		sprintf (buffer, "unexpected argument: %s", argv [index]);
	  	return formatMessage (buffer);
      	}

   // *QUK Start Worund the latest version couldn't flash Hornet 
  /*   
   {
   bint error;
   char *retval = identifyFlashCore (&deviceSize, &error);
   if (error) return retval;
   }
   */
   deviceSize = 0x200000; // handle up to 2MB device
   // *QUK  END
   stream = fopen (filename, "rb");
   if (!stream) 
   	{
		sprintf (buffer, "file %s not found", filename);
		return formatMessage (buffer);
   	}
   
   if(stat(filename,&stmp)<0)
   {
   fclose(stream);
   return NULL;
   }
   length = stmp.st_size;  
  
   newImage = malloc (length);
   oldImage = malloc (deviceSize);

   if (!newImage || !oldImage) 
      {
      freeResources (newImage, oldImage, stream);
      return formatMessage ("malloc failed");
      }
   count = fread (newImage, 1, length, stream);
   fclose (stream);
   if(count !=length)
     {
     freeResources (newImage, oldImage, stream);
     printw ("read from %s only %u of %u bytes", filename, count, length);
     return NULL;
     }

   // map the upper 16 MB
   mapSize = 0x1000000;
   mapBase = 0x100000000 - mapSize;
   error = mapPhysicalMemory (&mmIoBase, mapBase, mapSize ,1);
   if (error) return error;
 
    
   error = flashCore (length, verbose, force, dryRun, noBootBlock, newImage, oldImage, mmIoBase);

   unmapPhysical (mapBase, mapSize );
  
 return error;

 }
