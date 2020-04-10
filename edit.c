#include "project.h"


static  struct                                                             
{
	int  ages [MAXRETRIEVE];            //for newest, 2, 3..., MAXRETRIEVE   
	int  readage;                       // age to find on next retrieve      
	int  oldest;                        // age of oldest in retrieve buffer  
	int  oldestindex;                   // index of age of oldest in buffer  
	char command [MAXRETRIEVE] [MAX_COMMAND_LENGTH + 1];
} retrieve = {{1}, 1, 1};           	// command line retrieve system  

static int skipTrailingSpaces (char *buffer, int length)
    {
    int  ch;

    while (length)
        {
        ch = buffer [length - 1];
        if (ch != ' ' && ch != '\t' && ch != 0) break;
        length--;
        }
    return length;
    }
    

void removeTrailingSpaces (char *buffer)
{
	char *position;

	if (buffer [0] == '\0') return;

	position = buffer + strlen (buffer) - 1;

	while (*position == ' ')
	{
		*position = '\0';
		position--;
	}
}


int stricmp(const char *s1,const char *s2){
	int i;
	for(i = 0;s1[i]!='\0'&&s2[i]!='\0';i++){
		if(s1[i]!=s2[i])return s1[i]-s2[i];
	}
	if(s1[i]=='\0'&&s2[i]!='\0')return 0-s2[i];
	else if(s1[i]!='\0'&&s2[i]=='\0') return s1[i];
	else return 0;
}

//----------------------------------------------------------------------------
//
// formatMessage - return formatted message in static buffer
//
char *formatMessage (char *szFmt)
{
   //va_list Marker;
   //char buffer[200];
   //int cycle;
   //char *position = buffer[cycle];
   //cycle++;
   //if (cycle == DIMENSION (buffer)) cycle = 0;

	//va_start (Marker, szFmt);
    //snprintf (position,szFmt);
    //va_end (Marker);
    printw("%s\n",szFmt);
    return szFmt;
}


//----------------------------------------------------------------------------
//
// logError - log message and exit
//
void logError (char *szFmt,...)
{
	va_list Marker;
	char    buffer [256];

	va_start (Marker, szFmt);
	snprintf (buffer, sizeof buffer, szFmt, Marker);
	va_end (Marker);
	printf ("\n                              ");
	printf ("\n*** %s ***\n", buffer);
	exit (1);
}

static void locate (int x, int y)
{
	move(y,x);
}


static __u32 getkey (void)
{
	unsigned int keyCode;
    // make DOS and Windows match
    keyCode = getch ();
	return keyCode;
}

//------------------------------------------------------------------------------
//
// the buffer must not contain NULL characters (pad with spaces)
//
static int displayEditField (int x, int y, int length, char *buffer)
{
	int offset;
	char temp [MAX_COMMAND_LENGTH + 1];
	// make a NULL terminated copy of the space padded string for printing
	memcpy (temp, buffer, length);
	temp [length] = '\0';
	for (offset = length - 1; offset > 0; offset--)
		if (temp [offset] != ' ') break;

	locate (x, y);
	printw ("%s", temp);
	
	return offset;
}

