#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

extern void my_tan(sqlite3_context * context,
                   int               argc,
                   sqlite3_value  ** argv) {
   double   val;
   char    *v;

   _ksu_null_if_null_param(argc, argv);
   //Check whether the value is actually a number
   v = (char *)sqlite3_value_text(argv[0]);
   if (sscanf(v, "%lf", &val) == 1) {
     val = tan(val);
     if ((long)val == val) {
       sqlite3_result_int64(context, (long)val);
     } else {
       sqlite3_result_double(context, val);
     }
   } else {
     //Wrong input
     sqlite3_result_int(context, 0);
   }
}
