/*
 * ora_unistr.c
 * 
 * This program is used in SQLite and will take character data and return it in
 * the national character set
 * 
 * Input - Character data Output - The national character set conversion
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

/*
 * Converts character data to its national character set
 */

extern void ora_unistr(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
        unsigned char  *str;
        unsigned char  *p;
        unsigned char  *result;
        unsigned char   byte;
        int             i;

        _ksu_null_if_null_param(argc, argv);
        str = (unsigned char *)sqlite3_value_text(argv[0]);
        p = str;
        result = (unsigned char *)sqlite3_malloc(strlen((char *)str) + 1);

        i = 0;

        //Loop through the input
        while (*p != '\0') {
          // Checks to see if it is the beginning of a character
          // set given in hexadecimal form
          if (*p == '\\') {
            p++;
            //If incorrect input
            if (*p == '\0') {
               sqlite3_result_error(context, "Invalid UTF8 String", -1);
               return;
            }
            //Conversion for the first byte
            tolower(*p);
            int             x = 0;
            byte = 0;
            if (*p >= '0' && *p <= '9') {
              byte = (*p - '0') * 16;
              x = 1;
            }
            if (*p >= 'a' && *p <= 'f') {
              byte = (((*p - 'a') + 10) * 16);
              x = 1;
            }
            if (x == 0) {
              sqlite3_result_error(context, "Invalid UTF8 String", -1);
              return;
            }
            p++;

            if (*p == '\0') {
               sqlite3_result_error(context, "Invalid UTF8 String", -1);
               return;
            }
            //Conversion for the second byte
            tolower(*p);
            x = 0;
            if (*p >= '0' && *p <= '9') {
               byte += (*p - '0');
               x = 1;
            }
            if (*p >= 'a' && *p <= 'f') {
              byte += ((*p - 'a') + 10);
              x = 1;
            }
            if (x == 0) {
              sqlite3_result_error(context, "Invalid UTF8 String", -1);
              return;
            }
            result[i] = byte;
          } else {
            result[i] = *p;
          }
          p++;
          i++;
        }
        result[i] = '\0';
        sqlite3_result_text(context, (char *)result, -1, sqlite3_free);
        //Display the result and free memory
}
