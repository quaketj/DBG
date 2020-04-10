#include "project.h"

int get_cur_time()
{
	return 10;
}

#define	queryPerformanceCounter() 	get_cur_time()
#define	queryPerformanceFrequency() get_cur_time()

//----------------------------------------------------------------------------

typedef struct
{
   u32  ioBase;
   u32  controller;
   u16* southBridgeName [40];
}
SMBUS_INFO;

//-----------------------------------------------------------------------------
typedef struct 
{
	u32 bus,dev,func;
	u16	vendorid;
}
PCIaddress;


enum 
{
   PCI_UNKNOWN ,   PCI_ICH ,
   PCI_AMD    
};

//-----------------------------------------------------------------------------

#pragma pack (push,1)
typedef struct
   {
   u16   vendorid;
   u16   deviceid;
   }
PCI_CFG_HEADER;
#pragma pack (pop)

//void MultiCharToUnicode (char *bBuffer, unsigned short* wBuffer)
void MultiCharToUnicode (char *bBuffer, char* wBuffer)
{
	char  loop;
	for (loop = 0; loop<strlen(bBuffer); loop++) {
      		wBuffer[loop] = bBuffer[loop];
  	 }
	wBuffer[loop]= '\0';

}

u32 Xtoi (unsigned char  *str )
{
  u32   u;
  unsigned char  c;
  u32   m;

  if(str == NULL) return (u32)-1;
  
  
  m = (u32) -1 >> 4;
  while (*str && *str == ' ') {
    str += 1;
  }
  while (*str && *str == '0') {
    str += 1;
  }
  if (*str && (*str == 'x' || *str == 'X')) {
    str += 1;
  }
  u = 0;
  c = *(str++);
  while (c) {
    if (c >= 'a' && c <= 'f') {
      c -= 'a' - 'A';
    }

    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
      if (u > m) {
        return (u32) -1;
      }

      u = u << 4 | c - (c >= 'A' ? 'A' - 10 : '0');
    } else {
      break;
    }

    c = *(str++);
  }

  return u;
}


//----------------------------------------------------------------------------
//
// command line help goes here
//
char* smbHelpScreen (void)
{
	scrollok(stdscr,TRUE);
	printw ("use smb options (all byte values are hexadecimal)\n\n");
   	printw ("options: tx=a,o,x,x... send one or more bytes to adress a, offset o\n");
   	printw ("         rx=a,c        read from address a, offset zero, c bytes\n");
   	printw ("         rx=a,o,c      read from address a, offset o, c bytes\n");
   	printw ("         delay=n       delay n us after writing each command\n");
   	printw ("         verbose       print controller found, and I/O base\n");

	scrollok(stdscr,FALSE);
  	return NULL;
}

//-----------------------------------------------------------------------------

#define BASE_SUB_CLASS_CODE		(0x0C05)
char*  findPCI (PCIaddress *address, u32 *found)
{
	int  bytesReturned;
   	PCI_CFG_HEADER header;
   	char*  error=NULL;
    u32 data;

   	*found = 0;
   	for (address->bus = 0; address->bus < 1; (address->bus)++){
    	for (address->dev = 0; address->dev < 32; (address->dev)++){
        	for (address->func = 0; address->func < 8; (address->func)++){
            	error = readPciConfig (address->bus, address->dev, address->func, 0, 4, &bytesReturned, &data);
             	if (error) return error;
            	*(u32*)&header = data;
            	if (header.vendorid == 0xFFFF) continue;
	    		//if(header.vendorid == 0x8086)
	    		//{
				//	//enable SMBUS controller 
	    		//}

            	error = readPciConfig (address->bus, address->dev, address->func, 0x0a, 2, &bytesReturned, &data);
             	if (error) return error;
            	if (BASE_SUB_CLASS_CODE == (u16)data) 
               	{
               		address->vendorid = header.vendorid;
               		*found = 1;
					scrollok(stdscr,TRUE);
					printw("Found SMBUS Controller in Bus = 00, Dev = %02x, Func = %02x \n", address->dev, address->func);
					scrollok(stdscr,FALSE);
               		return error;
            	}
        	}
    	}
    }
	return error;
}

//-----------------------------------------------------------------------------
// look for a supported chipset

