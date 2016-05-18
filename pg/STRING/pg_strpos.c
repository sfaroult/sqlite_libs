#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

/*
 * STRPOS( string text, substring text )
 *
 * returns the location of the first occurrence
 * of the substring
 *
 * counting starts at 1 from the left
 *
 * returns 0 if the substring is not found
 * or if it is empty
 */
extern void pg_strpos(sqlite3_context  *context,
                      int               argc,
                      sqlite3_value   **argv) {
   char *string;
   char *substr;
   char *p;
   int loc = 1;

   _ksu_null_if_null_param(argc, argv);
   string = (char *)sqlite3_value_text(argv[0]);
   substr = (char *)sqlite3_value_text(argv[1]);
   if (strlen(substr) == 0) {
     sqlite3_result_int(context, 1);
     return;
   }
   p = strstr(string, substr);
   if (p == (char *)NULL) {
     sqlite3_result_int(context, 0);
   } else {
     // Must count utf8 characters
     loc = ksu_bytes_to_charpos((unsigned char *)string, p - string);
     if (loc == -1) {
       ksu_err_msg(context, KSU_ERR_INV_UTF8, "strpos");
     } else {
       sqlite3_result_int(context, 1 + loc);
     }
   }
}
