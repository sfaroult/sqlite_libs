#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>
#include <math.h>

extern void     ora_rawtohex(sqlite3_context * context, 
                       int               argc, 
                       sqlite3_value  ** argv) {

   unsigned long long  n;
   char               *result;
   char                num = 1;

   _ksu_null_if_null_param(argc, argv);
   switch (sqlite3_value_type(argv[0])) {
     case SQLITE_INTEGER:
          n = (unsigned long long)sqlite3_value_int64(argv[0]);
          break;
     case SQLITE_FLOAT:
          n = (unsigned long long)(0.5 + sqlite3_value_double(argv[0]));
          break;
     default:
          num = 0;
          break;
   }
   if (num) {
     result = (char *)sqlite3_malloc(1 + 2 * sizeof(n));
     if (result == (char *)NULL) {
       sqlite3_result_error_nomem(context);
       return;
     }
     (void)sprintf(result, "%llX", n);
   } else {
     int            len =  sqlite3_value_bytes(argv[0]);
     int            i;
     unsigned char *p = (unsigned char *)sqlite3_value_text(argv[0]);
     char          *r;

     result = (char *)sqlite3_malloc(1 + 2 * sqlite3_value_bytes(argv[0]));
     if (result == (char *)NULL) {
       sqlite3_result_error_nomem(context);
       return;
     }
     r = result;
     for (i = 0; i < len; i++) {
       (void)sprintf(r, "%02X", *p);
       r += 2;
       p++;
     }
   }
   sqlite3_result_text(context, result, -1, sqlite3_free);
}
