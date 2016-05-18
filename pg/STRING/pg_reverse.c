#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ksu_common.h>

//
//  Sets the passed pointer equal to the position
//  'len' spaces ahead of it
//
static unsigned char *skipByLen(unsigned char *s, int len) {
   int i = 0;
   unsigned char *p = s;

   while (i != len) {
      i++;
      // Beware that the macro increments p
      SQLITE_SKIP_UTF8(p);
   }
   return p;
}

/*
 *  REVERSE( string )
 *
 *  Returns the reversed order of the passed string
 *
 *  string is the string that needs to be reversed
 *
 */
extern void pg_reverse(sqlite3_context   *context,
                       int argc,
                       sqlite3_value     **argv) {
   int stringLen;            // overall length of memory for string
   int characterLen;         // overall length of characters in string
   unsigned char   *str;     // pointer to the begining of the passed string
   unsigned char   *result;  // pointer to the begining of the result string
   int i;                    // counter
   int offset;
   unsigned char   *cpoint; 
             // pointer to the next open space in the result string
   unsigned char   *spoint;
             // pointer to the begining of a UTF-8 char in the passed string
   int sizechar;
             // calculated size of the current passed string UTF-8 char

   _ksu_null_if_null_param(argc, argv);
   str = (unsigned char *)sqlite3_value_text(argv[0]);

   // get the MEMORY length of the string in characters for reserving memory
   stringLen = strlen((const char *)str);
   // get the CHARACTER length of the string to reverse
   characterLen = ksu_charlen(str);

   // Allocate memory, including the \0 character
   result = (unsigned char *)sqlite3_malloc(stringLen + 1);
   if (result == NULL) {
     sqlite3_result_error_nomem(context);
     return;
   }
   *result = '\0';
   i = 0;
   offset = 0;
   // Copy the character from the end of the string to the begining index
   while (i < characterLen) {
     // Get the starting pointer for the result string
     cpoint = skipByLen(result, i);
     // Get the pointer for the (possible) UTF-8 character
     spoint = skipByLen(str, characterLen - i - 1);
     // Calculate the size of the (possible) UTF-8 character
     sizechar = (str + stringLen - offset) - spoint;
     // Copy the UTF-8 character into the return string
     memcpy(cpoint, spoint, sizechar);
     offset += sizechar;
     i++;
   }

   // Add a termination character to the end of the result string
   *(result + stringLen) = '\0';
   sqlite3_result_text(context,
                       (char *)result,
                       -1,
                       sqlite3_free);
   return;
}
