/*
 * pg_div.c
 * 
 */
#include <stdio.h>
#include <math.h>
#include <ksu_common.h>

extern void pg_div(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
   double  x;
   double  y;
   long    ret;
        
   if (ksu_prm_ok(context, argc, argv, "div",
                  KSU_PRM_NUMERIC, KSU_PRM_NUMERIC)) {
     x = sqlite3_value_double(argv[0]);
     y = sqlite3_value_double(argv[1]);
     if (y == (double)0) {
       ksu_err_msg(context, KSU_ERR_DIV_ZERO, "div");
       return;
     }
     ret = (long)x/y;
     sqlite3_result_int(context, ret);
   }
}
