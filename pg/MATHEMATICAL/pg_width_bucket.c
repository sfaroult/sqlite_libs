#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ksu_common.h>

extern void  pg_width_bucket(sqlite3_context * context,
                             int               argc,
                             sqlite3_value  ** argv) {
        double          exp;
        double          min;
        double          max;
        double          count;
        double          width;
        int             bc = 1;
        double          i;

        _ksu_null_if_null_param(argc, argv);
        // exp, min and max must be numeric, 
        // count must be a positive integer.
        if (ksu_prm_ok(context, argc, argv, "width_bucket",
                       KSU_PRM_NUMERIC, KSU_PRM_NUMERIC,
                       KSU_PRM_NUMERIC, KSU_PRM_INT_GT_0)) {
          exp = sqlite3_value_double(argv[0]);
          min = sqlite3_value_double(argv[1]);
          max = sqlite3_value_double(argv[2]);
          count = sqlite3_value_int(argv[3]);
          width = (max - min) / count;

          if (exp < min) {
            sqlite3_result_int(context, 0);
          } else if (exp > max) {
            sqlite3_result_int(context, count + 1);
          } else {
             i = min;
             while (i < max) {
              if (exp >= i && exp < i + width) {
                sqlite3_result_int(context, bc);
                break;
              }
              i += width;
              bc++;
           }
        }
      }
}
