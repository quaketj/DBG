 /*! 
 *\file e820.c
 *
 * e820 related operations to retrieve e820 map.
 *
 * Author: Jackie Liu
 *
 * Copyright (c) 2012 Jabil Circuit.
 *
 * This source code and any compilation or derivative thereof is the sole
 * property of Jabil Circuit and is provided pursuant to a Software License
 * Agreement. This code is the proprietary information of Jabil Circuit and
 * is confidential in nature. Its use and dissemination by any party other
 * than Jabil Circuit is strictly limited by the confidential information
 * provisions of Software License Agreement referenced above.
 
 *\par ChangeLog:
 * July 25, 2012      The initial version
  */


#include "project.h"

struct mye820map {
	__u32 nr_map;
	struct e820entry map[E820MAX];
};
struct mye820map mye820map;

int e820_handler(char *path, int i)
{
	FILE *file;
   	char fileStr[256];
   	char typeStr[30];
   	//__u64 startAdd, endAdd;
   	unsigned long long startAdd, endAdd;
   	char start[64], end[64];
   	int type;

	/* get start address */
   	sprintf(fileStr, "%s/start", path);
    file = fopen(fileStr, "r");
    if( file == NULL) {
        return -1;
    }
    //fscanf does not work well
    fgets(start, sizeof(start), file);
    fclose(file);


	/* get end address */	
    sprintf(fileStr, "%s/end", path);
    file = fopen(fileStr, "r");
    if( file == NULL) {
        return -1;
    }
    fgets(end, sizeof(end), file);
    fclose(file);


	/* get mem type */
    sprintf(fileStr, "%s/type", path);
    file = fopen(fileStr, "r");
    if( file == NULL) {
        return -1;
    }
    fgets(typeStr, sizeof(typeStr), file);
    fclose(file);
  
	/* stroe start, end and type information */
    sscanf(start, "0x%llx", &startAdd);
    sscanf(end, "0x%llx", &endAdd);

    type=e820_string_to_type(typeStr);
    
    e820_add_entry(startAdd, endAdd, type, i);
   
	return 0;
}

char* e820_print_type(u32 type)
{
	char buf[256];

	switch (type) {
		case E820_RAM:
		case E820_RESERVED_KERN:
			return "(usable)";
		case E820_RESERVED:
			return "(reserved)";
		case E820_ACPI:
			return "(ACPI data)";
		case E820_NVS:
			return "(ACPI NVS)";
		case E820_UNUSABLE:
			return "(unusable)";
		default:
			return "(Not Know)";
	}
}

int e820_string_to_type(char *stringType)
{  
   if(strncasecmp("System RAM", stringType, strlen("System RAM")) == 0)
      return E820_RAM;
   else if(strncasecmp("ACPI Tables", stringType, strlen("ACPI Tables")) == 0)
      return E820_ACPI;
   else if(strncasecmp("ACPI Non-volatile Storage", stringType, strlen("ACPI Non-volatile Storage")) == 0)
      return E820_NVS;
   else
      return E820_RESERVED;
}

//Specially for e820 entries under /sys/firmware/memmap
int enumerate_dir_by_path(const char *path, int (*handler)(char *, int entryNum)) 
{
    struct dirent* ent = NULL;
    DIR *pDir;
	int i;
	char fullDirPath[256];
	
	pDir = opendir(path);
    if( pDir == NULL ) {
      //return -EINVAL;
      return -1;
    }
    while(NULL != (ent = readdir(pDir))) {
        if(ent->d_type == 8) {
        //file
        } else {
            //directory
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                continue;
            }
			//printf("%s\n", ent->d_name);
            i = atoi(ent->d_name);
			sprintf(fullDirPath,"%s/%s",path, ent->d_name);
			if(handler != NULL)
			    handler(fullDirPath, i);
        }
    }
	return 0;
}


char* e820_analysis (int argc, char* argv [])
{
    int i;
    char *path="/sys/firmware/memmap";
    char cmdline[512];
	char*  status = NULL;

	scrollok(stdscr,TRUE);
    enumerate_dir_by_path(path, e820_handler);
   	if( mye820map.nr_map == 0) {
    	printw("Cannot get e820 map from /sys/firmware/memmap, test cannot continue\n");
	  	return (char *)-1;
   	}

    for(i = 0; i < mye820map.nr_map; i++) {
		printw("bios-e820 %02d:  ",i);
	    printw("%016llx--",mye820map.map[i].addr);
	    printw("%016llx  ",mye820map.map[i].addr + mye820map.map[i].size );
	    printw("%s\n",e820_print_type(mye820map.map[i].type));
   	}

	scrollok(stdscr,FALSE);
  	return status;
}

int e820_add_entry(__u64 start, __u64 end, int type, int entry)
{
	if(entry >= E820_X_MAX)
    	return -1;
   	if (end <= start)
    	return -1;
   	if( mye820map.map[entry].size == 0 ) {
    	mye820map.nr_map ++;
	}
   	mye820map.map[entry].addr = start;
   	mye820map.map[entry].size = end - start + 1;
   	mye820map.map[entry].type = type;

  	return 0;
}


