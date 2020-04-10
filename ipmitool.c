#include "project.h"

/* Typedefs */
#define M_E 2.71828182845904523536
#define SEGMENT_OF(s)   ((UINT)((s) >> 16))

/* Boolean values */
//#define TRUE 1
//#define FALSE 0
#define byte char
#define BYTE char
#define clock_t  u32
#define time_t  u32

//------------------------------------------------------------------------------


/* Device Addresses */
#define BMC_DEV_ADDR          0x20
#define HSC0_DEV_ADDR         0xC0
#define HSC1_DEV_ADDR         0xC2
#define PSC_DEV_ADDR          0xC8

/* Max. command/data length */
#define MAX_PACKET_LEN        0x20

/* LUNs */
#define SMS_LUN               0x02

//
// Channel Ids
//
#define IPMB_CHANNEL_ID     0
//
// Structure of an IPMB message
//
//

#pragma pack(1)
typedef struct
{
    byte rsSa;
    byte netFnRsLn;
#define     NETFN_OF(nl)    (((nl) >> 2) & 0x3F)
#define     LUN_OF(nl)      ((nl) & 0x3)

    byte cSum1;
    byte rqSa;
    byte seqRqLn;
#define     SEQ_OF(sl)  (((sl) >> 2) & 0x3F)

    byte cmd;
    byte data[1];
    // last byte of data is the second checksum
} IpmbMsg_t;
//
// Structure of a BMC request
//
typedef struct
{
    byte netFnLn;
    byte cmd;
    byte data[MAX_PACKET_LEN];
} BmcReq_t;

#define MIN_BMC_REQ_SIZE    2

typedef struct
{
    byte        length;     // length of data in bmcReq (not including netFn/cmd).
    byte        ipmbAddr;   // slave address to send request to
    BmcReq_t    bmcReq;     // body of request
} MicroReq_t;
//
// Structure of a BMC response
//
typedef struct
{
    byte netFnLn;
    byte cmd;
    byte cCode;
    byte data[MAX_PACKET_LEN];
    
} BmcResp_t;

#define MIN_BMC_RESP_SIZE   3

typedef struct
{
    byte        respLen;    // bmcResp data length only
    byte        ipmbAddr; 
    BmcResp_t   bmcResp;
    
} MicroResp_t;

#define printwBmcReq( n, r, l )      printwBuffer( (n), (char*)(r), (l) )
#define printwMicroReq( n, r )   printwBuffer( (n), (char*)&((r)->bmcReq), (r)->length + MIN_BMC_REQ_SIZE )

#ifndef INTERFACE_PRIVATE

typedef void * InterfaceHandle;

#endif // INTERFACE_PRIVATE
//
// Interface function vector
//
typedef struct _InterfaceApi
{
    void    (*configure)( InterfaceHandle intf, u16 address );
    u16    (*capabilities)( InterfaceHandle intf );
#define         IF_IPMB_ACCESS  0x01

    byte    (*bmcAddr)( InterfaceHandle intf );
    void    (*setTimeout) ( InterfaceHandle intf, int which, int what );
    char*   (*sendRequest)( InterfaceHandle intf, BmcReq_t *req, int length );
    int     (*readResponse)( InterfaceHandle intf, BmcResp_t *resp );
    int     (*smsReady)( InterfaceHandle intf );
    clock_t (*lastReqTime)( InterfaceHandle intf );
    char*  name;

} InterfaceApi;

//
// I2C Bus numbers (LSB==0 means public, LSB==1 means private)
//
#define PUBLIC_I2C_BUS              0x00
#define PRIMARY_PRIVATE_I2C_BUS     0x01
#define SECONDARY_PRIVATE_I2C_BUS   0x03

//
// Maximum possible I2C address
//
#define MAX_I2C_ADDRESS       0xFE

//
// Mask off LSBit of I2C address 
//
#define I2C_MASK              0xFE

//
// IPMI version numbers
//
#define IPMI_1_0    0x10
#define IPMI_0_9    0x09
//
// Network Functions (NetFn) already shifted 2 bits
//
#define CHASSIS_REQUEST       0x00
#define CHASSIS_RESPONSE      0x04
#define BRIDGE_REQUEST        0x08
#define BRIDGE_RESPONSE       0x0C
#define SENSOR_EVENT_REQUEST  0x10
#define SENSOR_EVENT_RESPONSE 0x14
#define APP_REQUEST           0x18
#define APP_RESPONSE          0x1C
#define FW_REQUEST            0x20
#define FW_RESPONSE           0x24
#define STORAGE_REQUEST       0x28
#define STORAGE_RESPONSE      0x2C

//
// Application commands 
//
#define GET_DEVICE_ID            0x01
#define COLD_RESET               0x02
#define DISABLE_PROCESSOR        0x32
#define GET_MESSAGE              0x33
#define SEND_MESSAGE             0x34
#define READ_SMS_MESSAGE_BUFFER  0x37
#define MASTER_WRITE_I2C         0x50
#define MASTER_READ_I2C          0x51
#define MASTER_WRITE_READ_I2C    0x52
#define DUMP_TASK                0x5D

//
// Sensor event commands
//
#define PLATFORM_EVENT_MESSAGE      0x02
#define GET_SENSOR_HISTERISIS       0x25
#define GET_SENSOR_THRESHOLD        0x27
#define GET_SENSOR_EVENT_STATUS     0x2B
#define GET_SENSOR_READING          0x2D

//
// Storage commands
//
#define GET_FRU_INVENTORY_AREA_INFO 0x10
#define READ_FRU_INVENTORY_DATA     0x11
#define WRITE_FRU_INVENTORY_DATA    0x12
#define READ_BMC_MEMORY             0x1B
#define RESERVE_SDR_REPOSITORY      0x22
#define GET_SDR                     0x23
#define PARTIAL_ADD_SDR             0x25
#define CLEAR_SDR_REPOSITORY        0x27
#define RESERVE_SEL_REPOSITORY      0x42
#define GET_SEL                     0x43
#define ADD_SEL_ENTRY               0x44
#define PARTIAL_ADD_SEL             0x45
#define CLEAR_SEL                   0x47
#define GET_SEL_TIME                0x48

//
// FW transfer commands
//
#define ENTER_FW_TRANSFER           0x00
#define FW_PROGRAM_MEMORY           0x01
#define FW_READ_MEMORY              0x02
#define GET_FW_CHECKSUM             0x03
#define EXIT_FW_TRANSFER            0x04
#define SET_SEGMENT                 0x05

/* Add SEL add operations */
#define APPEND_RECORD            0x00
#define REPLACE_RECORD           0x01

/* Partial add progress states */
#define PARTIAL_ADD_IN_PROGRESS  0x00
#define LAST_RECORD_DATA         0x01

/* Completion codes */
#define COMPCODE_NORMAL             0x00
#define COMPCODE_NODEBUSY           0xC0
#define COMPCODE_INVALIDCMD         0xC1
#define COMPCODE_INVALIDLUN         0xC2
#define COMPCODE_TIMEOUT            0xC3
#define COMPCODE_OUTOFSPACE         0xC4
#define COMPCODE_RESVTNCNCLD        0xC5
#define COMPCODE_REQDATATRUNC       0xC6
#define COMPCODE_REQDATABADLEN      0xC7
#define COMPCODE_REQDATATOOLONG     0xC8
#define COMPCODE_OFFSETOUTOFRANGE   0xC9
#define COMPCODE_RESPDATATRUNC      0xCA
#define COMPCODE_NOTFOUND           0xCB
#define COMPCODE_INVALIDFIELD       0xCC
#define COMPCODE_COMMANDILLEGAL     0xCD
#define COMPCODE_NORESPONSE         0xCE
#define COMPCODE_UNSPECIFIED        0xFF

/* I2C Completion Codes */
#define I2C_WRITE_STATUS_NORMAL        0x00
#define I2C_WRITE_STATUS_LOST_ARB      0x01
#define I2C_WRITE_STATUS_BUS_ERROR     0x02
#define I2C_WRITE_STATUS_NAK_WRITE     0x03
#define I2C_WRITE_STATUS_TRUNC_READ    0x04
#define I2C_WRITE_STATUS_NAK_ADDRESS   0x05
#define I2C_WRITE_STATUS_BAD_LENGTH    0x06
#define I2C_READ_STATUS_NORMAL         0x00

/* Request Packet Offsets */
#define REQ_OFFSET_NETFN    0x00
#define REQ_OFFSET_CMD      0x01
#define REQ_OFFSET_DATA     0x02

