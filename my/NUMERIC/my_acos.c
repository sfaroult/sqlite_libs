#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ksu_common.h>
#include <ksu_my.h>

extern void my_acos(sqlite3_context * context,
                    int               argc,
                    sqlite3_value  ** argv) {
   double  val;
   char   *v;

   v = (char *)sqlite3_value_text(argv[0]);

   if (v == NULL) {
     sqlite3_result_null(context);
   } else {
     //  if  the value isn't numeric mysql throws a warning
     //  (we can't) but returns rubbish (we can too)
     val = my_value_double(argv[0], 0);
     if ((val < -1) || (val > 1)) {
       sqlite3_result_null(context); // That's what MySQL does
     } else {
       sqlite3_result_double(context, acos(val));
     }
   }
}
