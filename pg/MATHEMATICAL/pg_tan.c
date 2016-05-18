#include <stdio.h>
#include <math.h>

#include <ksu_common.h>

/*
 * Returns the tangent of argument (radians)
 */
extern void pg_tan(sqlite3_context * context,
                   int               argc,
                   sqlite3_value  ** argv) {
        double  val;

        if (ksu_prm_ok(context, argc, argv, "tan", KSU_PRM_NUMERIC)) {
           val = sqlite3_value_double(argv[0]);
           sqlite3_result_double(context, tan(val));
        }
}
