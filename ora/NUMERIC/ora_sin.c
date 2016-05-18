#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void     ora_sin(sqlite3_context * context,
                        int               argc,
                        sqlite3_value  ** argv) {
        double          val;

        if (ksu_prm_ok(context, argc, argv, "sin", KSU_PRM_NUMERIC)) {
           val = sqlite3_value_double(argv[0]);
           sqlite3_result_double(context, sin(val));
        }
}
