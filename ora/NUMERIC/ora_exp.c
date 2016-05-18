#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

#define e 2.71828183
/*
 *   Returns the position of the second argument
 *   in the first one, 0 if not found.
 */
extern void ora_exp(sqlite3_context  *context,
                    int argc,
                    sqlite3_value   **argv) {
   double val;

   if (ksu_prm_ok(context, argc, argv, "asin", KSU_PRM_NUMERIC)) {
        val = sqlite3_value_double(argv[0]);
        sqlite3_result_double(context, pow(e, val));
   }
}
