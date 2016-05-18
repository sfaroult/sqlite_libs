#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void     my_oct(sqlite3_context * context, 
                       int               argc, 
                       sqlite3_value  ** argv) {

   unsigned long long  n;
   char               *result;

   _ksu_null_if_null_param(argc, argv);
   switch (sqlite3_value_type(argv[0])) {
     case SQLITE_INTEGER:
          n = (unsigned long long)sqlite3_value_int64(argv[0]);
          break;
     case SQLITE_FLOAT:
          n = (unsigned long long)(0.5 + sqlite3_value_double(argv[0]));
          break;
     default:
          n = 0;
          break;
   }
   result = (char *)sqlite3_malloc(1 + 3 * sizeof(n));
   if (result == (char *)NULL) {
     sqlite3_result_error_nomem(context);
     return;
   }
   (void)sprintf(result, "%llo", n);
   sqlite3_result_text(context, result, -1, sqlite3_free);
}
