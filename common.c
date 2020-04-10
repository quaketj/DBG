#include "project.h"


unsigned int pullbit (unsigned int data, unsigned int bitNumber)
{
   return (data >> bitNumber) & 1;
}

unsigned int pullbits (unsigned int source, int lsb, int msb)
{
   return (source >> lsb) & ((1 << (msb - lsb + 1)) - 1);
}

unsigned int populationCount (void *buffer, unsigned int byteCount)
{
   unsigned int total = 0, index, bitNumber;
   char *buffer8 = buffer;
   
   for (index = 0; index < byteCount; index++)
      for (bitNumber = 0; bitNumber < 8; bitNumber++)
         if (buffer8 [index] & (1 << bitNumber)) total++;

   return total;
}

char  highestSetBit (void  *buffer, char  byteCount)
{
   char  index;
   char  bitNumber;
   char   *buffer8 = buffer;
   
   for (index = byteCount - 1; index >= 0; index--)
   	{
   	//Print(L"byteCount  %x\n",byteCount);
      for (bitNumber = 8; bitNumber > 0; bitNumber--)
      		{
      			//Print(L"bitNumber  %x\n",bitNumber);
        	 if (buffer8 [index] & (1 << (bitNumber -1))) return( index * 8 + bitNumber -1);
			 //Print(L"Buffer  %c",buffer8[index]);
      		}
   	}
   return 0;
}

char *buildDataList (int argc, char *argv[], int dataSize, u64 *data, int *elements)
{
   int index;
   u64 value;
   char *error;

   int count = 0;


   for (index = 0; index < argc; index++) {
      char *position = argv [index];
      if (*position == '\'' || *position == '\"') {
         position++;
         while (*position != '\0' && *position != argv [index][0])
            data [count++] = *position++;
         continue;
      }
      error = hextoint64 (position, &value, dataSize / 4); 
      if (error) return error;
      if (dataSize < 32)
         if (value >= ((u64) 1 << dataSize)) return formatMessage ("data too big");
     if (dataSize = 32)
         if (value > 0xffffffff) return formatMessage ("data too big");
 
      data [count++] = value;
   }

   *elements = count;
   return NULL;
}

unsigned samePattern (u64 *pattern, __u8 *buffer, unsigned patternSize)
{
   unsigned index;

   for (index = 0; index < patternSize; index++)
      if (buffer [index] != (__u8) pattern [index]) return 0;
   return 1;
}

//---------------------------------------------------------------------------
//
//  hextoint64 - converts hex string to binary
//
//   argument1: char*   position  - start of hex buffer
//   argument2: UINT64* result    - converted number
//   argument3: int     maxdigits - maximum allowed hex digits
//   return:    char*   NULL      - 0 good
//                      other     - error text
//
//---------------------------------------------------------------------------
  
char *hextoint64 (char *position, u64 *result, int maxDigits)
   {
   int digit, digitCount = 0;
  
   *result = 0;
   while (1)
      {
      digit = toupper (*position++);
      if (!isxdigit (digit)) return formatMessage ("bad hex digit");
      if (digit > '9')
         digit -= 'A' - 10;
      else
         digit -= '0';
  
      *result *= 16;
      *result += digit;
      if (++digitCount > maxDigits) return formatMessage ("too many hex digits");
      if (*position == '\0') break;
      }
   return NULL;
   }


void printMultiChar (char *Buffer)
{
	char  loop;
	for (loop = 0; loop<strlen(Buffer); loop++) {
      		printw( "%c", Buffer[loop]);
  	 }

	printw("\n");
}


