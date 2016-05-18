#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ksu_common.h>

extern void  pg_to_hex(sqlite3_context * context, 
                       int               argc, 
                       sqlite3_value  ** argv) {

   unsigned long long  n;
   long long           signedn;
   char               *result;

   if (ksu_prm_ok(context, argc, argv, "to_hex", KSU_PRM_INT)) {
     signedn = (long long)sqlite3_value_int64(argv[0]);
     n = (unsigned long long)signedn;
     if (signedn < 0) {
       signedn *= -1;
     }
     result = (char *)sqlite3_malloc(1 + 2 * sizeof(n));
     if (result == (char *)NULL) {
       sqlite3_result_error_nomem(context);
       return;
     }
     if (signedn > INT_MAX) {
       (void)sprintf(result, "%llx", n);
     } else {
       (void)sprintf(result, "%x", (unsigned int)n);
     }
     sqlite3_result_text(context, result, -1, sqlite3_free);
   }
}
