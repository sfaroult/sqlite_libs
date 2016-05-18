#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

/*
 * Rounds the specified number up. Returns the smallest number that is
 * greater than or equal to the specified number.
 * 
 */
extern void pg_ceil(sqlite3_context * context,
                    int               argc,
                    sqlite3_value  ** argv) {
     double val;

     if (ksu_prm_ok(context, argc, argv, "ceil", KSU_PRM_NUMERIC)) {
       val = sqlite3_value_double(argv[0]);
       sqlite3_result_double(context, ceil(val));
     }
}
