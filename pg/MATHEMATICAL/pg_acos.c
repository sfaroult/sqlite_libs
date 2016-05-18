#include <stdio.h>
#include <math.h>

#include <ksu_common.h>

/*
 * Returns the value of the arccos of val if invalid numerical value is
 * passed, returns an error.
 */
extern void  pg_acos(sqlite3_context * context,
                     int argc,
                     sqlite3_value ** argv) {
        double          val;

        if (ksu_prm_ok(context, argc, argv, "acos",
                       KSU_PRM_NUMERIC)) {
           val = sqlite3_value_double(argv[0]);
           if (val < -1 || val > 1) {
              ksu_err_msg(context, KSU_ERR_INV_ARG_VAL_RANGE,
                          (char *)sqlite3_value_text(argv[0]),
                          "acos");
           } else {
              sqlite3_result_double(context, acos(val));
           }
        }
}
