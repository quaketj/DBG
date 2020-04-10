#ifndef _EDIT_H_
#define _EDIT_H_

#undef  TERMIOSECHO
#define TERMIOSFLUSH

#define MAX_COMMAND_LENGTH 800 // longest command line to accept
#define MAX_ARGS 200           // maximum number of command line arguments to accept
#define MAXRETRIEVE  20     /* number of commands in retrieve buffer */
#define maxAddressDigits 9     // max number of hex digits in physical memory address


#define DIMENSION(array) (sizeof (array) / sizeof (array [0]))
#define min(x,y) ((x) < (y) ? (x) : (y))

//static void getCommand (char *buffer);
//static void breakUpCommandLine (char *buffer);
void removeTrailingSpaces (char *buffer);
char *formatMessage (char *szFmt);
//static char *processCommand (int argc, char *argv []);
void logError (char *szFmt,...);
char *skipws (const char *position);

#endif

