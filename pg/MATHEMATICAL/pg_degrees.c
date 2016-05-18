/*
 * pg_degrees.c
 * 
 * This program is used in SQLite and will convert a number from radians to
 * degrees
 * 
 * Input - A number in radians Output - The number in degrees
 * 
 */
#include <stdio.h>
#include <math.h>
#include <ksu_common.h>

/*
 * Converts a number from radians to degrees.
 */

extern void pg_degrees(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
   double  x;
        
   if (ksu_prm_ok(context, argc, argv, "degrees", KSU_PRM_NUMERIC)) {
     x = sqlite3_value_double(argv[0]);
     sqlite3_result_double(context, ((x * 180) / M_PI));
   }
}
