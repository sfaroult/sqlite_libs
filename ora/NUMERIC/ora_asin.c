#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void     ora_asin(sqlite3_context * context,
                                         int argc,
                                         sqlite3_value ** argv) {
        double          val;

        if (ksu_prm_ok(context, argc, argv, "asin", KSU_PRM_NUMERIC)) {
           val = sqlite3_value_double(argv[0]);
           if (val <= 1 && val >= -1) {
              sqlite3_result_double(context, asin(val));
           } else {
              ksu_err_msg(context, KSU_ERR_INV_ARG_VAL_RANGE,
                          (char *)sqlite3_value_text(argv[0]),
                          "asin");
           }
        }
}
