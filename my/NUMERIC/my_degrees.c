/*
 * my_degrees.c
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

extern void my_degrees(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
   int     valueType = sqlite3_value_type(argv[0]);
   double  x;
        
   if (valueType == SQLITE_NULL) {
     sqlite3_result_null(context);
   } else {
     if (valueType != SQLITE_INTEGER && valueType != SQLITE_FLOAT) {
       sqlite3_result_int(context, 0);
     } else {
       x = sqlite3_value_double(argv[0]);
       sqlite3_result_double(context, ((x * 180) / M_PI));
     }
   }
}
