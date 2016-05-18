#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void     ora_log(sqlite3_context * context,
                        int               argc,
                        sqlite3_value  ** argv) {
        double  val;
        int     b;

        if (ksu_prm_ok(context, argc, argv, "log",
                       KSU_PRM_INT_GT_0,
                       KSU_PRM_NUM_GT_0)) {
           b = sqlite3_value_int(argv[0]);
           if (b == 1) {
              // Invalid base
              ksu_err_msg(context, KSU_ERR_INV_LOG_BASE);
           } else {
              val = sqlite3_value_double(argv[1]);
              sqlite3_result_double(context, log(val) / log((double)b));
           }
        }
}
