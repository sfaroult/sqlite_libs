#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

extern void my_sign(sqlite3_context * context,
                    int               argc,
                    sqlite3_value  ** argv) {
   double  val;
   char   *v;

   _ksu_null_if_null_param(argc, argv);
   v = (char *)sqlite3_value_text(argv[0]);
   if (sscanf(v, "%lf", &val) == 1) {
     if (val > 0)
       sqlite3_result_int(context, 1);
     else if (val == 0)
       sqlite3_result_int(context, 0);
     else
       sqlite3_result_int(context, -1);
   } else {
     sqlite3_result_int(context, 0);
   }
}
