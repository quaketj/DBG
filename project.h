#ifndef _PROJECT_H_
#define _PROJECT_H_

#include <sys/io.h>
//#include <ncurses.h>
#include <curses.h>


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <linux/types.h> 
#include <sys/ioctl.h>
#include <sys/time.h>
#include <ctype.h>


#include <linux/pci.h>
//#include <pci/header.h>
//#include <pci/types.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef unsigned long long  UINT64;
typedef long long           INT64;
typedef unsigned int        UINT32;
typedef int                 INT32;
typedef unsigned short      UINT16;
typedef unsigned short      CHAR16;
typedef short               INT16;
typedef unsigned char       BOOLEAN;
typedef unsigned char       UINT8;
typedef char                CHAR8;
typedef signed char         INT8;

typedef unsigned long long  u64;
typedef unsigned int 		u32;
typedef unsigned short		u16;
typedef unsigned char       u8;


typedef unsigned int 		DWORD;
typedef unsigned short		WORD;
typedef unsigned char       BYTE;





/* e820 */
#include <asm/e820.h>
#include <dirent.h>
#include <asm-generic/errno-base.h>

/******************************/
#include "common.h"
#include "edit.h"
#include "misc.h"
#include "memory.h"
#include "smbus.h"
#include "e820.h"
#include "apic.h"
#include "ipmitool.h"
//#include "isa.h"

//cpu.c

char* displayCpuid ();





#endif
