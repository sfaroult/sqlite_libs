#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sqlite3.h>

#include <ksu_common.h>

/*
 *   CONCAT(string_part_1, string_part_2)
 *
 *   CONCAT combines two strings of characters together. It requires two parameters,
 *   both of which must be strings of characters. This function will not combine
 *   numeric types.
 *
 *   string_part_1 represents the base string -- the string to be added on to.
 *
 *   string_part_2 represents the string to be added -- it will be placed at the end of string_part_1.
 */
extern void ora_concat(sqlite3_context  *context,
                       int               argc,
                       sqlite3_value   **argv) {
   int     len1;  //number of characters in the 1st string
   int     len2;  //number of characters in the 2nd string
   char   *str1;  //The first string
   char   *str2;  //The second string
   char   *result;//The result string

   // Note that concat() considers that a NULL is an empty string
   if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
     if (sqlite3_value_type(argv[1]) == SQLITE_NULL) {
         sqlite3_result_null(context);
         return;
     }
     str1 = "";
     str2 = (char *)sqlite3_value_text(argv[1]);
   } else {
     str1 = (char *)sqlite3_value_text(argv[0]);
     str2 = (char *)sqlite3_value_text(argv[1]);
     if (str2 == NULL) {
       str2 = "";
     }
   }
   len1 = strlen(str1);
   len2 = strlen(str2);

   //Allocate enough memory for the combined string,
   //plus one byte for the '\0' character)
   if ((result = (char *)sqlite3_malloc(len1 + len2 + 1)) == NULL) {
     // Don't even try to check what goes wrong ...
     // Rats leaving ship.
     sqlite3_result_null(context);
     return;
   }

   //Use the following two fuctions to actually combine the strings.
   strcpy(result, str1);
   strcat(result, str2);

   //Return the concatenated string.
   sqlite3_result_text(context,
                       result,
                       -1,           // Length - -1 means terminated by \0
                       sqlite3_free); // Function for freeing memory
}