#define	SMBUS_BASE_ADDR_OFFSET	(0x20)
char*  inspectSMB (SMBUS_INFO *info)
{
	PCIaddress SBaddress = {0};
  	u32	found,bytesdReturned,data;
   	char*  error=NULL;
 
   	error = findPCI (&SBaddress,&found);  
   	if (error) return error;
  
   	if (found){
	  	if(SBaddress.vendorid == 0x8086){
	  		info->controller = PCI_ICH;
			strcpy ((char *)info->southBridgeName, (char *)"INTEL");
	  	}
	 	
		if(SBaddress.vendorid == 0x1002)
		{
	  		info->controller = PCI_AMD;
			strcpy ((char *)info->southBridgeName, (char *)"AMD");
		}
		error = readPciConfig (SBaddress.bus,SBaddress.dev,SBaddress.func,SMBUS_BASE_ADDR_OFFSET,4,&bytesdReturned,&data);
   		if (error) return error;
        info->ioBase = data & 0xFFF0;
   	}
	return error;
}




//----------------------------------------------------------------------------
//
// look for a supported SMBus controller
//
char* initialize (SMBUS_INFO *info, int verbose)
{
   	char*  error =NULL;
   
   	error = inspectSMB (info);
   	if (!info->ioBase) return formatMessage ("no supported interface found"); 
	scrollok(stdscr,TRUE);
    printw ("found %s, I/O base %xh\n", info->southBridgeName, info->ioBase);
	scrollok(stdscr,FALSE);
   	return error;
}



//-----------------------------------------------------------------------------
// SMBUS controller routines, read or write a single byte only
//-----------------------------------------------------------------------------

//char* ich_block (int iobase, int address, int offset, char* data, int delay)
char* ich_block (u32 iobase, int address, int offset, char* data, int delay)
{
    u32 nStatus, read = address & 1;
    u64 limit;
    u32 hi_byte = offset & 1;
    u8   hi_data, low_data;
    char*  error =NULL;
	u64 i;


//scrollok(stdscr,TRUE);
	//printw("iobase:0x%x,addr:0x%x,offset:0x%x\n",iobase,address,offset);

// firstly read word data   
    WriteIoPort (iobase + 0, 8, 0xFF);               // clear error status
    //WriteIoPort (iobase + 3, 8, offset/2);             // offset in eeprom
    WriteIoPort (iobase + 3, 8, (u8)offset);             // offset in eeprom
    WriteIoPort (iobase + 4, 8, (address & 0xFE + 1) );            // shifted slave address (caller sets read bit)
    WriteIoPort (iobase + 5, 8,0xFF);   // data, for writes only 
    WriteIoPort (iobase + 6, 8, 0xFF);   // data, for writes only 
    //WriteIoPort (iobase + 2, 8, 0x4c);               // start byte read or write
    WriteIoPort (iobase + 2, 8, 0x48);               // start byte read or write

#if 0 /* debug by Wenson */
   if (delay< 1000) BS->Stall(1);                      // give extra time to complete
   else
   	BS->Stall(delay /1000);
   // wait up to 100 ms for done status
   //limit = queryPerformanceCounter () + queryPerformanceFrequency ();
#endif
	//usleep(10000);
	//for (;;) {
	for (i=0;i<1000;i++) {
    	//nStatus = ReadIoPort (iobase, 8) & ~0x40;
    	nStatus = ReadIoPort (iobase, 8) & ~0x40;
      	//if (queryPerformanceCounter () > limit) break;
		usleep(10000);
      	if ((nStatus) == 2) break;            // interrupt status only
	}


    low_data = (u8)ReadIoPort (iobase + 5, 8);
    hi_data = (u8)ReadIoPort (iobase + 6, 8);


//printw("0x%x,0x%x,0x%x\n",nStatus,hi_data,low_data);
//scrollok(stdscr,FALSE);

    if(read)
  	{
  		//if(hi_byte) *data = hi_data;
		//else *data = low_data;
		*data = low_data;

		return NULL;
  	}
	
    WriteIoPort (iobase + 0, 8, 0xFF);               // clear error status
    //WriteIoPort (iobase + 3, 8, offset/2);             // offset in eeprom
    WriteIoPort (iobase + 3, 8, (u8)offset);             // offset in eeprom
    WriteIoPort (iobase + 4, 8, address );            // shifted slave address (caller sets read bit)

#if 0
    if( hi_byte)
   	{
   		WriteIoPort (iobase + 5, 8, low_data);   // data, for writes only 
   		WriteIoPort (iobase + 6, 8, *data);   // data, for writes only 
   	}
    else
   	{
   		WriteIoPort (iobase + 5, 8,*data);   // data, for writes only 
   		WriteIoPort (iobase + 6, 8, hi_data);   // data, for writes only 
   	}
   	WriteIoPort (iobase + 2, 8, 0x4C);               // start byte read or write
#endif
   	WriteIoPort (iobase + 5, 8, *data);   // data, for writes only 
   	WriteIoPort (iobase + 2, 8, 0x48);               // start byte read or write

#if 0 /* debug by Wenson */ 
   if (delay< 1000) BS->Stall(1);                      // give extra time to complete
   else
   	BS->Stall(delay /1000);
   // wait up to 100 ms for done status
   limit = queryPerformanceCounter () + queryPerformanceFrequency ();
#endif
   	//usleep(10000);
	for (i=0;i<1000;i++) {
   	//for (;;){
    	nStatus = ReadIoPort (iobase, 8) & ~0x40;
      	//if (queryPerformanceCounter () > limit) break;
      	usleep(10000);
      	if ((nStatus) == 2) break;            // interrupt status only
	}

scrollok(stdscr,TRUE);
printw("0x%x\n",nStatus);
scrollok(stdscr,FALSE);
    return NULL;
}


