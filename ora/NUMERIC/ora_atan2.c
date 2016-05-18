#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void     ora_atan2(sqlite3_context * context,
                          int argc,
                          sqlite3_value ** argv) {
        double          val1;
        double          val2;

        if (ksu_prm_ok(context, argc, argv, "atan",
                       KSU_PRM_NUMERIC, KSU_PRM_NUMERIC)) {
           val1 = sqlite3_value_double(argv[0]);
           val2 = sqlite3_value_double(argv[1]);
           sqlite3_result_double(context, atan2(val1, val2));
        }
}
