//=====================================================
//  dbug - a debugging utility for Linux 
//         by Quake Wang
//
//========================================================
#include <curses.h>
#include "project.h"

static void startupBanner (void)
{
	printw ("RAMAXEL debug utility 1.00 for Linux by Quake Wang \n");
}


static void helpScreen (void)
{
	scrollok(stdscr,TRUE);
	printw ("d[b|w|d|q] [range]           dump memory, b|w|d|q defaults to last used\n");
	printw ("e[b|w|d|q] address x x x...  enter one or more values into memory\n");
	printw ("f[b|w|d|q] [range] x x x...  fill memory with pattern\n");
	printw ("s range x x x...             search memory for pattern of one or more bytes\n");
	printw ("sn range x x x...            search memory for non-occurrance of pattern\n");
	printw ("write <file> [range]         write memory to file\n");
	printw ("u[16|32|64] [range]          unassemble memory, 16|32|64 defaults to last used\n");
	printw ("uf<16|32|64> file <start>    unassemble file, start is INITIAL display address\n");
	printw ("cache [wb|uc]                set or display cache mode for memory accesses\n");
	printw ("i[b|w|d] port                input from I/O port, b|w|d defaults to last used\n");
	printw ("o[b|w|d] port value          output to I/O port, b|w|d defaults to last used\n");
	printw ("dpci[b|w|d] b d f [range]    dump PCI config for bus, device, function offset\n");
	printw ("dpci[b|w|d]                  dump PCI config space for all devices found\n");
	printw ("epci[b|w|d] b d f o x x x... write PCI config space for bus, dev, func, offset\n");
	printw ("pcia b d f                   display addressing info for PCI bus dev func\n");
	printw ("cr[0|2|3|4|8] [value]        display or set control register\n");
	printw ("msr reg [value]              display or set model specific register\n");
	printw ("mtrr                         dump MTRRs\n");
	printw ("mtrr v start size tt         setup MTRR variable range v, tt=UC|WC|WT|WP|WB\n");
	printw ("dumpmp                       dump mp table\n");
	//printw ("dumpmp                       dump mp table\n\n");
	//printw ("<Press any key to continue>\n");
	//getch();
	//printw ("\nnuma                         display Windows NUMA summary\n");
	printw ("numa                         display Windows NUMA summary\n");
	printw ("cmos                         dump cmos\n");
	printw ("ec address x x x...          enter one or more values into cmos at address\n");
	printw ("fc [range] value             fill cmos with pattern\n");
	printw ("clearcmos                    clear cmos (except for time & date)\n");
	printw ("cpuid                        dump cpuid raw data\n");
	printw ("srat                         dump ACPI SRAT table\n");
	printw ("cpuspeed                     measure cpu speed\n");
	printw ("flash                        commands for programming flash bios\n");
	printw ("e820                         dump BIOS e820 memory map\n");
	printw ("ioapic <address>             dump I/O apic\n");
	printw ("patchinfo                    display processor patch details\n");
	printw ("ipmitool                     IPMI commands\n");
	printw ("frudump, sdrdump, clearsel   IPMI dump utility commands\n");
	printw ("ipmi                         IPMI utility commands\n");
	printw ("smbus                        SMBUS read and write\n");
	printw ("pir                          dump the $PIR (Pci IRQ Routing Table\n");
	printw ("q                            quit\n");
	printw ("\n");
	scrollok(stdscr,FALSE);
}


static void breakUpCommandLine (char *buffer)
{
	char *position = buffer;

	while (*position)
	{
		if (*position == ';') *position = '\0';
		position++;
	}
}

static void getCommand (char *buffer)
{
	for (;;)
	{
		printw ("-");
		editLine (buffer, MAX_COMMAND_LENGTH);
		if (*buffer) break;
	}
}

void dispalyCommand (int argc, char * argv[])
{
	int index;

	scrollok(stdscr,TRUE);
	printw("-");
	for(index= 1; index <= (argc-1); index++) printw ("%s ", argv[index]);
	printw("\n");
	scrollok(stdscr,FALSE);
}