//-----------------------------------------------------------------------------
char*  readSmbusNative (SMBUS_INFO *info, int address, int offset, int *count, int delay, char* buffer)
{
	int index;
	char data;
	char*  error =NULL;
   	// set read bit in address
   	address |= 1;

	//printw("addr:0x%x\n",address);
	//printw("offset:0x%x\n",offset);
	//printw("count:0x%x\n",*count);
	//return error;

	
	for (index = 0; index < *count; index++)
	{
		if ( info->controller == PCI_ICH || info->controller == PCI_AMD ) {
        	error = ich_block (info->ioBase, address, offset + index, &data, delay);
		}
      	else return formatMessage ("no native SMBUS read code");

      	if (error)  break;
      	buffer[index]=data;
	}

   	*count = index;
   	return error;
}
//-----------------------------------------------------------------------------
char*   writeSmbusNative (SMBUS_INFO *info, int address, int offset, int *count, int delay, char* buffer)
{
	int index;
   	char*  error=NULL;

   	// clear read bit in address
   	address &= ~1;
   	for (index = 0; index < *count; index++) {
    	if ( info->controller == PCI_ICH || info->controller == PCI_AMD )
        	error = ich_block (info->ioBase, address, offset + index, &buffer [index], delay);
      	else return formatMessage ("no native SMBUS read code");
      
     	if (error)  break;
    }

   	*count = index;
   	return error;
}

//-----------------------------------------------------------------------------
// read one or more bytes from SMBUS device

char* smbRead (SMBUS_INFO *info, char *buffer, int offset, int *count, int address, int delay)
{
   return readSmbusNative (info, address, offset, count, delay, buffer);
}

//----------------------------------------------------------------------------
// write one or more bytes to SMBUS device
char* smbWrite (SMBUS_INFO *info, char* buffer, int offset, int *count, int address, int delay)
{
	return writeSmbusNative (info, address, offset, count, delay, buffer);
}