/* Response Packet Offsets */
#define RSP_OFFSET_ADDR     0x00
#define RSP_OFFSET_NETFN    0x01
#define RSP_OFFSET_CMD      0x02
#define RSP_OFFSET_COMPCODE 0x03
#define RSP_OFFSET_DATA     0x04

/* IMB Response Packet Offsets */
#define IMB_RSP_OFFSET_RQADDR     0x00
#define IMB_RSP_OFFSET_NETFN      0x01
#define IMB_RSP_OFFSET_CKSUM      0x02
#define IMB_RSP_OFFSET_RSADDR     0x03
#define IMB_RSP_OFFSET_RQSEQRSLUN 0x04
#define IMB_RSP_OFFSET_CMD        0x05
#define IMB_RSP_OFFSET_COMPCODE   0x06
#define IMB_RSP_OFFSET_DATA       0x07

/* Timeout(s) */
#define SMS_TIMEOUT  4000

#define dprintf if( Debug ) printw
#define vprintf if( Verbose ) printw
//
// This structure defines the response from a Get Device Id command (IPMI 1.0)
//
typedef struct
{
    BYTE    devId;
    BYTE    devRev;
#define             PROVIDES_DEV_SDRS   0x80
    BYTE    fwMajRev;
#define             FW_MODE             0x80
#define             FW_MODE_UPDATE      0x80
#define             FW_MODE_OPERATIONAL 0x00
    BYTE    fwMinRev;
    BYTE    ipmiVer;
    BYTE    devSupport;
#define             IPM_CHASSIS_DEV     0x80
#define             IPM_BRIDGE          0x40
#define             IPM_EVENT_GEN       0x20
#define             IPM_EVENT_RECV      0x10
#define             IPM_FRU_DEV         0x08
#define             IPM_SEL_DEV         0x04
#define             IPM_SDR_DEV         0x02
#define             IPM_SENSOR_DEV      0x01
    BYTE    mfgId[3];
    BYTE    prodId[2];
} DevInfo;

static BYTE mSeqNum;
static BYTE mPrevSeqNum;

u16    gIPMBRespTimeInMS;
u32   gIPMBTryCount;
u32   gSMSTOCount;
u32   gNAKWCount;
u32   gNAKACount;
u32   gLARBCount;
u32   gBERRCount;
BYTE    Debug = 0;
BYTE    gIpmiVersion = IPMI_1_0;    // figure this out later
BYTE Verbose = 0;



static MicroReq_t   LastReq;    // used for re-transmissions
clock_t clock_start_time;
static InterfaceHandle Interface = NULL;


/

//------------------------------------------------------------------------------

static void logIo (int address, int data, int type)
   {
   if (ioLogIndex == DIMENSION (ioLogArray)) return;
   ioLogArray [ioLogIndex].address = address;
   ioLogArray [ioLogIndex].data = data;
   ioLogArray [ioLogIndex].type = type;
   ioLogIndex++;
   }



#if 0

//
// function prototypes
//
u8          FindInterface (char  *intfName );
char* SendRequestToMicro( MicroReq_t *, MicroResp_t * );
//int       ReadResponseFromMicro( MicroResp_t * );

InterfaceHandle InterfaceInit (char  *intfName );
//char* SendRequest( InterfaceHandle intf, MicroReq_t *, MicroResp_t *, BYTE );
//int   ReadResponse( InterfaceHandle intf, MicroResp_t * );
int     LastReqTime( void );
void SetIPMIversion( byte vers );
u8 CheckMsTimeout(clock_t);
void SetMsTimeout( clock_t *goal, u32 timeoutInMs );
void ChecksumRecord(BYTE * , u32 );
u32 CheckCompletionCode(BYTE );
void printwStandardDisclaimer(void);
clock_t mstime (void);
BYTE PingI2C(BYTE , BYTE );

#endif

// Default SMS and SMM interface addresses
//
#define ISA_SMS_DATA          0xCA2
#define ISA_SMM_DATA          0xCA4

/* State bits based on S1 & S0 below */

#define ISA_STATE_MASK        0xC0
#define ISA_IDLE_STATE        0x00
#define ISA_READ_STATE        0x40
#define ISA_WRITE_STATE       0x80
#define ISA_ERROR_STATE       0xC0

/* Status Register Bits */
#define ISA_S1_FLAG           0x80
#define ISA_S0_FLAG           0x40
/* RESERVED                   0x20 */
/* RESERVED                   0x10 */
#define ISA_CD_FLAG           0x08
#define ISA_SMS_MSG_FLAG      0x04
#define ISA_IBF_FLAG          0x02
#define ISA_OBF_FLAG          0x01

 /* SMS Transfer Stream Control Codes */
                                
#define ISA_GET_STATUS_ABORT   0x60
#define ISA_WRITE_START        0x61
#define ISA_WRITE_END          0X62
#define ISA_READ               0X68

#define BMC_SLAVE_ADDR 0x20                                 
#define MAX_ISA_LENGTH  35

#define ISA_SMS_TIMEOUT 5000    // in miliseconds - 1 second
#define ISA_SMM_TIMEOUT 100     // in microseconds


typedef struct
   {
   char *          ifName;
   u16            dataAddr;
   InterfaceApi *  interface_;
   } IsaIfInfo;


typedef struct _IsaInterface
{
   InterfaceApi    interface_;  // array of function pointers
   u16            dataAddr;   // address of interface
   clock_t         reqTime;    // time it took last send, not valid between send and recv
   u16            Controller; // Controller type
   u16            ReqBMCAddr; // BMC Address in request

} IsaInterface;

static int ioDebugLog = 0;
typedef struct
{
   u16 address;
   u8  data;
   u8  type;
}
IO_LOG;


int LastReqTime( void )
{
    return 0; // Interface->lastReqTime( Interface );
}

clock_t  mstime (void)
{
	return 0;
  //return  (clock_t ) queryPerformanceCounter () * 1000 / (clock_t )queryPerformanceFrequency ();
}

void SetIPMIversion( byte vers )
{
    gIpmiVersion = vers;
}

void SetMsTimeout( clock_t *goal, UINT32 timeoutInMs )
{
   clock_start_time = mstime();
   *goal = clock_start_time + timeoutInMs;
}


BOOLEAN CheckMsTimeout(clock_t goal)
{
   clock_t clock_reading;

   clock_reading = mstime();
   
   if (clock_reading >= goal)
   {
      return TRUE;
   }
   else
      return FALSE;
}


static u8 kcsReadIoPort (u32 address)
{
	u8 data;

   	data = (u8)ReadIoPort (address, 8);
   	if (ioDebugLog) logIo (address, data, 0);
   	return data;
}

//------------------------------------------------------------------------------

static void kcsWriteIoPort (u32 address, u8 data)
{
   if (ioDebugLog) logIo (address, data, 1);
   WriteIoPort (address, 8, data);
}

//------------------------------------------------------------------------------

static int ISA_OBF (int kcsBase)
{
   int kcsStatus = kcsBase + 1;
   if (kcsBase == 0x62) kcsStatus = 0x66;
   return kcsReadIoPort (kcsStatus) & ISA_OBF_FLAG;
}

//------------------------------------------------------------------------------

static int ISA_IBF (int kcsBase)
   {
   int kcsStatus = kcsBase + 1;
   if (kcsBase == 0x62) kcsStatus = 0x66;
   return kcsReadIoPort (kcsStatus) & ISA_IBF_FLAG;
   }

//------------------------------------------------------------------------------

static int ISA_MSG (int kcsBase)
   {
   int kcsStatus = kcsBase + 1;
   if (kcsBase == 0x62) kcsStatus = 0x66;
   return kcsReadIoPort (kcsStatus) & ISA_SMS_MSG_FLAG;
   }

//------------------------------------------------------------------------------

static int ISA_STATE (int kcsBase)
   {
   int kcsStatus = kcsBase + 1;
   if (kcsBase == 0x62) kcsStatus = 0x66;
   return kcsReadIoPort (kcsStatus) & ISA_STATE_MASK;
   }

//------------------------------------------------------------------------------

static int ISA_STATUS (int kcsBase)
   {
   int kcsStatus = kcsBase + 1;
   if (kcsBase == 0x62) kcsStatus = 0x66;
   return kcsReadIoPort (kcsStatus);
   }

//------------------------------------------------------------------------------

static int ISA_READ_BMC_DATA (int kcsBase)
   {
   return kcsReadIoPort (kcsBase);
   }

//------------------------------------------------------------------------------

static void ISA_WRITE_BMC_DATA (int kcsBase, int data)
   {
   kcsWriteIoPort (kcsBase,(u8) data);
   }

//------------------------------------------------------------------------------

