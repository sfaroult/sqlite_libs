#include <ksu_common.h>
#include <math.h>

extern void my_radians(sqlite3_context * context, 
                       int               argc, 
                       sqlite3_value  ** argv) {
   double val;
   int    typ;

   _ksu_null_if_null_param(argc, argv);
   //Check whether the value is actually a number 
   typ = sqlite3_value_type(argv[0]);
   if ((typ != SQLITE_INTEGER) && (typ != SQLITE_FLOAT)) {
      sqlite3_result_int(context, 0);
   } else {
      val = sqlite3_value_double(argv[0]);
      sqlite3_result_double(context, ((val * M_PI) / 180));
   } 
} 
