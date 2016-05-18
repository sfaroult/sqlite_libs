#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

extern void my_pow(sqlite3_context * context,
                   int               argc,
                   sqlite3_value  ** argv) {
   int     typ1;
   int     typ2;
   double  m;
   double  n;
   double  result = 0;

   typ1 = sqlite3_value_type(argv[0]);
   typ2 = sqlite3_value_type(argv[1]);
   if ((typ1 == SQLITE_NULL) || (typ2 == SQLITE_NULL)) {
     sqlite3_result_null(context);
     return;
   }
   if (!(typ2 == SQLITE_INTEGER || typ2 == SQLITE_FLOAT)) {
     sqlite3_result_int(context, 1);
     return;
   }
   n = sqlite3_value_double(argv[1]);
   if (!(typ1 == SQLITE_INTEGER || typ1 == SQLITE_FLOAT)) {
     if (n < 0) {
       sqlite3_result_null(context);
     } else if (n == (double)0) {
       sqlite3_result_int(context, 1);
     } else {
       sqlite3_result_int(context, 0);
     }
     return;
   }
   if (n == (double)0) {
     sqlite3_result_int(context, 1);
     return;
   }
   m = sqlite3_value_double(argv[0]);
   if (m == (double)0) {
     if (n < (double)0) {
       sqlite3_result_null(context);
     } else {
       sqlite3_result_int(context, 0);
     }
     return;
   }
   if (m < (double)0) {
     if ((long)n != n) {
       sqlite3_result_null(context);
     } else {
       result = pow(m,n);
     }
   } else {
     result = pow(m,n);
   }
   if ((long)result == result) {
     sqlite3_result_int64(context, (long)result);
   } else {
     sqlite3_result_double(context, result);
   }
}
