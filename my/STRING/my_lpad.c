#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>
#include <ksu_my.h>

/*
 *   LPAD( string, padded_length, pad_string)
 *
 *   Left pads "string" to "padded length" characters (not bytes),
 *   with "pad_string".
 *
 *   string is the string to pad characters to (the left-hand side).
 *
 *   padded_length is the number of characters to return.
 *   If the padded_length is smaller than the original string, the LPAD
 *   function will truncate the string to the size of padded_length.
 *
 */
extern void my_lpad(sqlite3_context  *context,
                    int argc,
                    sqlite3_value   **argv) {
   int len;                // Required length
   int clen;               // number of CHARACTERS in the string
   unsigned char   *str;
   unsigned char   *pad;
   unsigned char   *result;
   unsigned char   *r;
   unsigned char   *p;
   int padlen;             // CHARACTER length of padding
   int i;

   _ksu_null_if_null_param(argc, argv);
   str = (unsigned char *)sqlite3_value_text(argv[0]);
   len = my_value_int(argv[1], 0);
   if (len < 0) {
     sqlite3_result_null(context);
   } else if (len == 0) {
     sqlite3_result_text(context, "", -1, SQLITE_STATIC);
   } else {
     pad = (unsigned char *)sqlite3_value_text(argv[2]);
     padlen = ksu_charlen(pad);
     // Allocate memory for 4 * required length (as a character
     // is at most four bytes), plus one for \0
     if ((result = (unsigned char *)sqlite3_malloc(4 * len + 1)) == NULL) {
       sqlite3_result_error_nomem(context);
       return;
     }
     // Check the length of the original string
     clen = ksu_charlen(str);
     // Copy - i is a CHARACTER counter
     r = result;
     p = pad;
     i = 0;
     // First insert padding (if required length
     // is greater than string length)
     while (i < len - clen) {
       i++;
       _ksu_utf8_copychar(p, r);
       // Check whether we must repeat the padding
       if (*p == '\0') {
         p = pad;
       }
     }
     // Now add the string
     p = str;
     while (i < len) {
       i++;
       _ksu_utf8_copychar(p, r);
     }
     *r = '\0';
     sqlite3_result_text(context, (char *)result, -1, sqlite3_free);
   }
}
