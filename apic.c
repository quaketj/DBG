#include "project.h"

#pragma pack (push,1)
typedef struct
{
   u32 	signature;
   u32 	physicalAddress;
   u8  	length, specRev, checksum;
   u8  	mpFeatureByte [5];
}MP_FLOATING_POINTER_STRUCTURE;

typedef struct
{
   u8  	signature [4];
   u16 	baseTableLength;
   u8  	specRecv, checksum;
   u8  	oemId [8];
   u8  	productId [12];
   u32 	oemTablePointer;
   u16 	oemTableSize, entryCount;
   u32 	localApicAddesss;
   u16 	extendedTableLength;
   u8  	extendedtableChecksum, reserved;
}MP_CONFIG_TABLE_HEADER;


typedef struct
{
   u8 entryType, localApicID, localApicVersion, CpuFlags;
   u32 CpuSignature, featureFlags, reserved [2];
}
MP_PROCESSOR;

typedef struct
{
   u8 entryType, busID;
   u8 busTypeString [6];
}
MP_BUS;

typedef struct
{
   u8 entryType, id, version, flags;
   u32 physicalAddress;
}
MP_IO_APIC;

typedef struct
{
   u8 entryType, interruptType, flags, reserved;
   u8 sourceBusId, sourceBusIrq, destinationIoApicId, destinationIoApicIntin;
}
MP_IO_INTERRUPT;

typedef struct
{
   u8 entryType, interruptType, flags, reserved;
   u8 sourceBusId, sourceBusIrq, destinationLocalApicId, destinationLocalApicIntin;
}
MP_LOCAL_INTERRUPT;

typedef struct
{
   u8 entryType, entryLength, busId, addressType;
   u64 addressBase, addressLength;
}
MP_SYSTEM_ADDRESS_SPACE;   

typedef struct
{
   u8 entryType, entryLength, busId, busInfo;
   u8 parentBus, reserved [3];
}
MP_BUS_HIERARCHY;

typedef struct
{
   u8 entryType, entryLength, busId, addressMod;
   u32 predifinedRangeList;
}
MP_COMPATIBILITY_BUS;


int ReadMem (u64 phy_start_addr,u8 *buf)
{
	char *error = NULL;
	 __u8  *mmIoBase8;

   	 //__u16 *mmIoBase16;
   	 //__u32 *mmIoBase32;
   
	error = mapPhysicalMemory ((void **)&mmIoBase8,phy_start_addr,0x00,0x00);
	if(error) return -1;
	*buf = *mmIoBase8;
	unmapPhysical (phy_start_addr,0x00);
	return 0;
}

#define	APIC_IOREGSEL	(0x00)
#define	APIC_IOWIN		(0x10)
u32 readApic (u64 phy_addr,u8 offset)
{
	char *error = NULL;
	 __u8  *mmIoBase8;
	u32 data;

	error = mapPhysicalMemory ((void **)&mmIoBase8,phy_addr,0x00,0x00);
	if(error) return 0xffffffff;
	*((u32 *)mmIoBase8) = (u32)offset;
	unmapPhysical (phy_addr,0x00);
	
	error = mapPhysicalMemory ((void **)&mmIoBase8,phy_addr + APIC_IOWIN,0x00,0x00);
	if(error) return 0xffffffff;
	data = *((u32 *)mmIoBase8) ;
	unmapPhysical ((phy_addr + APIC_IOWIN ),0x00);

	return data;
}

u8 checksum (void *address, u32 size)
{
   u8	*position = address;
   u8	total = 0;
   //UINT8    temp;
   while (size--) 
   {
	   total = total +  *position;
	   position++;
   }
   return total;
}


