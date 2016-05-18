#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

extern void my_floor(sqlite3_context * context,
                     int               argc,
                     sqlite3_value  ** argv) {
   int     valueType = sqlite3_value_type(argv[0]);
   double  result;
        
   if (valueType == SQLITE_NULL) {
     sqlite3_result_null(context);
   } else {
     if (valueType != SQLITE_INTEGER && valueType != SQLITE_FLOAT) {
       sqlite3_result_int(context, 0);
     } else {
       result = floor(sqlite3_value_double(argv[0]));
       sqlite3_result_int64(context, (long)result);
     }
   }
}
