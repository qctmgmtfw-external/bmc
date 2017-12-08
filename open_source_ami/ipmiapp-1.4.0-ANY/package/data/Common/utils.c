#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <coreTypes.h>
#include "icc_what.h"

#include "crc32.h"
#include <stdlib.h>

#ifdef ICC_OS_WINDOWS
#include <malloc.h>
#endif


#ifdef ICC_OS_WINDOWS
#include <conio.h>
int GetPasswordInput(char* pwd)
{
 int  l=0;
 char c;

 printf("Password : ");
 fflush(stdout);

 for ( ;; )
 {
   c = (char)getch();
   switch ( c )
   {
     case '\b':  // Backspace
       if ( l>0 )
       {
         putchar('\b');
         putchar(' ');
         putchar('\b');
         l--;
       }
       break;
     case '\r':  // Return (Enter)
       pwd[l] = '\0';
       return 0;
     default:
       putchar('*');
       if ( l<256 )
         pwd[l++] = (char)c;
       break;
   }

 }
    return 0;
}
#endif  /*  ICC_OS_WINDOWS  */

#ifdef ICC_OS_LINUX
int GetPasswordInput(char* pwd)
{
    strcpy(pwd,(const char *) getpass("Password : "));
    return 0;
}
#endif