static MP_FLOATING_POINTER_STRUCTURE *findMpFloatingPointerStructure (void *virtualAddress, int size)
{

	MP_FLOATING_POINTER_STRUCTURE *position = virtualAddress;

   	while (size){
    	if (position->signature == 0x5F504D5F) // look for ascii "_MP_" tag
        	if (checksum (position, sizeof (MP_FLOATING_POINTER_STRUCTURE)) == 0) return position;
      	position++;
      	size -= sizeof (MP_FLOATING_POINTER_STRUCTURE);
	}
   	return NULL;
}

static MP_FLOATING_POINTER_STRUCTURE *findMpTable (u8 *lowMemVirtual) 
{
   MP_FLOATING_POINTER_STRUCTURE *mpfps;

   mpfps = findMpFloatingPointerStructure (lowMemVirtual, 0x20000);
   return (void *) mpfps;
}



char * getIoapicAddress (u64 *phyAddress)
{
	MP_FLOATING_POINTER_STRUCTURE *mpfps;
   	u8  *lowMemVirtual;
   	MP_CONFIG_TABLE_HEADER *mpHeader;
   	u32 size, entries;
   	u8  *position;
   	u32 bytesRemaining;
   	u32 physicalAddress = 0;
   	static int interruptCount;
   	u32 nTemp;
   	u32 nLength;
   	char *error=NULL;
   	unsigned short busTypeString[8];
    u64 length;
   	char *featureText1 [] = {"Full Table mode", "chapter 5 default mode"};
   	char *featureText2 [] = {"Virtual Wire Mode","PIC mode"};
      
	char oemId [9], productId [13];
	char w_oemId [10], w_productId [14],w_signature[5];
   	char pciBusNumbers [256];
	u8 sum;


  	// Scan _MP_ table in 0xe0000--0xfffff
   	nLength = 0x20000;
   	// look for the MP table to get memory size and hole info
   	
  	lowMemVirtual = malloc (nLength);
	if(!lowMemVirtual) return formatMessage ("malloc memory fail");
  	memset(lowMemVirtual,'\0',nLength);

  	for(nTemp =0; nTemp <nLength; nTemp++)
		ReadMem (0xe0000 + nTemp,(u8*) lowMemVirtual + nTemp);

  	*phyAddress = 0;	
   	mpfps = findMpTable (lowMemVirtual);
   	if (!mpfps) return formatMessage ("mp table not present");
   	physicalAddress = mpfps->physicalAddress;
  
#if 0 
	scrollok(stdscr,TRUE);
	printw("   MP Floating Pointer Structure found at %x\n", 0xE0000 + (u8 *)mpfps - lowMemVirtual);
   	printw("   Signature              %8s\n", "_MP_");
   	printw("   Table Physical Address %8xh\n", physicalAddress);
   	printw("   Length                 %8xh\n", mpfps->length * 16);
   	printw("   MPS Spec Revision      %7x.%1x\n", 1, mpfps->specRev);
   	printw("   Checksum               %8x\n", mpfps->checksum);
   	printw("   MPFeature1             %8xh (%s)\n", mpfps->mpFeatureByte [0],featureText1 [mpfps->mpFeatureByte [0]]); 
   	printw("   MPFeature2             %8xh (%s)\n", mpfps->mpFeatureByte [1],featureText2 [mpfps->mpFeatureByte [1]]);
	scrollok(stdscr,FALSE);
#endif 
   	if (physicalAddress !=0){
    	nLength = sizeof mpHeader [0];
		mpHeader = (MP_CONFIG_TABLE_HEADER *)malloc (nLength);
		if(!mpHeader)  return formatMessage ("malloc memory fail");
      	memset(mpHeader,'\0',nLength);
      	for(nTemp =0; nTemp <nLength; nTemp++){
			ReadMem (physicalAddress + nTemp,(u8 *)mpHeader + nTemp);
		}
      	length = mpHeader->baseTableLength + mpHeader->extendedTableLength;
		if (mpHeader) { mpHeader = NULL; free(mpHeader);}
      	mpHeader = malloc (length);
		if(!mpHeader)  return formatMessage ("malloc memory fail");
      	memset(mpHeader,'\0',length);
      	for(nTemp =0; nTemp < length; nTemp++){
			ReadMem (physicalAddress + nTemp, (u8*) mpHeader + nTemp);
		}

   		memcpy (w_oemId, mpHeader->oemId,8);
     	memcpy (w_productId, mpHeader->productId,12);
		w_oemId[8] ='\0';
		w_productId[12] ='\0';
      	MultiCharToUnicode (mpHeader->signature, w_signature); 
		
		scrollok(stdscr,TRUE);
#if 0
      	printw("\nMP Configuration Table Header\n");
      	printw("   OEM ID                        %13s\n", w_oemId);
      	printw("   Product ID                    %13s\n", w_productId);
      	printw("   Number of base table entries  %12xh (%x)\n", mpHeader->entryCount, mpHeader->entryCount);
      	printw("   Signature                     %13.4s\n", w_signature);
      	printw("   Base Table Length             %12xh\n", mpHeader->baseTableLength);
      	printw("   Spec Revision                 %11x.%1x\n", 1, mpHeader->specRecv);
      	printw("   Checksum                      %12xh\n", mpHeader->checksum);
      	printw("   OEM Table Physical Address    %12xh\n", mpHeader->oemTablePointer);
      	printw("   OEM Table Size                %12xh\n", mpHeader->oemTableSize);
      	printw("   Local APIC Physical Address   %12xh\n", mpHeader->localApicAddesss);
      	printw("   Extended Table Length         %12xh\n", mpHeader->extendedTableLength);
      	printw("   Extended Table Checksum       %12xh\n", mpHeader->extendedtableChecksum);
      	printw("\n");
#endif	
		position = (void *) (&mpHeader [1]); // skip past header to first record
      	bytesRemaining = mpHeader->baseTableLength - sizeof (mpHeader [0]);
      	entries = 0;
#if 1
		while (bytesRemaining > 0)
		{
        	u32 entryType = *position;
		 	memset(busTypeString,0,sizeof( busTypeString));
         	entries++;

         	if (entryType == 0) // processor entry
			{
            	static u32  itemCount;
            	MP_PROCESSOR *item = (void *) position;
            	size = sizeof (item [0]);
            	position += size;
            	bytesRemaining -= size;
#if 0
            	if (itemCount++ == 0)
               		printw ("\n\nPROCESSOR: ");
            	else
               		printw ("           ");
            	printw ("family,model,stepping %x,%x,%x", pullbits (item->CpuSignature, 8, 11), pullbits (item->CpuSignature, 4, 7), pullbits (item->CpuSignature, 0, 3));
            	printw (" local APIC ID=%x version=%x", item->localApicID, item->localApicVersion);
            	if (item->CpuFlags & 1) printw ("  enabled");
            	if (item->CpuFlags & 2) printw ("  boot processor");
            	printw ("\n");
#endif
            	continue;
            }

         	if (entryType == 1) // bus entry
            {
            	static int itemCount;
            	MP_BUS *item = (void *) position;
            	size = sizeof (item [0]);
            	position += size;
            	bytesRemaining -= size;
#if 0
            	if (itemCount++ == 0)
               		printw ("\n\nBUS: ");
            	else
               		printw ("     ");
            	printw ("%3x-", item->busID);
				item->busTypeString[5] = '\0';
				printMultiChar (item->busTypeString);
				if(memcmp(item->busTypeString, "PCI  ", 5)==0) pciBusNumbers [item->busID]++;
#endif
            	continue;
            }

         	if (entryType == 2) // I/O apic entry
            {
            	static int itemCount;
            	MP_IO_APIC *item = (void *) position;
            	size = sizeof (item [0]);
            	position += size;
            	bytesRemaining -= size;
            	if (itemCount++ == 0)
               		printw ("\nI/O APIC: ");
            	else
               		printw ("          ");
            	printw ("ID=%x version=%xh address=%x", item->id, item->version, item->physicalAddress);
            	if ((item->flags & 1) != 0) printw (" enabled");
            	printw ("\n");
				*phyAddress = (u64)item->physicalAddress;
				//break;
            	continue;
            }
         	if (entryType == 3) // I/O interrupt entry 
            {
            	char  *polarityText [] = {"conforms", "  high  ", "reserved", "  low   "};
            	char  *triggerText [] = {"conforms", "  edge  ", "reserved", " level  "};
            	char  *typeText [] = {" INT  ", " NMI  ", " SMI  ", "EXTInt"};
            	MP_IO_INTERRUPT *item = (void *) position;
            	size = sizeof (item [0]);
            	position += size;
            	bytesRemaining -= size;
#if 0
            	if (interruptCount++ == 0)
               	{
               		printw ("\n\nInterrupts:\n");
               		printw ("    Bus Dev   IRQ  APIC  INTIN  Polarity  Trigger   Type    I/O\n");
               	}
           		printw ("%6x", item->sourceBusId);
            	// device
            	if (pciBusNumbers [item->sourceBusId]) printw (" %2x ", pullbits (item->sourceBusIrq, 2, 6));
            	else printw ("    ");
            	if (pciBusNumbers [item->sourceBusId] !=0) printw ("   INT%c", 'A' + pullbits (item->sourceBusIrq, 0, 1));
            	else printw ("%6xh", item->sourceBusIrq);
            	if (item->destinationIoApicId == 255) printw ("%6s ", "all");
            	else printw ("%6x ", item->destinationIoApicId);
            	printw ("%4x", item->destinationIoApicIntin);
            	printw ("%12s", polarityText [pullbits (item->flags, 0, 1)]);
            	printw ("%10s", triggerText [pullbits (item->flags, 2, 3)]);
            	if (item->interruptType < DIMENSION (typeText)) printw ("%8s", typeText [item->interruptType]);
            	else printw ("%8s", L"  ?   ");
            	if (entryType == 3) printw ("  I/O");
            	else printw (" Local");
            	printw ("\n");
#endif
            	continue;
            }
			if (entryType == 4) //local interrupt entry
            {
            	char  *polarityText [] = {"conforms", "  high  ", "reserved", "  low   "};
            	char  *triggerText [] = {"conforms", "  edge  ", "reserved", " level  "};
            	char  *typeText [] = {" INT  ", " NMI  ", " SMI  ", "EXTInt"};
            	MP_IO_INTERRUPT *item = (void *) position;
            	size = sizeof (item [0]);
            	position += size;
            	bytesRemaining -= size;
#if 0
            	if (interruptCount++ == 0)
               	{
               		printw ("\n\nInterrupts:\n");
               		printw ("    Bus Dev   IRQ  APIC  INTIN  Polarity  Trigger   Type    Local\n");
               	}
           		printw ("%6x", item->sourceBusId);
            	// device
            	if (pciBusNumbers [item->sourceBusId]) printw (" %2x ", pullbits (item->sourceBusIrq, 2, 6));
            	else printw ("    ");
            	if (pciBusNumbers [item->sourceBusId] !=0) printw ("   INT%c", 'A' + pullbits (item->sourceBusIrq, 0, 1));
            	else printw ("%6xh", item->sourceBusIrq);
            	if (item->destinationIoApicId == 255) printw ("%6s ", "all");
            	else printw ("%6x ", item->destinationIoApicId);
            	printw ("%4x", item->destinationIoApicIntin);
            	printw ("%12s", polarityText [pullbits (item->flags, 0, 1)]);
            	printw ("%10s", triggerText [pullbits (item->flags, 2, 3)]);
            	if (item->interruptType < DIMENSION (typeText)) printw ("%8s", typeText [item->interruptType]);
            	else printw ("%8s", L"  ?   ");
            	if (entryType == 3) printw ("  I/O");
            	else printw (" Local");
            	printw ("\n");
#endif
            	continue;
            }

			printw ("\nunexpected MP table entry, type=%02x\n", entryType);
		}

      	if (bytesRemaining) printw ("\nunexpected MP table bytesRemaining=%d!\n", bytesRemaining);
      	if (entries != mpHeader->entryCount) printw ("\nexpected %u entries, found %x!\n", mpHeader->entryCount, entries);

      	bytesRemaining = mpHeader->extendedTableLength;
      	printw ("\nMP table extension found at %xh, extended table size %xh", physicalAddress, bytesRemaining);
      	sum = checksum (position, mpHeader->extendedTableLength) + mpHeader->extendedtableChecksum;
      	if (sum != 0) printw (" non-zero checksum %02x", sum);
      	printw ("\n");

#if 0
      	while (bytesRemaining > 0)
         {
         UINT32 entryType = position [0];
         UINT32 entrySize = position [1];

         bytesRemaining -= entrySize;

         if (entryType == 128) // system address space
            {
            MP_SYSTEM_ADDRESS_SPACE *item = (void *) position;
            static int itemCount;
            static CHAR16 *addressTypeText [] = {L"I/O", L"Memory", L"Prefetch"};

            if (itemCount++ == 0)
               printw (L"\n\nSYSTEM ADDRESS: ");
            else
               printw (L"                ");

            position += item->entryLength;
            printw (L"bus %3x base address %8x length %8x type %s\n", item->busId, item->addressBase, item->addressLength, addressTypeText [item->addressType]);
            if (item->addressType != 1) continue;       // skip all but memory type address records
            if (item->addressBase < 0x100000) continue; // below 1m is not part of the PCI hole
            smallestBusAddress = min (item->addressBase, smallestBusAddress);
            continue;
            }

         if (entryType == 129) // bus hierarchy
            {
            MP_BUS_HIERARCHY *item = (void *) position;
            static int itemCount;

            if (itemCount++ == 0)
               printw (L"\n\nBUS HIERARCHY: ");
            else
               printw (L"                ");

            position += item->entryLength;
            printw (L"bus%3x, parent bus %x", item->busId, item->parentBus);
            if (item->busInfo & 1)
               printw (L"  subtractive decode");
            printw (L"\n");

            continue;
            }

         if (entryType == 130) // Compatibility Bus Address Space Modifier Entry
            {
            MP_COMPATIBILITY_BUS *item = (void *) position;
            static int itemCount;

            if (itemCount++ == 0)
               printw (L"\n\nCOMPATIBILITY_BUS: ");
            else
               printw (L"                   ");

            position += item->entryLength;
            printw (L"bus %3x predefined range list=%x", item->busId, item->predifinedRangeList);
            printw (L"\n");

            continue;
            }

         else // type we are not looking for
            {
            position += entrySize;
            printw (L"\n\nUnexpected extended table entry type %xh\n", entryType);
            continue;
            }
         }

      if (bytesRemaining) printw (L"\nUnexpected extended MP table bytesRemaining=%d!\n", bytesRemaining);
#endif 
		scrollok(stdscr,FALSE);
		}
#endif

#if 0
//########################################################################################################
      	while (bytesRemaining > 0){
        	u32 entryType = *position;
		 	memset(busTypeString,0x00,sizeof( busTypeString));
         	entries++;
      		if (entryType == 0) // processor entry
            {
            	static u32  itemCount;
            	MP_PROCESSOR *item = (void *) position;
            	size = sizeof (item [0]);
				position += size;
            	bytesRemaining -= size;
            	continue;
            }
         	if (entryType == 1) // bus entry
            {
            	static int itemCount;
            	MP_BUS *item = (void *) position;
            	size = sizeof (item [0]);
            	position += size;
            	bytesRemaining -= size;
            	continue;
            }
         	if (entryType == 2) // I/O apic entry
            {
            	static int itemCount;
            	MP_IO_APIC *item = (void *) position;
            	size = sizeof (item [0]);
            	position += size;
            	bytesRemaining -= size;

				scrollok(stdscr,TRUE);
            	printw("\n\nI/O APIC: ");
            	printw("ID=%x version=%xh address=%x", item->id, item->version, item->physicalAddress);
				*phyAddress =(u64) item->physicalAddress;
            	if ((item->flags & 1) == 0) printw(" enabled");
            	printw("\n");
				scrollok(stdscr,TRUE);
            	break;
            }
         	if (entryType == 3 || entryType == 4) // I/O interrupt entry or local interrupt entry
            {
            	MP_IO_INTERRUPT *item = (void *) position;
            	size = sizeof (item [0]);
            	position += size;
            	bytesRemaining -= size;
            	continue;
            }
		}
	}
