#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

extern void     pg_power(sqlite3_context * context, 
                         int               argc, 
                         sqlite3_value  ** argv) {
        double   v1;
        double   v2;
                
        if (ksu_prm_ok(context, argc, argv, "power",
                       KSU_PRM_NUMERIC, KSU_PRM_NUMERIC)) {
          v1 = sqlite3_value_double(argv[0]);
          v2 = sqlite3_value_double(argv[1]);
          if (v2 == 0) {
            sqlite3_result_int(context, 1);
            return;
          }
          if ((v1 == 0) && (v2 < 0)) {
            // Error
            ksu_err_msg(context, KSU_ERR_ARG_N_NOT_NUM_GT_0, 2, "power");
            return;
          }
          if ((v1 < 0) && (v2 != (int)v2)) {
            // Error
            ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 2, "power");
            return;
          }
          sqlite3_result_double(context, pow(v1, v2));
        } 
} 
