#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ksu_common.h>

/*
 *   INITCAP(string_to_be_formatted)
 *
 *   INITCAP makes the first letter of each word a capital letter, while
 *   making the rest of the words lowercase. Numbers and punctuation
 *   remain the same.
 *
 *   string_to_be_formatted represents the input string of characters
 *   that the user wishes to format. It is the only parameter.
 */
extern void pg_initcap(sqlite3_context  *context,
                       int argc,
                       sqlite3_value   **argv)
{
   int len1;  //The number of characters in the string that is being passed in.
   unsigned char   *str1;         // The string being passed in.
   unsigned char   *str2;         // Result 
   unsigned char   *strptr1 = NULL;// An additional pointer to str1.
   unsigned char   *strptr2 = NULL;// An additional pointer to str2.
   unsigned char    utf8[5];

   _ksu_null_if_null_param(argc, argv);
   // In this case, we know that we have valid input. We read in
   // the input, and calculate the length of the string. We also
   // assign an additional pointer to the string for the purpose
   // of formatting the string.
   str1 = (unsigned char *)sqlite3_value_text(argv[0]);
   strptr1 = str1;
   len1 = ksu_charlen(str1);
   if ((str2 = (unsigned char *)sqlite3_malloc(1 + sizeof(unsigned char)
                              * len1 * 4)) != (unsigned char *)NULL) {
      int first; // This simply represents whether or not we are modifying
                 // the first letter of a word. It is 1 if so, and 0 if not.

      // Now we need to go through the entire string to ensure proper
      // formatting. This will make iniial characters uppercase, the other
      // characters lowercase, and leave the other characters the same.
      strptr2 = str2;
      while (*strptr1 != '\0') {
         first = 1;
         while (*strptr1 && !ksu_is_letter(strptr1)) {
            _ksu_utf8_copychar(strptr1, strptr2);
         }
         while (*strptr1 && ksu_is_letter(strptr1)) {
            if (first == 1) {
               strcpy((char *)strptr2,
                      (char *)ksu_utf8_charupper(strptr1, utf8));
               while (*strptr2) {
                  SQLITE_SKIP_UTF8(strptr2);
               }
               SQLITE_SKIP_UTF8(strptr1);
               first = 0;
            } else   {
               strcpy((char *)strptr2,
                      (char *)ksu_utf8_charlower(strptr1, utf8));
               while (*strptr2) {
                  SQLITE_SKIP_UTF8(strptr2);
               }
               SQLITE_SKIP_UTF8(strptr1);
            }
         }
      }
      //The formatted string is then outputed to SQLite.
      sqlite3_result_text(context,
                       (char *)str2,
                       -1,           // Length - -1 means terminated by \0
                       sqlite3_free);  // Function for freeing memory
   } else {
      sqlite3_result_null(context);
   }
}
