#include <stdio.h>

#include <ksu_common.h>

extern void pg_chr(sqlite3_context *context,
                   int              argc,
                   sqlite3_value  **argv) {
   long          code;
   unsigned char utf8[5];

   // 0 not accepted
   if (ksu_prm_ok(context, argc, argv, "chr", KSU_PRM_INT_GT_0)) {
      // Check whether the value is actually a number
      code = (long)sqlite3_value_int64(argv[0]);
      sqlite3_result_text(context,
                  (char *)ksu_decimal_to_utf8((const unsigned int)code,
                                              utf8),
                             -1,
                             SQLITE_TRANSIENT);
   }
}