//-----------------------------------------------------------------------------
char* smbus (int argc, char* argv [])
{
	char*  error=NULL;
   	int arg;
   	int verbose = 0, delay = -1;
   	SMBUS_INFO info = {0};
   	char w_buffer[64];
	char buffer[256];


   	if (argc == 2) return smbHelpScreen ();

	for (arg = 2; arg < argc; arg++) {
    	char* position = argv[arg];
	
      	if (strcmp (position, "verbose") == 0) {
         	verbose++;
		}
      	else if (strncmp (position, "delay=", 6) == 0)
			error = hextoint64 (position + 6, (u64*)&delay, maxAddressDigits); if (error) return error;
      	else continue;
	}

	for (arg = 2; arg < argc; arg++){
    	char* position = argv [arg];
    	if (*position == '\0') continue; // processed on first pass
      	else if (strncmp (position, "rx=", 3) == 0) 
		{
        	char* rxbuffer;
         	u32  index,count, offset = 0, expected;
         	u32  address;
         	//u16	 buffer [256];

//scrollok(stdscr,TRUE);
			memset (buffer,0xFF,sizeof(buffer) );
		 	MultiCharToUnicode (position + 3,w_buffer);  
			
		 	address = (u32)Xtoi ((unsigned char *)w_buffer);
         	address |= 1; // force read/write to read

         	position = strchr (position, ',');
         	if (!position) return formatMessage ("expected  ','");
		 	MultiCharToUnicode (position + 1, (char *)w_buffer);  
		 	count = (u32)Xtoi ((unsigned char *)w_buffer);
         	position = strchr (position + 1, ',');
		 
         	if (position) {
            	offset = count;
			 	MultiCharToUnicode (position + 1,w_buffer);   
			 	count = (u32)Xtoi ((unsigned char *)w_buffer);
            }
			
			rxbuffer	= malloc(count);
			if(!rxbuffer) {
				scrollok(stdscr,TRUE);
				printw ("Not Enough memory to malloc\n");
				scrollok(stdscr,FALSE);
				return error;
			}

         	expected = count;
         	error = initialize (&info, verbose); if (error) return error;	
         	if (delay == -1) delay = 0; // default delay for read

//printw("addr:%0x\n",address); 
//printw("offset:%0x\n",offset); 
//printw("count:%d\n",count); 
//scrollok(stdscr,FALSE);
//return 0;

         	error = smbRead (&info, rxbuffer, offset, &count, address, delay);
         	if (!error) {
				scrollok(stdscr,TRUE);
				printw ("reading byte %x of %x\n", count + 1, expected);
				scrollok(stdscr,FALSE);
			}
         	if (count) { 
				scrollok(stdscr,TRUE);
				printw("received:\n");
				scrollok(stdscr,FALSE);
			}

			//scrollok(stdscr,TRUE);
			//printw("count:0x%x expected:0x%x\n",count,expected);
			//for(delay=0;delay<count;delay++) {
			//	printw("0x%02x ",rxbuffer[delay]);
			//}
			//scrollok(stdscr,FALSE);
			for (index = 0; index < expected; index++)
            {
            	char data = rxbuffer[index];

				scrollok(stdscr,TRUE);
            	printw ("%02x ", (u8)data);
				if(((index+1)%16 ) == 0) printw("\n");
				scrollok(stdscr,FALSE);
            }
			scrollok(stdscr,TRUE);
            printw ("\n");
			scrollok(stdscr,FALSE);

        	free (rxbuffer);
        } else if (strncmp (position, "tx=", 3) == 0) {
         	u32  address, count = 0, offset, expected;
         	position += 3; // skip "tx="
		 	MultiCharToUnicode (position,w_buffer);   
		 	address = (u32)Xtoi ((unsigned char *)w_buffer);
         	address &= ~1; // force read/write to write

         	position = strchr (position, ',');
         	if (!position) return formatMessage ("expected ','");

		 	MultiCharToUnicode (position + 1,w_buffer);   
		 	offset = (u32)Xtoi ((unsigned char *)w_buffer);
         	position = strchr (position + 1, ',');
	 		memset(buffer,' ',sizeof (buffer));
         	while (position)
            {
           		char temp;
		    	MultiCharToUnicode (position + 1,w_buffer);   
				temp = (u32)Xtoi ((unsigned char *)w_buffer);
            	buffer [count++] = (u8) temp;
            	position = strchr(position + 1, ',');
            }

         	expected = count;
         	initialize (&info, verbose);
         	if (delay == -1) delay = 10000; // default delay for read
         	error = smbWrite (&info, buffer, offset, &count, address, delay);
          	if (error) {
				scrollok(stdscr,TRUE);
				printw("writing byte %x of %x\n", count + 1, expected);
				scrollok(stdscr,FALSE);
			} else {
				scrollok(stdscr,TRUE);
				printw("writing byte %x of %x error\n", count + 1, expected);
				scrollok(stdscr,FALSE);
			}
      	}
      	else {
			scrollok(stdscr,TRUE);
        	printw("ERROR: invalid command line argument: %s\n", position);
			scrollok(stdscr,FALSE);
			return error;
        	//return formatMessage ("ERROR: invalid command line argument: %s", position);
		}
   	}

	// if only argument is verbose, display the controllwer type
   	if (argc == 2 && verbose) {
      initialize (&info, verbose);
	}
   	return error;
}