//static char *processCommand (int argc, char *argv [])
char *processCommand (int argc, char *argv [])
{
	//   static enum CACHE_CONTROL cacheMode = CACHE_CONTROL_wb;
	static u64 nextDumpAddress, nextUnassembleAddress;
	static int lastDumpSize = 8, lastEnterSize = 8, lastFillSize = 8, lastUnassembleSize = 32, lastIoReadSize = 8;
	static int lastIoWriteSize = 8, lastPciSize = 32;
	char	*position = argv [1];
	char    *error = NULL;
	static char buffer [200];
	
	dispalyCommand(argc, argv); // Display user entered command
	if (stricmp (position, "q") == 0 && argc == 2)
	{
      	endwin();
        exit (0);
    }
   	else if (stricmp (position, "s") == 0)
    	return memSearch (argc, argv, 1);
   	else if (stricmp (position, "sn") == 0) 
      	return memSearch (argc, argv, 0);
   	else if (stricmp (position, "e") == 0)
      	return memEnter (argc, argv, lastEnterSize);
   	else if (stricmp (position, "eb") == 0)
    {
    	lastEnterSize = 8;
      	return memEnter (argc, argv,lastEnterSize);
    }
   	else if (stricmp (position, "ew") == 0)
    {
        lastEnterSize = 16;
        return memEnter (argc, argv, lastEnterSize);
    }
   	else if (stricmp (position, "ed") == 0)
    {
        lastEnterSize = 32;
        return memEnter (argc, argv, lastEnterSize);
    }
   	else if (stricmp (position, "eq") == 0)
    {
        lastEnterSize = 64;
        return memEnter (argc, argv, lastEnterSize);
    }
   	else if (stricmp (position, "d") == 0)
        return memDump (argc, argv,  lastDumpSize, &nextDumpAddress);
   	else if (stricmp (position, "db") == 0)
    {
        lastDumpSize = 8;
        return memDump (argc, argv,lastDumpSize, &nextDumpAddress);
    }
   	else if (stricmp (position, "dw") == 0)
    {
        lastDumpSize = 16;
        return memDump (argc, argv, lastDumpSize, &nextDumpAddress);
    }
   	else if (stricmp (position, "dd") == 0)
    {
        lastDumpSize = 32;
        return memDump (argc, argv, lastDumpSize, &nextDumpAddress);
    }
   	else if (stricmp (position, "dq") == 0)
    {
        lastDumpSize = 64;
        return memDump (argc, argv, lastDumpSize, &nextDumpAddress);
    }
   	else if (stricmp (position, "f") == 0)
        return memFill (argc, argv, lastFillSize);
   	else if (stricmp (position, "fb") == 0)
    {
        lastFillSize = 8;
        return memFill (argc, argv,lastFillSize);
    }
   	else if (stricmp (position, "fw") == 0)
    {
        lastFillSize = 16;
        return memFill (argc, argv, lastFillSize);
    }
   	else if (stricmp (position, "fd") == 0)
    {
        lastFillSize = 32;
        return memFill (argc, argv, lastFillSize);
    }
   	else if (stricmp (position, "fq") == 0)
    {
        lastFillSize = 64;
        return memFill (argc, argv, lastFillSize);
    }
   	/*
   	else if (stricmp (position, "uf16") == 0)
      return fileUnassemble (argc, argv, 16);
   	else if (stricmp (position, "uf32") == 0)
      return fileUnassemble (argc, argv, 32);
   	else if (stricmp (position, "uf64") == 0)
      return fileUnassemble (argc, argv, 64);
   	else if (stricmp (position, "u") == 0)
      return memUnassemble (argc, argv, lastUnassembleSize, &nextUnassembleAddress);
   	else if (stricmp (position, "u16") == 0)
      {
      lastUnassembleSize = 16;
      return memUnassemble (argc, argv,lastUnassembleSize, &nextUnassembleAddress);
      }
   	else if (stricmp (position, "u32") == 0)
      {
      lastUnassembleSize = 32;
      return memUnassemble (argc, argv, lastUnassembleSize, &nextUnassembleAddress);
      }
   	else if (stricmp (position, "u64") == 0)
      {
      lastUnassembleSize = 64;
      return memUnassemble (argc, argv, lastUnassembleSize, &nextUnassembleAddress);
      }
*/
   	else if (stricmp (position, "dpci") == 0)
        return pciDump (argc, argv, lastPciSize);
   	else if (stricmp (position, "dpcib") == 0)
    {
        lastPciSize = 8;
        return pciDump (argc, argv, lastPciSize);
    }
   	else if (stricmp (position, "dpciw") == 0)
    {
        lastPciSize = 16;
        return pciDump (argc, argv, lastPciSize);
    }
   	else if (stricmp (position, "dpcid") == 0)
    {
        lastPciSize = 32;
        return pciDump (argc, argv, lastPciSize);
    }
   	else if (stricmp (position, "epci") == 0)
        return pciEnter (argc, argv, lastPciSize);
   	else if (stricmp (position, "epcib") == 0)
    {
        lastPciSize = 8;
        return pciEnter (argc, argv, lastPciSize);
    }
   	else if (stricmp (position, "epciw") == 0)
    {
        lastPciSize = 16;
        return pciEnter (argc, argv, lastPciSize);
    }
   	else if (stricmp (position, "epcid") == 0)
    {
        lastPciSize = 32;
        return pciEnter (argc, argv, lastPciSize);
    }
   	else if (stricmp (position, "pcia") == 0 && argc == 5)
    {
        u64 bus, dev, func;
        __u32 cf8value, offset;
        error = hextoint64 (argv [2], &bus,  2);  if (error) return error;
        error = hextoint64 (argv [3], &dev,  2);  if (error) return error;
        error = hextoint64 (argv [4], &func, 2);  if (error) return error;
        cf8value = (bus << 16) + (dev << 11) + (func << 8);
        cf8value |= 0x80000000;
        offset = (bus << 20) + (dev << 15) + (func << 12);
	    scrollok(stdscr,TRUE);
        printw ("PCI config %02X/%02X/%02X: CF8=%08X MMIO=X%07X\n", (int)bus, (int)dev, (int)func, cf8value, offset);
        scrollok(stdscr,FALSE);
    }
	else if (stricmp (position, "i") == 0)
        return ioRead (argc, argv, lastIoReadSize);
   	else if (stricmp (position, "ib") == 0)
    {
        lastIoReadSize = 8;
        return ioRead (argc, argv, lastIoReadSize);
    }
   	else if (stricmp (position, "iw") == 0)
    {
        lastIoReadSize = 16;
        return ioRead (argc, argv, lastIoReadSize);
    }
   	else if (stricmp (position, "id") == 0)
    {
        lastIoReadSize = 32;
        return ioRead (argc, argv, lastIoReadSize);
    }
   	else if (stricmp (position, "o") == 0)
        return ioWrite (argc, argv, lastIoWriteSize);
   	else if (stricmp (position, "ob") == 0)
    {
        lastIoWriteSize = 8;
        return ioWrite (argc, argv, lastIoWriteSize);
    }
   	else if (stricmp (position, "ow") == 0)
    {
        lastIoWriteSize = 16;
        return ioWrite (argc, argv, lastIoWriteSize);
    }
   	else if (stricmp (position, "od") == 0)
    {
        lastIoWriteSize = 32;
        return ioWrite (argc, argv, lastIoWriteSize);
    }
   	else if (stricmp (position, "cmos") == 0 && argc == 2)
        return dumpCmos ();
   	else if (stricmp (position, "clearcmos") == 0 && argc == 2)
        clearCmos ();
   	else if (stricmp (position, "ec") == 0)
        return cmosEnter (argc, argv);
   	else if (stricmp (position, "fc") == 0)
        return cmosFill (argc, argv);
   	else if (stricmp (position, "flash") == 0)
    	return _flash (argc, argv);
	else if (stricmp (position, "smbus") == 0)
		return smbus(argc,argv);
	else if (stricmp (position, "e820") == 0)
		return e820_analysis(argc,argv);
	else if (stricmp (position, "ioapic") == 0)
		return ioapic(argc,argv);
	else if (stricmp (position, "ipmi") == 0 && argc >= 2)
		//return ipmiTool(argc,argv);
		return 0;
	else if (stricmp (position, "cpuid") == 0 && argc >= 2)
		 return displayCpuid ();	
   	else if (stricmp (position, "?") == 0 || stricmp (position, "help") == 0) 
		helpScreen ();
      	else{ 
		printw("\nunrecognized command: %s\n", argv[1]);
		return buffer;
	}
	return error;
}


