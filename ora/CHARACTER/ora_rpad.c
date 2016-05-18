#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <ksu_common.h>

/*
 *   RPAD( string, padded_length, [ pad_string ] )
 *
 *   Right pads "string" to "padded length" characters (not bytes),
 *   with spaces by default, or 'pad_string' otherwise
 *
 *   string is the string to pad characters to (the left-hand side).
 *
 *   padded_length is the number of characters to return.
 *   If the padded_length is smaller than the original string, the RPAD
 *   function will truncate the string to the size of padded_length.
 *
 *   pad_string is optional. This is the string that will be padded to
 *   the right-hand side of string. If this parameter is omitted, the LPAD
 *   function will pad spaces to the right-side of string.
 */
extern void ora_rpad(sqlite3_context  *context,
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

   _ksu_check_arg_cnt(argc, 2, 3, "rpad");
   if (ksu_prm_ok(context, argc, argv, "rpad",
                  KSU_PRM_TEXT, KSU_PRM_INT_GT_0, KSU_PRM_TEXT)) {
      str = (unsigned char *)sqlite3_value_text(argv[0]);
      len = sqlite3_value_int(argv[1]);
      if (argc == 2) {
         // Default padding
         pad = (unsigned char *)" ";
         padlen = 1;
      } else {
         pad = (unsigned char *)sqlite3_value_text(argv[2]);
         padlen = ksu_charlen(pad);
      }
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
      p = str;
      i = 0;
      // First copy the string
      while (*p && (i < len)) {
         i++;
         *r = *p;
         r++;
         if ((*(p++)) >= 0xc0) {
            while ((*p & 0xc0) == 0x80) {
               *r = *p;
               p++;
               r++;
            }
         }
      }
      // Then insert padding (if required length
      // is greater than string length)
      p = pad;
      while (i < len) {
         i++;
         *r = *p;
         r++;
         if ((*(p++)) >= 0xc0) {
            while ((*p & 0xc0) == 0x80) {
               *r = *p;
               p++;
               r++;
            }
         }
         // Check whether we must repeat the padding
         if (*p == '\0') {
            p = pad;
         }
      }
      *r = '\0';
      sqlite3_result_text(context,
                          (char *)result,
                          -1,           // Length - -1 means terminated by \0
                          sqlite3_free); // Function for freeing memory
   
   }
}
