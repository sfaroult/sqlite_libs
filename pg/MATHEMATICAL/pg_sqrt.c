#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ksu_common.h>
#include <math.h>

/*
 *   Returns the position of the second argument
 *   in the first one, 0 if not found.
 */
extern void pg_sqrt(sqlite3_context  *context,
                     int argc,
                     sqlite3_value   **argv) {
   double val;

   if (ksu_prm_ok(context, argc, argv, "sqrt", KSU_PRM_NUM_GE_0)) {
     val = sqlite3_value_double(argv[0]);
     sqlite3_result_double(context, sqrt(val));
   }
}
