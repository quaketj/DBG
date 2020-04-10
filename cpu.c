#include<stdio.h>
#include<stdlib.h>

#include "project.h"

void cpuidCall(unsigned int i, unsigned int * buf)
{
   unsigned int eax,ebx,ecx,edx;
   __asm__ (
    "cpuid"
    :"=a"(eax),"=b"(ebx),"=c"(ecx),"=d"(edx):"a"(i));
    buf[0]=eax;
    buf[1]=ebx;
    buf[2]=edx;
    buf[3]=ecx;
}

//---------------------------------------------------------------------------

char* displayCpuid ()
   {
   int index, functionCount;
   int buf[4];
   functionCount = 8;
   scrollok(stdscr,TRUE);
   printw ("function     EAX       EBX       ECX       EDX\n");
   scrollok(stdscr,FALSE);
   for (index = 0; index < functionCount; index++)
   	{
   		cpuidCall(index,buf);
		scrollok(stdscr,TRUE);
       	printw("%8x     %08x  %08x  %08x  %08x\n", index, buf[0],buf[1],buf[2],buf[3]);
		scrollok(stdscr,FALSE);
   	}

   functionCount =9;
   for (index = 0; index < functionCount; index++)
   	{
   		cpuidCall(0x80000000 + index, buf);
		scrollok(stdscr,TRUE);
     	printw("%8x     %08x  %08x  %08x  %08x\n", 0x80000000 + index,  buf[0],buf[1],buf[2],buf[3]);
		scrollok(stdscr,FALSE);
			
   	}

   return NULL; 
   }