static void ISA_WRITE_BMC_CMD (int kcsBase, int data)
   {
   int kcsCommand = kcsBase + 1;
   if (kcsBase == 0x62) kcsCommand = 0x66;
   kcsWriteIoPort (kcsCommand,(u8) data);
   }


static void IsaIntfConfigure( IsaInterface * intf, u16 addr )
   {
   intf->dataAddr      = addr;
   }

//------------------------------------------------------------------------------

static u16 IsaSmsCapabilities( IsaInterface *intf )
   {
   return IF_IPMB_ACCESS;
   }

//------------------------------------------------------------------------------

static byte IsaBmcAddr( IsaInterface *intf )
   {
   return BMC_DEV_ADDR;
   }

//------------------------------------------------------------------------------

static void IsaSmsSetTimeout( IsaInterface *intf, int which, int what )
   {
   ;   // not yet
   }

//------------------------------------------------------------------------------

static int IsaSmsReady( IsaInterface * intf )
   {
   return ISA_MSG(intf->dataAddr);
   }

//------------------------------------------------------------------------------

static clock_t IsaLastReqTime( IsaInterface * intf )
   {
   return intf->reqTime;
   }

//------------------------------------------------------------------------------
// Isa SMM routines
//

static u16 IsaSmmCapabilities( IsaInterface * intf )
   {
   return 0;
   }

void printISAStatus( char ISACmdStatus )
{
  
	scrollok(stdscr,TRUE);
   	printw( "ISA Registers\n");
   	printw( "\n");

   	printw( "Command/Status:     %02Xh\n", ISACmdStatus);
   	printw( "-------------------------\n");
   	printw( "   S1:              %02Xh\n", (ISACmdStatus & ISA_S1_FLAG));
   	printw( "   S0:              %02Xh\n", (ISACmdStatus & ISA_S0_FLAG));
   	printw( "   RESERVED:        %02Xh\n", (ISACmdStatus & 0x20));
   	printw( "   RESERVED:        %02Xh\n", (ISACmdStatus & 0x10));
   	printw( "   C/D#:            %02Xh\n", (ISACmdStatus & ISA_CD_FLAG));
   	printw( "   SMS_ATN/SMM_ATN: %02Xh\n", (ISACmdStatus & ISA_SMS_MSG_FLAG));
   	printw( "   IBF:             %02Xh\n", (ISACmdStatus & ISA_IBF_FLAG));
   	printw( "   OBF:             %02Xh\n", (ISACmdStatus & ISA_OBF_FLAG));
   	printw( "\n");
   	printw( "   STATE (S1 + SO): ");

   	switch(ISACmdStatus & ISA_STATE_MASK)
    {
      case ISA_IDLE_STATE:
         printw( "IDLE\n");
         break;
      case ISA_READ_STATE:
        printw( "READ\n");
         break;
      case ISA_WRITE_STATE:
        printw( "WRITE\n");
         break;
      case ISA_ERROR_STATE:
        printw( "ERROR\n");
         break;
      default:
         break;
	}
	scrollok(stdscr,FALSE);
}


//------------------------------------------------------------------------------

static void IsaSmmSetTimeout( IsaInterface * intf, int which, int what )
   {
   ;
   }
#define SMM_QUANTUM 1

static int wait_while_smm_ibf( u16 addr, u16 timeout )
   {           
   for ( ; ISA_IBF(addr) ; timeout -= SMM_QUANTUM )
      {
      // usDelay( SMM_QUANTUM );

      if ( timeout <= SMM_QUANTUM ) 
         {
         printISAStatus((char) ISA_STATUS((int)addr) );
         return -1;
         }              
      }
   return 0;
   }

static int wait_while_sms_ibf( u16 addr, u16 timeout)
    {           
   clock_t overallTimeout;

   SetMsTimeout( &overallTimeout, timeout);
   

   while ( ISA_IBF(addr) )
      {     
      if ( CheckMsTimeout(overallTimeout) ) 
         {
         printISAStatus((char) ISA_STATUS(addr) );
         return(-1); /* timed out */
         }              
      }

   return(0);        /* have message, no timeout */
   }

static int wait_til_obf( u16 addr, u16 timeout)
     {       
   clock_t overallTimeout;

   SetMsTimeout( &overallTimeout, timeout);

   while ( !ISA_OBF(addr) )
      {        
      if ( CheckMsTimeout(overallTimeout) ) 
         {
         return(-1); /* timed out */
         }              
      }                                                    

   return(0);        /* have message, no timeout */
   }


/* -----------------------------------------------------------------------------
Name:    IsaSmsSendRequest
Purpose: Sends cmds to the BMC
Returns: 0 success, -1 failure
Parameters:
msgBuf - pointer to source buffer
Notes:
----------------------------------------------------------------------------- */

static char* IsaSmsSendRequest( IsaInterface * intf, BmcReq_t *req, int length )
   { 
   char *msgBuf = (byte *) req;
   int    i;
   char    state;
   char    nstatus;
   char*				Status;
   Status          = NULL; 
   length += MIN_BMC_REQ_SIZE;

	scrollok(stdscr,TRUE);
   	printw( "IsaSmsSendRequest: intf struct at 0x%x, 0x%x bytes\n",  (u32*) intf, length );

   	intf->reqTime = mstime();
  	printw( "req start time %lX\n", intf->reqTime );
	scrollok(stdscr,FALSE);
   //
   // wait for BMC to read whatever might be in the input buffer
   //
   if ( wait_while_sms_ibf( intf->dataAddr, ISA_SMS_TIMEOUT ) ) 
      	return formatMessage ("ERROR: Timeout waiting for IBF to clear before sending WRITE START");
   //
   // clear any OBF
   //
   nstatus = (char)ISA_STATUS((int)intf->dataAddr);

   if ( nstatus & ISA_OBF_FLAG )                 /* flush output buffer */
      {
      ISA_READ_BMC_DATA((int)intf->dataAddr);
      }
   //
   // make sure we're starting in idle or error state
   //
   state = nstatus & ISA_STATE_MASK;

   if (state != ISA_ERROR_STATE && state != ISA_IDLE_STATE)
      {
		scrollok(stdscr,TRUE);
      	printw( "ERROR: BMC Interface not in idle or error state\n");
		scrollok(stdscr,FALSE);

      if ( Debug )
         printISAStatus( nstatus );
      //
      // try forcing into error state
      //
      ISA_WRITE_BMC_CMD( intf->dataAddr, ISA_ERROR_STATE );

      if ( wait_while_sms_ibf( intf->dataAddr, ISA_SMS_TIMEOUT ) ) 
         return formatMessage ("ERROR: Timeout waiting for IBF to clear after sending ERROR.\n");
      //
      // flush any outpound data
      //
      if( ISA_OBF(intf->dataAddr) )
         (void) ISA_READ_BMC_DATA(intf->dataAddr);

      // return -1;
      }
   //
   // get the BMC's attention and wait until he reads it
   // 
   ISA_WRITE_BMC_CMD( intf->dataAddr, ISA_WRITE_START );

   if ( wait_while_sms_ibf( intf->dataAddr, ISA_SMS_TIMEOUT ) ){
      return formatMessage ("ERROR: Timeout waiting for IBF to clear after sending WRITE START.\n");
	}
   //
   // send each message byte out
   //
   for ( i = 0; i < length - 1; i++ )              /* send the message */
      {                   
      nstatus =(char) ISA_STATUS((int)intf->dataAddr);

      if ( nstatus & ISA_OBF_FLAG )                 /* flush output buffer */
         {
         ISA_READ_BMC_DATA(intf->dataAddr);
         }

      if ( (nstatus & ISA_STATE_MASK) != ISA_WRITE_STATE )  /* should have gone to */
         return formatMessage ("ERROR: BMC did not enter WRITE STATE.\n");

      ISA_WRITE_BMC_DATA( intf->dataAddr, msgBuf[i] );             /* output data byte */

      if ( wait_while_sms_ibf( intf->dataAddr, ISA_SMS_TIMEOUT ) )         /* wait til BMC reads */
         return formatMessage ("ERROR: Timeout waiting for IBF to clear after sending data byte.\n");
      }

   ISA_WRITE_BMC_CMD( intf->dataAddr, ISA_WRITE_END );             /* flag next byte is last */

   if ( wait_while_sms_ibf( intf->dataAddr, ISA_SMS_TIMEOUT ) )            /* wait til BMC reads */
      return formatMessage ("ERROR: Timeout waiting for IBF to clear after sending WRITE END.\n");
   nstatus =(char) ISA_STATUS((int)intf->dataAddr);

   if ( nstatus & ISA_OBF_FLAG )                 /* flush output buffer */
      {
      ISA_READ_BMC_DATA(intf->dataAddr);
      }

   if ( (nstatus & ISA_STATE_MASK) != ISA_WRITE_STATE )  /* should have stayed in */
      return formatMessage ("ERROR: BMC did not stay in WRITE STATE.\n");

   ISA_WRITE_BMC_DATA( intf->dataAddr, msgBuf[i] );                /* output last data byte */

   if ( wait_while_sms_ibf( intf->dataAddr, ISA_SMS_TIMEOUT ) )            /* wait til BMC reads */
      return formatMessage ("ERROR: Timeout waiting for IBF to clear after sending last data byte.\n");

   if ( ISA_STATE(intf->dataAddr) != ISA_READ_STATE ) /* and verify BMC went */
      return formatMessage ("ERROR: BMC did not enter READ STATE.\n");
   else
      return Status;
   }

