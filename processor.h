//
/*++


Module Name:

  processor.h

Abstract:

  declares interface functions

Revision History

--*/

#ifndef _EFI_SHELL_PROCESSOR_
#define _EFI_SHELL_PROCESSOR_

static CHAR16  *mtrrTypeText [8] = {L"UC", L"WC", L"?(2)", L"?(3)", L"WT", L"WP", L"WB", L"?(7)"};

//----------------------------------------------------------------------------
// mp table

#pragma pack (push,1)
typedef struct
   {
   UINT32 signature;
   UINT32 physicalAddress;
   UINT8  length, specRev, checksum;
   UINT8  mpFeatureByte [5];
   }
MP_FLOATING_POINTER_STRUCTURE;

typedef struct
   {
   UINT8  signature [4];
   UINT16 baseTableLength;
   UINT8  specRecv, checksum;
   UINT8  oemId [8];
   UINT8  productId [12];
   UINT32 oemTablePointer;
   UINT16 oemTableSize, entryCount;
   UINT32 localApicAddesss;
   UINT16 extendedTableLength;
   UINT8  extendedtableChecksum, reserved;
   }
MP_CONFIG_TABLE_HEADER;

typedef struct
   {
   UINT8 entryType, localApicID, localApicVersion, CpuFlags;
   UINT32 CpuSignature, featureFlags, reserved [2];
   }
MP_PROCESSOR;

typedef struct
   {
   UINT8 entryType, busID;
   UINT8 busTypeString [6];
   }
MP_BUS;

typedef struct
   {
   UINT8 entryType, id, version, flags;
   UINT32 physicalAddress;
   }
MP_IO_APIC;

typedef struct
   {
   UINT8 entryType, interruptType, flags, reserved;
   UINT8 sourceBusId, sourceBusIrq, destinationIoApicId, destinationIoApicIntin;
   }
MP_IO_INTERRUPT;

typedef struct
   {
   UINT8 entryType, interruptType, flags, reserved;
   UINT8 sourceBusId, sourceBusIrq, destinationLocalApicId, destinationLocalApicIntin;
   }
MP_LOCAL_INTERRUPT;

typedef struct
   {
   UINT8 entryType, entryLength, busId, addressType;
   UINT64 addressBase, addressLength;
   }
MP_SYSTEM_ADDRESS_SPACE;   

typedef struct
   {
   UINT8 entryType, entryLength, busId, busInfo;
   UINT8 parentBus, reserved [3];
   }
MP_BUS_HIERARCHY;

typedef struct
   {
   UINT8 entryType, entryLength, busId, addressMod;
   UINT32 predifinedRangeList;
   }
MP_COMPATIBILITY_BUS;


typedef struct
   {
   UINT32 ebx; // do not change the order (to preserve vendor ID text)
   UINT32 edx; // also, debugger and app must match
   UINT32 ecx;
   UINT32 eax; 
   }
CPUID_REGS;


typedef struct
   {
   UINT32 eax, ebx, ecx, edx;
   }
CPUID_FUNCTION;

typedef struct
   {
   CPUID_FUNCTION standard [8], extended [32];
   }
CPUID_INFO;

typedef struct
   {
	CPUID_INFO mp_cpuid;
	UINT64 mp_cr0;
	UINT64 mp_cr2;
	UINT64 mp_cr3;
	UINT64 mp_cr4;
	UINT64 mp_cr8;
	UINT64 mp_msr;
	UINT32 apicId;
	UINT64 cpuspeed;

   }
MP_INFO;



typedef enum _registerNumber
   {
   _reg_msr       ,
   _reg_cr0       ,
   _reg_cr2       ,
   _reg_cr3       ,
   _reg_cr4       ,
   _reg_cr8       ,
   _reg_cpuid     ,
   _reg_intelpatch,
   }
REGISTER_NUMBER;


typedef struct
   {
   UINT8   signature [4];
   UINT32          entryPoint;
   UINT8   revision;
   UINT8   length;
   UINT8   checksum;
   UINT8   spare [5];
   }
PCI_BIOS32_STRUCT;


//----------------------------------------------------------------------------
// structure for MP communication
typedef struct
   {
   UINT32 bootFlag;  // set by application processor to signal presence to boot processor
   UINT64 data;      // points to data for the application processor
   }
MP_STRUCT;


typedef struct
   {
   unsigned reserved   :24;
   unsigned apicID     :8;
   }
LOCAL_APIC_ID_REGISTER;  // 0xFEE00020

typedef enum
   {
   _ds_none             ,
   _ds_self             ,
   _ds_allIncludingSelf ,
   _ds_allExcludingSelf ,
   }
APIC_DESTINATION_SHORTHAND;





//----------------------------------------------------------------------------

typedef struct
   {
   unsigned vector               :8;
   unsigned deliveryMode         :3;
   unsigned destinationMode      :1;
   unsigned deliveryStatus       :1;
   unsigned reserved1            :1;
   unsigned level                :1;
   unsigned triggerMode          :1;
   unsigned remoteReadStatus     :2; // documented as reserved after Pentium Pro
   unsigned destinationShorthand :2;
   unsigned reserved2            :12;
   }
INTERRUPT_COMMAND_REGISTER;  // 0xFEE00300

typedef struct
   {
   unsigned reserved             :24;
   unsigned destinationField     :8;
   }
INTERRUPT_COMMAND_DESTINATION;  // 0xFEE00310

typedef enum
   {
   _IA32_MTRRCAP           = 0xFE,
   _IA32_MTRR_PHYSBASE0    = 0x200,
   _IA32_MTRR_PHYSMASK0    = 0x201,
   _IA32_MTRR_FIX64K_00000 = 0x250,
   _IA32_MTRR_FIX16K_80000 = 0x258,
   _IA32_MTRR_FIX16K_A0000 = 0x259,
   _IA32_MTRR_FIX4K_C0000  = 0x268,
   _IA32_CR_PAT            = 0x277,
   _IA32_MTRR_DEF_TYPE     = 0x2FF,
   }
_IA32_MTRR;

typedef struct
   {
   unsigned int type             :8;
   unsigned int reserved1        :2;
   unsigned int fixedRangeEnable :1;
   unsigned int enable           :1;
   UINT64 reserved2              :53;
   }
IA32_MTRR_DEF_TYPE;

typedef struct
   {
   unsigned int vcnt       :8;
   unsigned int fix        :1;
   unsigned int reserved1  :1;
   unsigned int wc         :1;
   UINT64 reserved         :53;
   }
IA32_MTRRCAP;



typedef struct
   {
   unsigned int type         :8;
   unsigned int reserved1    :4;
   unsigned int PhysBaseLow  :20;
   unsigned int PhysBaseHigh :4;
   }
IA32_MTRR_PHYSBASE;


typedef struct
   {
   unsigned int reserved1    :11;
   unsigned int valid        :1;
   unsigned int PhysMaskLow  :20;
   unsigned int PhysMaskHigh :4;
   }
IA32_MTRR_PHYSMASK;

#pragma pack (pop)


typedef struct
   {
    UINT32 changeIt;
    UINT64 newValue;
    UINT32 msr_cr_Number;
    UINT32 cpuIndex;
   }
MP_ProcedureArgument;


void cpuidCall (UINT32 eaxRegInput, CPUID_REGS *regs);

#endif
