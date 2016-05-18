#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

/*
 * FROM ORACLE DOCUMENTATION:
 *
 * CHR returns the character having the binary equivalent to n as a VARCHAR2
 * value in either the database character set or, if you specify
 * USING NCHAR_CS, the national character set.
 *
 * For single-byte character sets, if n > 256, then Oracle Database returns
 * the binary equivalent of n mod 256. For multibyte character
 * sets, n must resolve to one entire code point.
 * Invalid code points are not validated, and the result of
 * specifying invalid code points is indeterminate.
 *
 * This function takes as an argument a NUMBER
 * value, or any value that can be implicitly
 * converted to NUMBER, and returns a character.
 *
 * Implementation detail: The validity of the UTF-8 code IS validated.
 * If the code is invalid, the function returns NULL.
 */
extern void ora_chr(sqlite3_context *context,
                    int              argc,
                    sqlite3_value  **argv) {
   long         code;
   unsigned char utf8[5];

   if (ksu_prm_ok(context, argc, argv, "chr", KSU_PRM_INT_GE_0)) {
      // Check whether the value is actually a number
      code = (long)sqlite3_value_int64(argv[0]);
      if (code == 0) {
         sqlite3_result_null(context);
      } else   {
         sqlite3_result_text(context,
                  (char *)ksu_decimal_to_utf8((const unsigned int)code,
                                              utf8),
                             -1,
                             SQLITE_TRANSIENT);
      }
   }
}