/* -----------------------------------------------------------------------------
Name:    IsaSmsReadResponse
Purpose: Reads response data through the SMS/BMC ISA interface.
Returns: count - number of data bytes read (first three are NetFn,system sw ID, and cmd)
Parameters:
msgBuf - pointer to destination buffer
Notes:
----------------------------------------------------------------------------- */

static int IsaSmsReadResponse( IsaInterface * intf, BmcResp_t * resp )
   {
   char *  msgBuf = (char *) resp;                
   int     i;
   char    state;
   char    status;

scrollok(stdscr,TRUE);
   printw( "IsaSmsReadResponse: intf struct at 0x%x, dataAddr %xh\n", (u32*) intf, intf->dataAddr );
scrollok(stdscr,FALSE);


   i = 0;

   while( i < MAX_ISA_LENGTH )
      {
      //
      // make sure the BMC saw our last request
      //
      if ( wait_while_sms_ibf( intf->dataAddr, ISA_SMS_TIMEOUT) )
         {                           
         intf->reqTime = 0;
scrollok(stdscr,TRUE);
         printw( "IsaSmsReadResponse: IBF timeout, respLen %d\n", i );
scrollok(stdscr,FALSE);

         return -2;
         }
      //
      // test for state
      //
      status =(char) ISA_STATUS((int)intf->dataAddr);

      // printw( L"OW-S%02X ", status);

      state = (char)(status & ISA_STATE_MASK);
      //
      // check to see if we're done
      //
      if ( state == ISA_IDLE_STATE )
         {
         //
         // read and ignore our handshake (clears OBF)
         //
         status = (char)ISA_READ_BMC_DATA((int)intf->dataAddr);

scrollok(stdscr,TRUE);
         printw( "req end time %x\n", mstime() );

        intf->reqTime = mstime() - intf->reqTime;    // calculate duration 

         printw( "IsaSmsReadResponse: read %d bytes\n", i );
scrollok(stdscr,FALSE);

         return i;
         }
      //
      // any state other than read is an error
      //
      else if( state != ISA_READ_STATE )
         {
scrollok(stdscr,TRUE);
         printw( "IsaSmsReadResponse: intf in wrong state (%02Xh), respLen %d\n",
            status, i);
scrollok(stdscr,FALSE);
         intf->reqTime = 0;

         return -1;
         }
      //
      // we're in read state
      // wait until the BMC gives us our data
      //
      if ( wait_til_obf( intf->dataAddr, ISA_SMS_TIMEOUT) )
         {
         intf->reqTime = 0; 
scrollok(stdscr,TRUE);
         printw( "IsaSmsReadResponse: OBF timeout, respLen %d\n", i );
scrollok(stdscr,FALSE);

         return -3;
         }
      //
      // capture our data and ask for the next
      //
      msgBuf[i++] = (char)ISA_READ_BMC_DATA((int)intf->dataAddr);

      ISA_WRITE_BMC_DATA( intf->dataAddr, ISA_READ );
      }
   //
   // too many data bytes, complain vociferously
   //
scrollok(stdscr,TRUE);
   printw( "IsaSmsReadResponse: buffer overrun\n" ); 
scrollok(stdscr,FALSE);

   intf->reqTime = 0; 

   return -3;
   }


static char* IsaSmmSendRequest( IsaInterface * intf, BmcReq_t * req, int length )
   {
   byte *  msgBuf = (byte *) req;
   int     i;
   char*				Status;
   Status          = NULL; 

   length += MIN_BMC_REQ_SIZE;
   //
   // wait until we can stuff our request in the buffer
   //
   if ( wait_while_smm_ibf( intf->dataAddr, ISA_SMM_TIMEOUT ) )                    /* BMC could be busy */
      return formatMessage ("ERROR: Timeout waiting for IBF to clear before sending WRITE START.\n");
   ISA_WRITE_BMC_CMD( intf->dataAddr, ISA_WRITE_START );           /* get the BMC's attention */
   //
   // make sure he gets the write request
   //
   if ( wait_while_smm_ibf( intf->dataAddr, ISA_SMM_TIMEOUT ) )            /* wait til BMC reads */
      return formatMessage ("ERROR: Timeout waiting for IBF to clear after sending WRITE START.\n");
   //
   // write out our message
   //
   for ( i = 0; i < length - 1; i++ )              /* send the message */
      {                   
      char nstatus =(char) ISA_STATUS((int)intf->dataAddr);

      if ( nstatus & ISA_OBF_FLAG )                 /* flush output buffer */
         ISA_READ_BMC_DATA(intf->dataAddr);
      //
      // make sure the BMC entered WRITE state
      //
      if ( (nstatus & ISA_STATE_MASK) != ISA_WRITE_STATE )
         return formatMessage ("ERROR: BMC did not enter WRITE STATE.\n");
      //
      // give the BMC our data byte
      //
      ISA_WRITE_BMC_DATA( intf->dataAddr, msgBuf[i] );
      //
      // make sure it takes it
      //
      if ( wait_while_smm_ibf( intf->dataAddr, ISA_SMM_TIMEOUT ) )
         return formatMessage ("ERROR: Timeout waiting for IBF to clear after sending data byte.\n");
      }
   //
   // tell the BMC that the next byte is the last in the request
   // and wait for it to get the command
   //
   ISA_WRITE_BMC_CMD( intf->dataAddr, ISA_WRITE_END );

   if ( wait_while_smm_ibf( intf->dataAddr, ISA_SMM_TIMEOUT ) )
      return formatMessage ("ERROR: Timeout waiting for IBF to clear after sending WRITE END.\n");
   //
   // give the BMC the last byte
   // and wait for it to take it
   //
   ISA_WRITE_BMC_DATA( intf->dataAddr, msgBuf[i] );

   if ( wait_while_smm_ibf( intf->dataAddr, ISA_SMM_TIMEOUT ) )
      return formatMessage ("ERROR: Timeout waiting for IBF to clear after sending last data byte.\n");
   //
   // make sure the BMC goes into READ state
   //
   if ( ISA_STATE((int)intf->dataAddr) != ISA_READ_STATE )
      return formatMessage ("ERROR: BMC did not enter READ STATE.\n");
   else
      return Status;
   }

//------------------------------------------------------------------------------

static int IsaSmmReadResponse( IsaInterface * intf, BmcResp_t * resp )  
   {
   byte *  msgBuf = (byte *) resp;
   int     i;
   char    state;
   char    status;
   u16    loopCnt;

   for (i = 0; i < MAX_ISA_LENGTH; i++)
      {
      //
      // get a byte
      //
      loopCnt = 1000;

      do
         {
         //
         // wait until there's data to read or the interface
         // is idle or has an error
         //
         status =(char) ISA_STATUS(intf->dataAddr);

         state = (char)(status & ISA_STATE_MASK);

         if (state == ISA_ERROR_STATE)
            {
			scrollok(stdscr,TRUE);
            printw(L"Interface error after %d bytes read\n", i);
			scrollok(stdscr,FALSE);
            printISAStatus((char) ISA_STATUS(intf->dataAddr) );
            return -1;
            }
         else if (state == ISA_IDLE_STATE)
            {
            // printf( "Interface idle after %d bytes read\n", i);

            return i;
            }
         else if ( loopCnt-- <= 0 )
            {

			scrollok(stdscr,TRUE);
            printw("Interface timeout after %d bytes read\n", i);
			scrollok(stdscr,FALSE);
            printISAStatus( (char)ISA_STATUS((int)intf->dataAddr) );
            return -1;
            }
         }
         while ( (status & ISA_OBF_FLAG) == 0 );

         msgBuf[i] =(char) ISA_READ_BMC_DATA((int)intf->dataAddr);

         //
         // wait for 10 us and then make sure that the BMC has
         // taken our last command byte
         //
         // usDelay( (u16) 10 );

         //for (  ISA_SMM_IBF ; loopCnt-- )
         //  ;
         //if ( ISA_SMM_IBF )
         //{
         //  printISAStatus( ISA_SMM_STATUS );
         //  return -1;
         //}
         //
         // ask for the next byte
         //
         //ISA_SMM_WRITE_BMC_DATA( ISA_READ );
      }
	scrollok(stdscr,TRUE);
   	printw( "Receive buffer overflow\n" );
	scrollok(stdscr,FALSE);
   	return -1;
   }