void initial() 
{
	initscr();
	cbreak();
	nonl();
//	noecho();
	intrflush(stdscr,FALSE); 
	keypad(stdscr,TRUE); 
//	refresh(); 
//	scrollok(stdscr,TRUE);

}



int main (int argc, char *argv [])
{
	int      index;
	char     *cmdline, *position, *currentCommand;
	iopl(3);

#if 0
	unsigned char *mem_addr = NULL;
	int offset = 0;
	u64 membase = 0;
	u64 physicalAddress;
	int nmemFile;
	unsigned char *virtualAddress;

	unsigned char val;

	physicalAddress = 0x100;

	membase = physicalAddress & 0xfffffffffffff000;
	offset = physicalAddress & 0xfff;
	nmemFile = open("/dev/mem",O_RDWR);
	if( nmemFile !=-1) {
		mem_addr = (unsigned char *)mmap(0,
			0x100000,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			nmemFile,
			membase);
		
		virtualAddress = (void *)(mem_addr + offset);
	} else printf("open /dev/mem fail");

	val = *(unsigned char*)virtualAddress;

	printf("val:%d\n",val);

	return 0;
#endif


	initial();
	
	// setup an empty command buffer
	cmdline = calloc (MAX_COMMAND_LENGTH + 1, 1);
	if (!cmdline) logError ("out of memory");
	
	// command line argument processing
	for (index = 1; index < argc; index++)
	{
		position = argv [index];

		if (*position != '-' && *position != '/') logError ("unexpected option: %s", position);
		position++;

		if (strcmp (position, "?") == 0)
		{
			printw ("\ndebug utility for DOS, Win32, and Win64\n");
			printw ("use: debug              interactive mode\n");
			printw ("     debug /\"commands\"  execute commands in batch mode (; separates)\n");

			refresh();
			sleep(3);
			endwin();
			return 1;
		}

		// batch mode commands on command line
		else if (*position == '\"')
		{
			size_t length = strlen (position + 1);
			if (length > MAX_COMMAND_LENGTH - 2) logError ("command too long");
			strcpy (cmdline, position + 1);

			// remove closing quote if present
			if (cmdline [length - 1] == *position) cmdline [length - 1] = '\0';
			continue;
		}

		else // Windows remove the quotes, making debug /"q" appear ar debug /q
		{
			size_t length = strlen (position);
			if (length > MAX_COMMAND_LENGTH - 2) logError ("command too long");
			strcpy (cmdline, position);
			breakUpCommandLine (cmdline);
			continue;
		}
	}

	// command processing loop
	currentCommand = cmdline;
	
	if (*currentCommand == '\0') startupBanner ();

	for (;;)
	{
		char *error, *nextCommand, *argv [MAX_ARGS];
		int argc;
		// if the entire command line has been processed, get another one
		while (*currentCommand == '\0' && currentCommand < cmdline + MAX_COMMAND_LENGTH) currentCommand++;
		if (*currentCommand == '\0')
		{

			getCommand (cmdline);
			breakUpCommandLine (cmdline);
 			currentCommand = cmdline;

		}

		// find the next command line
		nextCommand = currentCommand + strlen (currentCommand) + 1;

		// break up the command into separate arguments
		argc = 1;
		removeTrailingSpaces (currentCommand);
		if (*currentCommand == '\0') continue;
		position = currentCommand;
		for (;;)
		{
			position = skipws (position);
			argv [argc++] = position;
			// find the next non-quoted space
			position = strchr (position, ' ');
			if (!position) break;
			*position = '\0';
			position++;
			if (argc + 1 == DIMENSION (argv)) break;
		}

		currentCommand = nextCommand;

		// process the broken up command
		if (argc == 1) continue;
		error = processCommand (argc, argv);
	
		if (error) 
		{
			scrollok(stdscr,TRUE);

			printw ("%s\n", error);

			scrollok(stdscr,FALSE);
		}
		
		// flush input that may be present from canceling a command
		while (kbhit ()) getch(); 
		
	}

	endwin();
}
