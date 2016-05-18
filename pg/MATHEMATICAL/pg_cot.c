#include <stdio.h>
#include <math.h>

#include <ksu_common.h>

/*
 * Returns the cotangent of argument (radians)
 */
extern void pg_cot(sqlite3_context * context,
                   int               argc,
                   sqlite3_value  ** argv) {
        double  val;

        if (ksu_prm_ok(context, argc, argv, "cot", KSU_PRM_NUMERIC)) {
           val = sqlite3_value_double(argv[0]);
           if (val == (double)0) {
             sqlite3_result_text(context, "Infinity", -1, SQLITE_STATIC);
           } else {
             sqlite3_result_double(context, 1.0/tan(val));
           }
        }
}