#endif
	if (lowMemVirtual) {
		lowMemVirtual=NULL;
		free(lowMemVirtual);
	}

    if(mpHeader) {
		mpHeader = NULL;
		free(mpHeader);
	}
	return error;
}


u64 pullbit64 (u64 data, unsigned int bitNumber)
{
   return (data >> bitNumber) & 1;
}

u64 pullbits64 (u64 source, int lsb, int msb)
{
   return (source >> lsb) & (((u64)1 << (msb - lsb + 1)) - 1);
}


//---------------------------------------------------------------------------
char*  ioapic (int argc, char *argv [])
{
	u64 physicalAddress;
	u32 reg0, reg1;
	int index, dimension;
	static char		*triggerText [] = {"edge","level"};
	static char  	*polarityText [] = {"high", "low"};
	static char		*maskText [] = {"no", "yes"};
	static char     *statusText [] = {"idle", "pending"};
	static char     *destmodeText [] = {"physical", "logical"};
	static char     *deliverymodeText [] = {"fixed", "lowest", "SMI", "3", "NMI", "INIT", "6", "ext"};
	char	*error=NULL;

	if (argc == 2)
		getIoapicAddress (&physicalAddress);
	else if (argc == 3) {
		error = hextoint64 (argv [2], &physicalAddress, maxAddressDigits);    if (error) return error;
	} else
		return formatMessage ("incorrect argument count");

	reg0 = readApic (physicalAddress, 0);
	reg1 = readApic (physicalAddress, 1);
	dimension = pullbits (reg1, 16, 23);
	scrollok(stdscr,TRUE);
	//printw("phy_addr:0x%x\n",physicalAddress);
	//printw("reg0:0x%x\n",reg0);
	//printw("reg1:0x%x\n",reg1);
	printw ("apic ID 0x%02x, 0x%02xh redirection entries\n", pullbits (reg0, 24, 27), dimension);
	printw("     DestID Trigger Polarity Masked error Destmode Deliverymode Vector\n");
	for (index = 0; index < dimension; index++)
	{
		u64 lopart = readApic (physicalAddress, 0x10 + index * 2 + 0);
		u64 hipart = readApic (physicalAddress, 0x10 + index * 2 + 1);
		u64 data = lopart | (hipart << 32);

		printw ("%3xh", index);
		printw ("%7x", pullbits64 (data, 56, 63));
		printw ("%8s", triggerText [pullbit64 (data, 15)]);
		printw ("%9s", polarityText [pullbit64 (data, 13)]);
		printw ("%7s", maskText [pullbit64 (data, 16)]);
		printw ("%7s", statusText [pullbit64 (data, 12)]);
		printw ("%9s", destmodeText [pullbit64 (data, 11)]);
		printw ("%13s", deliverymodeText [pullbits64 (data, 8, 10)]);
		printw ("  %02xh\n", pullbits64 (data, 0, 7));
	}
	scrollok(stdscr,FALSE);
	return error;
}


