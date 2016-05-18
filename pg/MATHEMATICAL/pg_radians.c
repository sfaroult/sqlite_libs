/*
 * pg_radians.c
 * 
 */
#include <stdio.h>
#include <math.h>
#include <ksu_common.h>

/*
 * Converts a number from degrees to radians.
 */

extern void pg_radians(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
   double  x;
        
   if (ksu_prm_ok(context, argc, argv, "radians", KSU_PRM_NUMERIC)) {
     x = sqlite3_value_double(argv[0]);
     sqlite3_result_double(context, ((x * M_PI) / (double)180));
   }
}