//------------------------------------------------------------------------------
//
// Interface structure for the ISA SMS interface
//

static InterfaceApi IsaSmsInterface = {
   IsaIntfConfigure,
   IsaSmsCapabilities,
   IsaBmcAddr,
   IsaSmsSetTimeout,
   IsaSmsSendRequest,
   IsaSmsReadResponse,
   IsaSmsReady,
   IsaLastReqTime,  
   "isa_sms"
   };
//
// Interface structure for the ISA SMM interface
//
static InterfaceApi IsaSmmInterface = {
   IsaIntfConfigure,
   IsaSmmCapabilities,
   IsaBmcAddr,
   IsaSmmSetTimeout,
   IsaSmmSendRequest,
   IsaSmmReadResponse,
   NULL,
   IsaLastReqTime,  
   "isa_smm"
   };

static IsaIfInfo IsaInterfaces[] =
   {
      { "isa",     ISA_SMS_DATA, & IsaSmsInterface  },   // alias for isa_sms
      { "isa_sms", ISA_SMS_DATA, & IsaSmsInterface  },
      { "isa_smm", ISA_SMM_DATA, & IsaSmmInterface  },
      { NULL,     0,      NULL }
   };


#define MAX_PER_LINE      16
void PrintBuffer( char *name, char  *buf, UINT32  length )
{
    UINT32  count = 0;


	scrollok(stdscr,TRUE);
    printw("%s:\n", name);
        
    for ( ; count < length ; count++ )
    {
        if( count && (count % MAX_PER_LINE) == 0 )
        {
            printw("\n");
        }
        printw("%02x ", (CHAR16)buf[count] );
    }
    printw("\n");
	scrollok(stdscr,FALSE);
}

#define PrintBmcReq( n, r, l )      PrintBuffer( (n), (CHAR8*)(r), (l) )
#define PrintMicroReq( n, r )   PrintBuffer( (n), (CHAR8*)&((r)->bmcReq), (r)->length + MIN_BMC_REQ_SIZE )



InterfaceHandle IsaIntfPresent( char *str )
{
	u32    i;
   	u32    dataAddr = 0;
   	char  *addrStr;
   	IsaInterface *intf;

   //
   // see if this is a generic search
   //  
   if ( str == NULL ){
   		//
      	// only search for the standard SMS if.
      	//

		i = 0;
		scrollok(stdscr,TRUE);
      	printw( "IsaIntfPresent: NULL name, will test for first if\n");
		scrollok(stdscr,FALSE);
	} else {

     	//addrStr = strchr( str, ':' );
     	//*addrStr++ = '\0';
	    // (void) sscanf( addrStr, "%x", & dataAddr );
    	// hextoint64 (addrStr, (UINT64*)&dataAddr, maxAddressDigits); 
        

      	//
      	// look for the interface name
      	//
      	for( i = 0 ; IsaInterfaces[i].ifName ; i++ )
		{
        	if( strcmp( str, IsaInterfaces[i].ifName) == 0 ) break;
        }
      	//
      	// check for failure to find the name
      	//
      	if( IsaInterfaces[i].ifName == NULL ) {
			scrollok(stdscr,TRUE);
         	printw("IsaIntfPresent: can't find name\n");
			scrollok(stdscr,FALSE);
         	return NULL;
        }
		scrollok(stdscr,TRUE);
      	printw("IsaIntfPresent: found name" );
		scrollok(stdscr,FALSE);
      	//if(Debug)printwMultiChar (str);
   	}

	if( dataAddr == 0 )
    	dataAddr = IsaInterfaces[i].dataAddr;
		scrollok(stdscr,TRUE);
   		printw("IsaIntfPresent: found intf at %xh\n", dataAddr );
		scrollok(stdscr,FALSE);
   		//
   		// create an interface structure to pass back
   		//
   		intf = (IsaInterface *) malloc( sizeof(IsaInterface) );
		if (NULL == intf ) {
			scrollok(stdscr,TRUE);
   			printw(" Malloc IsaInterface fail!\n");
			scrollok(stdscr,FALSE);
			return NULL;
		}

   		intf->interface_ = * IsaInterfaces[i].interface_;
   		intf->dataAddr  =(u16) dataAddr;

		scrollok(stdscr,TRUE);
   		printw("IsaIntfPresent: intf struct at 0x%x\n", (u32*) intf );
		scrollok(stdscr,FALSE);

   		return (InterfaceHandle) intf;
}



static InterfaceHandle ( * Interfaces[] )() =
{
    IsaIntfPresent,
    NULL
};

InterfaceHandle InterfaceInit( char  *intfName )
{
    int             i;
    InterfaceHandle h = NULL;
    //
    // initialize interface related varibles
    //
    gIPMBTryCount = 0;
    gNAKWCount = 0;
    gNAKACount = 0;
    gLARBCount = 0;
    gBERRCount = 0;
    gSMSTOCount = 0;

    //srand(mstime ());//88888888888888888888888888

   // mSeqNum = (BYTE)(rand() & 0xFF);
   mSeqNum = 15;
    //
    // look for the interface
    //
    //
    for ( i = 0 ; Interfaces[i] ; i++ )
    {
        if( (h = Interfaces[i]( intfName )) != NULL )
        {
            break;
        }
    }
    return h;
}


u8  FindInterface( char  *intfName )
{
    Interface = InterfaceInit( intfName );

    if(Interface != NULL)
		return 1;
    else
		return 0;
}




static char*	 ExitWithUsage(void)
{
   	char *			Status;
   	Status = NULL;

	scrollok(stdscr,TRUE);
   	printw("\n");
   	printw("Usage: IpmiTool [-i9][-if interface] IPMBAddr NetFnLUN Cmd [Data1 ... DataN]\n");
   	printw("  -i9      Use IPMI 0.9 commands instead of IPMI 1.0 Send Msg/Get Msg (default)\n");
   	printw("  -if <if> Select BMC interface to use (default:isa_sms)\n");
   	printw("           Supported interfaces: isa_sms, isa_smm, smb\n");
   	printw("  IPMBAddr IPMB address of controller (e.g., BMC = 20, HSC = C0, mBMC = 84)\n");
   	printw("  NetFnLUN Command net function (high 6 bits) and LUN (low 2 bits)\n");
   	printw("  Cmd      Command BMC to perform\n");
   	printw("  Data1..N Command data\n");

	scrollok(stdscr,FALSE);
   return Status;
}


//---------------------------------------------------------------------------

char*	ipmi_hexAsciiDump (u8 *mmIoBase,u64 physicalAddress, u64 count)
{
	char*	Status = NULL;
	u64 	maxAddress;
	int 	addressColumns;

	maxAddress = physicalAddress + count * sizeof mmIoBase[0]  - 1;
	addressColumns = (highestSetBit (&maxAddress, sizeof maxAddress)) / 4 + 1;

	// do hex-ascii dump
	while (count )

	{
		int offset, index;
		char  buffer [81];

		memset(buffer,0,sizeof(buffer));
		for (index = 0; index < 81; index++) {
			buffer[index] =' ';

		}
		snprintf(buffer,  sizeof (buffer) ,"%0*x-",physicalAddress);
		
		for (index = 0, offset = addressColumns - 2; index < 16; index++)
		{
			char  value = *mmIoBase++, ascii;
			offset += 3;
			if (index == 8) offset += 2;
			snprintf(buffer + offset,sizeof (buffer) -offset *2, "%02x", value);
			buffer [offset + 2] = ' '; /* remove NULL */

			if (isprint (value))
				ascii = value;
			else
				ascii = '.';
			buffer [(index + addressColumns + 53)] = (char)ascii;  
			physicalAddress++;
			count--;
			if (count == 0) break;
		}
		buffer [index + addressColumns + 53] = '\0';
		printw ("%s\n", buffer);
		if (NULL ==  kbhit ())
		{
			 return formatMessage ("dump canceled");
		}
	}
	return Status;
}

