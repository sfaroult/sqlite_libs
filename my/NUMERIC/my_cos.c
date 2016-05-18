#include <stdio.h>
#include <math.h>
#include <ksu_common.h>
#include <ksu_my.h>

extern void     my_cos(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
   int     valueType = sqlite3_value_type(argv[0]);
   double  value;
        
   if (valueType == SQLITE_NULL) {
     sqlite3_result_null(context);
   } else {
     value = my_value_double(argv[0], 0);
     sqlite3_result_double(context, cos(value));
   }
}