static int editField (int x, int y, int length, char *buffer)
{
	int   keycode;
	int   ascii;
	int   i, j, age, lengthnts;
	int   processed;
	int   printable;
	int   cpos;
	int   insertmode = 1;

	static char printtype [] =
	{
		0,0,0,0,0,0,0,0,                /* control characters */
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,

		1,1,1,1,1,1,1,1,                /* space ! " # $ % & ' ( ) * + , - . / */
		1,1,1,1,1,1,1,1,

		1,1,1,1,1,1,1,1,1,1,            /* digits (0 1 2 3 4 5 6 7 8 9) */

		1,1,1,1,1,1,1,                  /* : ; < = > ? @ */

		1,1,1,1,1,1,1,1,1,1,1,1,1,      /* A-Z (upperegs-case alphabet) */
		1,1,1,1,1,1,1,1,1,1,1,1,1,

		1,1,1,1,1,1,                    /* [ \ ] ^ _ ` */

		1,1,1,1,1,1,1,1,1,1,1,1,1,      /* a-z (loweregs-case alphabet) */
		1,1,1,1,1,1,1,1,1,1,1,1,1,

		1,1,1,1,0,                      /* { | } ~ DEL */

		0};                             /* graphics */

		// display with all data selected   
		cpos = displayEditField (x, y, length, buffer);
		locate (x + cpos, y);
		for (;;)
		{
			keycode = getkey ();                    /* read the keyboard */
			ascii = keycode & 0xff;                 /* ASCII code if non-zero */
			processed = 0;                          /* key not processed yet */
			if (ascii)
				printable = printtype [ascii];
			else
				printable = 0;
			if (printable)
			{
				processed = 1;                      /* keycode is processed */
				if (insertmode && cpos + 1 < length)
					for (i = length - 1; i > cpos; i--)
						buffer [i] = buffer [i - 1];
				buffer [cpos] = keycode;            /* update edit buffer */
				if (cpos + 1 < length)          
					cpos++;                         /* move cursor right */
			}
			if (keycode == KEY_RIGHT)                  /* right arrow */
			{
				processed = 1;                      /* keycode is processed */
				if (cpos + 1 < length) cpos++;
			}
			else if (keycode == KEY_LEFT)             /* left arrow */
			{
				processed = 1;                      /* keycode is processed */
				if (cpos) cpos--;
			}
			else if (keycode == KEY_IC)             /* toggle INSERT mode */
			{
				processed = 1;                      /* keycode is processed */
				insertmode ^= 1;                    /* toggle insert mode */
			}
			else if (keycode == KEY_DC)             /* delete key */
			{
				processed = 1;                      /* keycode is processed */
				memcpy  (buffer + cpos,
					buffer + cpos + 1,
					length - cpos);
				buffer [length - 1] = ' ';
			}
			else if (keycode == KEY_UP)             /* up arrow (retrieve function) */
			{
				processed = 1;                      /* keycode is processed */

				for (i = 0; i < MAXRETRIEVE; i++)   /* scan for next to read */
					if (retrieve. ages [i] == retrieve. readage) break;

				memcpy (buffer, retrieve. command [i], length);
				if (!*buffer) memset (buffer, ' ', length);
				cpos = skipTrailingSpaces (buffer, length);    /* put cursor at end of line */

				retrieve. readage++;                        /* set up next retrieve */
				if (retrieve. readage > retrieve. oldest)   /* if all retrieved */
					retrieve. readage = 1;                  /* start from newest */
			}
		    else if (ascii == 10 || ascii == 13) /* Enter key */
			{
				scrollok(stdscr,TRUE);
				printw ("\n");
				scrollok(stdscr,FALSE);
				
				scroll(stdscr);
				lengthnts = skipTrailingSpaces (buffer, length);
				if (lengthnts == 0)
					break;                              /* nothing entered */

				/*----------------keyboard retrieve function---------------*/

				/* if command already in buffer, make it the newest */
				for (i = 0; i < MAXRETRIEVE; i++)
				{
					if (lengthnts != skipTrailingSpaces (retrieve. command [i], length)) continue;
					if (memcmp (retrieve. command [i], buffer, lengthnts)) continue;

					age = retrieve. ages [i];           /* rotate this and newer */
					for (j = 0; j < MAXRETRIEVE; j++)   /* make all others older */
					{
						if (retrieve. ages [j] == 0)    /* skip if not in use */
							continue;
						if (retrieve. ages [j] > age)
							continue;                   /* skip if past rotation */
						retrieve. ages [j]++;           /* make it older */
					}
					retrieve. ages [i] = 1;             /* make current newest */
					retrieve. readage = 1;              /* reset search position */
					break;                              /* were done */
				}
				if (i != MAXRETRIEVE) break;

				/* add command to retrieve buffer. Make current entries older */
				for (i = 0, j = 0; i < MAXRETRIEVE; i++)
				{
					if (retrieve. ages [i] == MAXRETRIEVE)  /* if too old to keep */
						retrieve. ages [i] = 0;             /* free it */
					if (retrieve. ages [i] == 0)            /* if entry unused */
						j = i;                              /* mark the spot */
					else
						retrieve. ages [i]++;               /* make entry older */
					if (retrieve. ages [i] >= retrieve. oldest)
					{
						retrieve. oldest = retrieve. ages [i];  /* record oldest */
						retrieve. oldestindex = i;
					}
				}
				memcpy (retrieve. command [j], buffer, length);

				/* keep oldest empty to show when all choices have been displayed */
				if (retrieve. oldest == MAXRETRIEVE)
					memset (retrieve. command [retrieve. oldestindex], 0, length);

				retrieve. ages [j] = 1;                     /* make newest */
				retrieve. readage = 1;                      /* reset search position */
				break;
			}
			
			else if (keycode == KEY_BACKSPACE)                    /* Backspace key */
			{
				processed = 1;                      /* keycode is processed */
				if (cpos == 0)
					memcpy (buffer, buffer + 1, length);
				else
				{
					cpos--;
					memcpy (buffer + cpos, buffer + cpos + 1, length - cpos);
				}
				buffer [length - 1] = ' ';
			}
			else if (keycode == KEY_HOME)             /* home key */
			{
				processed = 1;                      /* keycode is processed */
				cpos = 0;
			}
			else if (keycode == KEY_END)             /* end key */
			{
				processed = 1;                      /* keycode is processed */
				cpos = length - 1;
				while (cpos && (buffer [cpos - 1] == ' ' ||buffer [cpos - 1] == 0))
					cpos--;
			}
			else if (keycode == 27)             /* escape key */
			{
				processed = 1;                      /* keycode is processed */
				cpos = 0;
				memset (buffer, ' ', length);
			}

			if (processed == 0) break;              /* return key if not processed */
			displayEditField (x, y, length, buffer);
			locate (x + cpos, y);
		}

		buffer [skipTrailingSpaces (buffer, length)] = '\0';
		return keycode;
}


int editLine (char *buffer, int width)
{
	unsigned int keyCode = 0, x, y;
#ifndef WENSON_DEBUG_
	getyx(stdscr,y,x);
#endif
	width =min (width, 80);
	memset (buffer, ' ', width);
	while (keyCode != 0x0D)
		keyCode = editField (x, y, width, buffer); 
	return keyCode;
}

char *skipws (const char *position)
 {
   while (*position == ' ' || *position == '\t') position++;
   return (char *) position;
}