/*******************************************************************************
*  Routine: ChecksumRecord
*
*  Arguments:
*     Buffer - buffer to checksum
*     DataLength - length of buffer, includes to-be-added checksum
*
*  Returns:
*     N/A
*
*  Description:
*     Adds a checksum to the end of the buffer.  The checksum is a
*     two's complement of the sum of the buffer (in a BYTE).
*
*******************************************************************************/
void ChecksumRecord( BYTE * Buffer, UINT32 DataLength )
{
   UINT32 i;
   UINT32 Sum;
   
   Sum = 0;
   DataLength--;
   
   for(i=0; i < DataLength; i++)
   {
      Sum += Buffer[i];
   }
   Buffer[DataLength] = (BYTE)(((~Sum) + 1) & 0xFF);
}

/*******************************************************************************
*  Routine: BuildIpmbReq
*
*  Arguments:
*     IpmbMsg_t *   msg     - where to build the request
*     MicroReq_t *  req     - request providing the cmd/data
*
*  Returns:
*     length of the built request
*
*  Description:
*     This routine builds an IPMB request from a BMC request.
*     
*******************************************************************************/

UINT32 BuildIpmbReq ( IpmbMsg_t * msg, MicroReq_t * req )
{
    //
    // build an IPMB message
    //
    msg->rsSa       = req->ipmbAddr;        /* slave addr */
    msg->netFnRsLn  = req->bmcReq.netFnLn;  /* NetFn + LUN (LUN = 0) */
    msg->cSum1      = -(CHAR8)(msg->rsSa + msg->netFnRsLn);

    msg->rqSa       = BMC_DEV_ADDR;       /* BMC's slave addr       */

    mPrevSeqNum     = (BYTE)(mSeqNum & 0x3F);

    msg->seqRqLn    = (BYTE)(((mSeqNum) << 2) | SMS_LUN);  /* return to ISA */
    msg->cmd        = req->bmcReq.cmd;

    memcpy( msg->data, req->bmcReq.data, req->length );
    ChecksumRecord( & msg->rqSa, req->length + 4);     // add rqsa, seq, cmd, checksum 

    mSeqNum++;

    return req->length + 7;     // add in IPMB overhead (7) 
}


/*******************************************************************************
*  Routine: FlushSMSBuffer
*
*  Arguments:
*     N/A
*
*  Returns:
*     N/A
*
*  Description:
*     This routine flushes the SMS Buffer.
*     
*******************************************************************************/

static void FlushSMSBuffer( InterfaceApi *intf)
{
    BmcResp_t   bResp;
    BmcReq_t    bReq;
    BYTE        reqLength;

    //if( Debug )
    //    printw("FlushSMSBuffer\n" );

    if (intf->smsReady != NULL) {
        while( intf->smsReady(intf) )
        {
            bReq.netFnLn = APP_REQUEST;

            bReq.cmd = ( gIpmiVersion == 0x09 ) ? READ_SMS_MESSAGE_BUFFER : GET_MESSAGE;
            reqLength = 0;

            //if( Debug )
            //    printw( "Flushing SMSBuffer\n" );

            if( intf->sendRequest( intf, & bReq, reqLength) != 0 )
            {
				scrollok(stdscr,TRUE);
                printw( "Error sending Read SMS Message Buffer command while flushing SMS Buffer.\n");
				scrollok(stdscr,FALSE);
                return;
            }
            //
            // Read and throw away the contents
            //
            (void) intf->readResponse( intf, & bResp);
        }
    }
}

struct Assoc
{
    BYTE value;
    CHAR16 *  str;
} CompCodes[] =
{
    { COMPCODE_NORMAL,      "Ok" },
    { COMPCODE_NODEBUSY,   "NODE BUSY" },
    { COMPCODE_INVALIDCMD,  "INVALID COMMAND" },
    { COMPCODE_INVALIDLUN,  "INVALID LUN" },
    { COMPCODE_TIMEOUT,     "TIMEOUT" },
    { COMPCODE_OUTOFSPACE,  "OUT OF SPACE" },
    { COMPCODE_RESVTNCNCLD, "RESERVATION CANCELED" },
    { COMPCODE_REQDATATRUNC,"REQUEST DATA TRUNCATED" },
    { COMPCODE_REQDATABADLEN,   "REQUEST DATA BAD LENGTH" },
    { COMPCODE_REQDATATOOLONG,  "REQUEST DATA TOO LONG" },
    { COMPCODE_OFFSETOUTOFRANGE,"OFFSET OUT OF RANGE" },
    { COMPCODE_RESPDATATRUNC,   "RESPONSE DATA TRUNCATED" },
    { COMPCODE_NOTFOUND,        "NOT FOUND" },
    { COMPCODE_INVALIDFIELD,    "INVALID FIELD" },
    { COMPCODE_COMMANDILLEGAL,  "COMMAND ILLEGAL" },
    { COMPCODE_NORESPONSE,      "NO RESPONSE" },
    { COMPCODE_UNSPECIFIED,     "UNSPECIFIED" },
    { I2C_WRITE_STATUS_LOST_ARB,    "I2C WRITE - LOST ARBITRATION" },
    { I2C_WRITE_STATUS_BUS_ERROR,   "I2C WRITE - BUS ERROR" },
    { I2C_WRITE_STATUS_NAK_WRITE,   "I2C WRITE - NAK ON WRITE" },
    { I2C_WRITE_STATUS_TRUNC_READ,  "I2C WRITE - TRUNCATED READ" },
    { I2C_WRITE_STATUS_NAK_ADDRESS, "I2C WRITE - NAK ON ADDRESS" },
    { I2C_WRITE_STATUS_BAD_LENGTH,  "I2C WRITE - BAD LENGTHS" },
    { 0x00, NULL },
};


UINT32 CheckCompletionCode( BYTE cCode )
{ 
    CHAR16 *str = NULL;
    
    if( cCode != COMPCODE_NORMAL )
    {
        struct Assoc *a = CompCodes;
        
        while( a->str != NULL )
        {
            if( cCode == a->value )
            {
                str = a->str;
                break;
            }
            a++;         
        }
        if( str )
            printw( "Error from micro: %s\n", str);
        else
            printw( "Error from micro: unrecognized completion code %02xh\n", cCode);
    }
   return (cCode != COMPCODE_NORMAL);
}


/*******************************************************************************
*  Routine: IsI2CRetryNeeded
*
*  Arguments:
*     CompCode - the completion code from an I2C operation
*
*  Returns:
*     TRUE if retry needed
*     FALSE otherwise
*
*  Description:
*     This routine returns TRUE if an I2C retry is needed
*     
*******************************************************************************/
static u8 IsI2CRetryNeeded(BYTE CompCode)
{
   BOOLEAN  Retry;

   gIPMBTryCount++;   
	scrollok(stdscr,TRUE);
   switch(CompCode)
   {
      case I2C_WRITE_STATUS_NAK_WRITE:
         Retry = TRUE;
         printw("NAKW ");
         gNAKWCount++;
         break;
      case I2C_WRITE_STATUS_NAK_ADDRESS:
         Retry = TRUE;
         printw("NAKA ");
         gNAKACount++;
         break;
      case I2C_WRITE_STATUS_LOST_ARB:
         Retry = TRUE;
         printw("LARB ");
         gLARBCount++;
         break;
      case I2C_WRITE_STATUS_BUS_ERROR:
         Retry = TRUE;
         printw("BERR ");
         gBERRCount++;
         break;
      default:
         Retry = FALSE;
         CheckCompletionCode(CompCode);
         break;
   }
	scrollok(stdscr,FALSE);
   return Retry;
}


/*******************************************************************************
*  Routine: SendRequest
*
*  Arguments:
*     InterfaceApi *intf
*     MicroReq_t *    cmd
*
*  Returns:
*     0 success, -1 failure
*
*  Description:
*     This routine sends commands the BMC or micros on the IPMB.
*     
*******************************************************************************/

