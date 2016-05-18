#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void     ora_ceil(sqlite3_context * context,
                         int                argc,
                         sqlite3_value   ** argv) {
        double          val;
        int             result;

        if (ksu_prm_ok(context, argc, argv, "ceil", KSU_PRM_NUMERIC)) {
           val = sqlite3_value_double(argv[0]);
           if (val - (int)val == 0) {
              result = (int)val;
           } else {
              if (val > 0) {
                 result = (int)val + 1;
              } else {
                 result = (int)val;
              }
           }
           sqlite3_result_int(context, result);
        }
}
