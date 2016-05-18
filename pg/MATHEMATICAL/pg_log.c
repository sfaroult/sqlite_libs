#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void  pg_log(sqlite3_context * context,
                    int               argc,
                    sqlite3_value  ** argv) {
        double  val;
        double  base;

        _ksu_check_arg_cnt(argc, 1, 2, "log");
        if (argc == 2) {
          base = sqlite3_value_double(argv[0]);
          if ((base <= 0) || (base == 1)) {
            ksu_err_msg(context,  KSU_ERR_INV_ARG_VAL_RANGE, "log");
            return;
          }
          val = sqlite3_value_double(argv[1]);
        } else {
          base = 10;
          val = sqlite3_value_double(argv[0]);
        }
        sqlite3_result_double(context, log(val) / log(base));
}