char* SendRequest( InterfaceApi *intf, MicroReq_t * req, MicroResp_t * rsp, BYTE broadcast )
{

    BYTE        RetryCount, respLen, i;

    BmcResp_t   bResp;
    char*		Status = NULL;
   
    struct BmcSendReq
    {
        BYTE    dest;       // bus/channel
        BYTE    data[2];    // broadcast reqs build IPMB req at data[1]
        
    }* bsReq = (struct BmcSendReq *) LastReq.bmcReq.data;

     Status          = NULL; 	

	scrollok(stdscr,TRUE);
    printw("SendRequest: intf 0x%x, req->ipmbAddr %xh, req->length 0x%x\n",
        (u32*) intf, req->ipmbAddr, req->length);
	scrollok(stdscr,FALSE);
    
    if ( req->ipmbAddr == intf->bmcAddr( intf ) )
    {
        //if( Debug )
        //    printwMicroReq( L"Send Req", req );
        
      return intf->sendRequest(intf, & req->bmcReq, req->length);
    }
    //
    // This is to a different controller. Send it through the BMC
    //
    FlushSMSBuffer( intf);

    //
    // build the IPMB message in the last request packet structure
    //
    
    LastReq.ipmbAddr        = intf->bmcAddr( intf );

            LastReq.bmcReq.netFnLn  = APP_REQUEST;

            if ( gIpmiVersion == 0x09 )
            {
                LastReq.bmcReq.cmd  = MASTER_WRITE_I2C;
                bsReq->dest         = PUBLIC_I2C_BUS;
            }
            else
            {
                LastReq.bmcReq.cmd  = SEND_MESSAGE;
                bsReq->dest         = IPMB_CHANNEL_ID;
            }
    if( broadcast )
    {
        LastReq.length = (UINT8)BuildIpmbReq( (IpmbMsg_t *) & bsReq->data[1], req ) + 1;
        bsReq->data[0] = 0;
    }
    else
    {
        LastReq.length =(UINT8) BuildIpmbReq( (IpmbMsg_t *) & bsReq->data[0], req );
    }
    LastReq.length++;   // account for bus/channel byte

    //if( Debug )
    //    printwMicroReq( L"SendMsg Req", &LastReq );
    //
    // Now send it to the BMC, collecting the response to the SendMessage cmd.
    //
    RetryCount = 0;

    do
    {
        Status = intf->sendRequest( intf, & LastReq.bmcReq, (int)(LastReq.length)) ;
         if (Status) return Status;      
    
        if( (respLen = (UINT8)intf->readResponse( intf, &bResp)) < MIN_BMC_RESP_SIZE )
            return formatMessage ("received < MIN_BMC_RESP_SIZE");          

        //if( Debug )
        //    printwBuffer( L"SendMsg Resp", (byte *) & bResp, respLen );

    } 

    while ( IsI2CRetryNeeded( bResp.cCode ) && ++RetryCount < 3 );

    if ( RetryCount >= 3 && bResp.cCode != COMPCODE_NORMAL )
        return formatMessage ("ERROR on I2C access, tried three times.\n");

    rsp->respLen  = respLen-3;
    for(i=0; i<sizeof(BmcResp_t); i++)
        *(byte *)((byte *)&(rsp->bmcResp) + i) = *(byte *)((byte *)&bResp + i);
    
    return Status;
}


char*  SendRequestToMicro( MicroReq_t * req, MicroResp_t * resp )
{
    return SendRequest( Interface, req, resp, 0 /* no broadcast */ );
}



/*******************************************************************************
*  Routine: wait_for_SMS_message_ready
*
*  Arguments:
*     interface to check
*
*  Returns:
*     TRUE - if message ready
*     FALSE - if timed out with no message ready
*
*  Description:
*     Waits for the interface to signal that SMS message is ready to be read
*
*******************************************************************************/

static BOOLEAN wait_for_SMS_message_ready( InterfaceApi *intf )
{
   clock_t overallTimeout;  

   SetMsTimeout ( & overallTimeout, SMS_TIMEOUT );

   gIPMBRespTimeInMS = 0;

  
   while ( ! intf->smsReady( intf ) )
   {
      //BS->Stall(1);
      gIPMBRespTimeInMS++;
      
      if ( CheckMsTimeout(overallTimeout) ) 
      {
         return FALSE; /* timed out with no message ready */
      }              
   }

   
   return TRUE; /* message ready */
} 

/*******************************************************************************
*  Routine: ResendLastRequest
*
*  Arguments:
*     N/A
*
*  Returns:
*     0 success, -1 failure
*
*  Description:
*     This routine resends the last request packet.
*     
*******************************************************************************/

static char*  ResendLastRequest( InterfaceApi *intf)
{
    BYTE        RetryCount;
    BmcResp_t   bResp;
    byte        respLength;
    char*		Status = NULL; 
    if( Debug )
        PrintMicroReq( "ReSend Req", & LastReq );

    RetryCount = 0;

    do
    {
        Status = intf->sendRequest( intf, & LastReq.bmcReq, LastReq.length) ;
        if (Status) return Status;      

        if( (respLength =(UINT8) intf->readResponse( intf, & bResp)) < MIN_BMC_RESP_SIZE )
            return formatMessage ("received < MIN_BMC_RESP_SIZE");

        if( Debug )
            PrintBuffer( "ReSend Resp", (byte *) & bResp, respLength );

    } while ( IsI2CRetryNeeded( bResp.cCode ) && ++RetryCount < 3 );

    if ( RetryCount >= 3 )
        return formatMessage ("ERROR on I2C access, tried three times.\n");
    return Status;
}


/*******************************************************************************
*  Routine: ReadSmsBuffer
*
*  Arguments:
*     interface handle
*     response structure
*
*  Returns:
*     length of response data, not including the SA, netFnLn, cmd, or cCode
*
*  Description:
*     This routine reads the Sms buffer and parses the IPMB response.
*     
*******************************************************************************/

int ReadSmsBuffer ( InterfaceApi *intf, MicroResp_t * response )
{
    BmcReq_t    bReq;
    BmcResp_t   bResp;
    byte        reqLength;
    int         respLength;
    BYTE        RetryCount;
    IpmbMsg_t * iRsp;

    RetryCount = 0;

    do
    {
        if( wait_for_SMS_message_ready( intf ) == TRUE )
            break;

        RetryCount++;
        gSMSTOCount++;
	scrollok(stdscr,TRUE);
        printw("SMS_MsgRdy_TO%d ", RetryCount);
	scrollok(stdscr,FALSE);

        ResendLastRequest( intf);

    } while( RetryCount < 3);

    if ( RetryCount >= 3 )
    {
        response->ipmbAddr = intf->bmcAddr( intf );
	scrollok(stdscr,TRUE);
        printw("Error - SMS MESSAGE READY timeout, tried three times.\n" );
	scrollok(stdscr,FALSE);
        return -1;
    }      
    //
    // There seems to be a response for us. Collect it
    //

    bReq.netFnLn = APP_REQUEST;
            
    bReq.cmd = ( gIpmiVersion == 0x09 ) ? READ_SMS_MESSAGE_BUFFER : GET_MESSAGE;

    reqLength = 2;

    if( Debug )
        PrintBmcReq( "ReadSmsReq", & bReq, reqLength );

    if( intf->sendRequest( intf, &bReq, reqLength) != 0 )
    {
        response->ipmbAddr = intf->bmcAddr(intf);
	scrollok(stdscr,TRUE);
        printw("Error requesting SMS message containing response.\n");
	scrollok(stdscr,FALSE);
        return -1;
    }
    
    if( (respLength = intf->readResponse( intf, &bResp)) < MIN_BMC_RESP_SIZE )
    {
        response->ipmbAddr = intf->bmcAddr(intf);
	scrollok(stdscr,TRUE);
        printw("Error getting response to Get Message.(%d)\n", respLength );
	scrollok(stdscr,FALSE);
        return -1;
    }

    if( Debug )
        PrintBuffer( "ReadSmsResp", (byte *) & bResp, respLength );
    //
    // Make sure we got the response from the BMC ok
    //
    if( bResp.cCode != COMPCODE_NORMAL )
    {
        //
        // bad completion code, just copy the response and bail
        //
        response->ipmbAddr  = intf->bmcAddr(intf);
        response->respLen   =(UINT8) (respLength - MIN_BMC_RESP_SIZE);

        memcpy( & response->bmcResp, & bResp, respLength );

        return 0;
    }
    //
    // good response, unpack the message
    //
    if ( gIpmiVersion == 0x09 )
    {
        //
        // overlay the (missing) SA on the cCode
        //
        iRsp = (IpmbMsg_t *) & bResp.cCode;
    }
    else
    {
        //
        // overlay the (missing) SA on the channel number
        //
        iRsp = (IpmbMsg_t *) bResp.data;
    }

    if( SEQ_OF(iRsp->seqRqLn) != mPrevSeqNum )
    {
	scrollok(stdscr,TRUE);
        printw("ERROR: IPMB resp. packet's seq#  does not match req. packet seq#.\n");
	scrollok(stdscr,FALSE);
        return -1;
    }
    response->ipmbAddr          = iRsp->rqSa;
    response->bmcResp.netFnLn   = iRsp->netFnRsLn;
    response->bmcResp.cmd       = iRsp->cmd;
    response->bmcResp.cCode     = iRsp->data[0];
    //
    // subtract off the bmc rsp header (2), bmc cCode, channel no
    // nfln, cSum1, rqSa, seq, cmd, ctlr cCode, and csum2
    //
    respLength -= 11;

    memcpy( response->bmcResp.data, & iRsp->data[1], respLength );

    response->respLen =(UINT8) respLength;

    if( Debug )
        PrintBuffer( "MicroResp", (BYTE *) response, respLength + 4 );

    return 0;   
}



/*******************************************************************************
*  Routine: ReadResponse
*
*  Arguments:
*     Interface handle
*     response structure pointer
*
*  Returns:
*     length of response data, not including the SA, netFnLn, cmd, or cCode
*
*  Description:
*     This routine receives responses from the BMC or micros on the IPMB.
*     
*******************************************************************************/

int ReadResponse( InterfaceApi *intf, MicroResp_t *resp )
{
    int respLen;

    if ( resp->ipmbAddr == intf->bmcAddr(intf) )
    {
        respLen = intf->readResponse( intf, & resp->bmcResp);

        if( Debug )
        {
            if( respLen < 0 ) {
				scrollok(stdscr,TRUE);
                printw( "Resp: error %d\n");
				scrollok(stdscr,FALSE);
			}
            else
                PrintBuffer( "Resp", (byte *) & resp->bmcResp, respLen );
        }
        if( respLen < MIN_BMC_RESP_SIZE )
            return -1;

        resp->respLen = (byte)(respLen - MIN_BMC_RESP_SIZE);
        
        return 0;
    }
    //
    // This was to a micro other than the BMC. Get the response
    //
    if (intf->smsReady != NULL)
        return ReadSmsBuffer( intf, resp );
    else
        // SMS interface not supported
        return 0;
}

//----------------------------------------------------------------------------

int ReadResponseFromMicro( MicroResp_t *response )
{
    return ReadResponse( Interface, response );
}

static IO_LOG ioLogArray [10000];
static int ioLogIndex;

//------------------------------------------------------------------------------

void dumpIoLog (void)
{
	int index;

   	if (!ioLogIndex) return;
	scrollok(stdscr,TRUE);
   	for (index = 0; index < ioLogIndex; index++)
    	printw("%c %04x %02x\n","RW" [ioLogArray [index].type], 
      	ioLogArray [index].address, ioLogArray [index].data);
   	printw("\n");
	scrollok(stdscr,FALSE);
   	ioLogIndex = 0;
}


/*******************************************************************************
*  Routine: main
*
*  Arguments:
*     argc - argument count
*     argv - array of arguments
*
*  Returns:
*     Errorlevel of 0 to O/S if all went well
*     Nonzero errorlevel otherwise
*
*  Description:
*     This is the entry point for the utility to download an SDR file to the BMC
*
*******************************************************************************/

char* ipmiTool (int argc, char  *argv[])
{
	int  			i;
   	int          	printTime = 0;
   	time_t       	startTime;
   	char         	CmdBuffer[100];
   	int          	temp;
   	char         	ResponseBuffer[256];
   	char*       	ifName="isa_sms";
   	MicroReq_t*    	cmd = (MicroReq_t *) CmdBuffer;
   	MicroResp_t*   	resp = (MicroResp_t *) ResponseBuffer;
   	char*			Status =NULL;

   	argv += 2;
   	argc -= 2;


   	//
   	// require at least SA, netfnLun, cmd
   	//
   	if ( argc < 3 ) return ExitWithUsage();

   	//
   	// parse our switches
   	//
   	while ( *argv[0] == '-' )
	{
    	//
    	// check for debug option
      	//
      	if ( strcmp( *argv, "-d" ) == 0 )
		{
        	Debug = 1;
         	argv++;
         	argc--;
        }
      	//
      	// see if he wants to print the command execution time
      	//
      	else if ( strcmp( *argv, "-t" ) == 0 )
		{
        	printTime = 1;
         	argv++;
         	argc--;
        }
      	//
      	// see if he wants to print the command execution time
      	//
      	else if ( strcmp( *argv, "-i9" ) == 0 )
		{
        	SetIPMIversion( IPMI_0_9 );
        	argv++;
         	argc--;
        }
      	//
      	// check for requested interface
      	//
      	else if ( strcmp( *argv, "-if" ) == 0 )
		{
        	if( (ifName = *++argv) == NULL ) return ExitWithUsage();

         	argv++;
         	argc -= 2;
         }
	}


  	if ( FindInterface( ifName ) == FALSE ) return formatMessage ("Can't find an interface to use\n");
   	
	memset((void *)CmdBuffer,(int)0x5a,100);
	Status = hextoint64 (*argv++, (u64*)&temp, maxAddressDigits);  if (Status) return ExitWithUsage();
   	//(char )cmd->ipmbAddr = (char)(temp & 0xFF);
	
	//rollok(stdscr,TRUE);
    //intw("WesonTest: temp:0x%x ipmbAddr:0x%x,test:0x%x\n",temp,cmd->ipmbAddr,(char)(temp & 0xFF));

		
    //intw("WesonTest: addr1:0x%x addr2:0x%x\n",CmdBuffer,cmd);
	//rollok(stdscr,FALSE);

   	//
   	// collect netfnLun, cmd, and data
   	//
   	//

#if 0
   	for ( i = 0; i + 1< argc; i++)
	{
		Status = hextoint64 (*argv++, (u64*)&temp, maxAddressDigits);  if (Status) return ExitWithUsage();
	scrollok(stdscr,TRUE);
    printw("WesonTest: temp:0x%x ipmbAddr:0x%x\n",temp,cmd->ipmbAddr);
	scrollok(stdscr,FALSE);

      	((char *)&cmd->bmcReq)[i] = (char)(temp & 0xFF);
	}
   	cmd->length =(char)( i - 2);    // adjust for the netfn and cmd bytes
#endif

	//scrollok(stdscr,TRUE);
    //printw("WesonTest: cmd->ipmbAddr:0x%x cmd->length:0x%x\n",cmd->ipmbAddr,cmd->length);
	//scrollok(stdscr,FALSE);

	//char *p = &CmdBuffer[0];
	for (i=0; i<10; i++ )
	{
	scrollok(stdscr,TRUE);
    printw("cmd%02d:0x%x\n",i,(u8)CmdBuffer[i]);
	scrollok(stdscr,FALSE);
	}

	return NULL;

   	//
   	// Send the command to the micro
   	//
   	startTime = mstime();

   	if( SendRequestToMicro( cmd, resp ) != 0 )
    	return formatMessage ("Error sending command to BMC.\n");
   	resp->ipmbAddr = cmd->ipmbAddr;


   	if( ReadResponseFromMicro( resp ) )
    	return formatMessage ("Error reading from BMC\n");

   	//
   	// print out the buffer, skipping the first byte (length)
   	// there are 5 bytes not accounted for by the respLen:
   	//   respLen, ipmbAddr, nfLun, cmd, cCode
   	//
   	ipmi_hexAsciiDump (ResponseBuffer + 1, 0, resp->respLen + 4);
   	if( printTime ) {
		scrollok(stdscr,TRUE);
		printw( "%ld ms\n", mstime() - startTime );
		scrollok(stdscr,FALSE);
	}

   	dumpIoLog ();
	
	if( Interface ) {
		free(Interface);
		Interface = NULL;
	}
   	return Status; // exit( resp->bmcResp.cCode );
}

//----------------------------------------------------------------------------
char*  ipmi (void *message, int txcount, void *response, int rxlimit, int *rxcount)
{
	int             ipmiError, index;
   	char            CmdBuffer[256];
   	char            ResponseBuffer[256];
   	unsigned char   *position;
   	unsigned char   *txdata = (void *) message;
   	unsigned char   *rxdata = (void *) response;
   	MicroReq_t*     cmd = (MicroReq_t *) CmdBuffer;
   	MicroResp_t*    resp = (MicroResp_t *) ResponseBuffer;
   	char*		    Status = NULL;

   
   	position = (void *) &cmd->bmcReq;
   	cmd->ipmbAddr = txdata [0];
   	for (index = 1; index < txcount; index++) position [index - 1] = txdata [index];
   	cmd->length = (char)(txcount - 3);    // adjust for the netfn and cmd bytes
   	FindInterface ("isa_sms");
   	Status = SendRequestToMicro (cmd, resp);
   	if (Status) return Status;
   	resp->ipmbAddr = cmd->ipmbAddr;
   	ipmiError = ReadResponseFromMicro (resp);
   	if (ipmiError) return formatMessage ("Error reading from BMC\n");
   	*rxcount = 0;
   	for (index = 1; index < resp->respLen + 5; index++)
	{
		//if (*rxcount == rxlimit) return formatMessage ("rx buffer too small, limit=%u length=%u", rxlimit, resp->respLen + 4);
		if (*rxcount == rxlimit) return formatMessage ("rx buffer too small\n");
		rxdata [(*rxcount)++] = ResponseBuffer [index];
	}

   	return Status;
}
