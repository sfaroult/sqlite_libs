#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>

extern void ora_month(sqlite3_context * context,
                      int               argc,
                      sqlite3_value  ** argv) {
        char  *arg1;
        int    y;
        int    m;

        if (ksu_prm_ok(context, argc, argv, "month", KSU_PRM_DATETIME)) {
          arg1 = (char *)sqlite3_value_text(argv[0]);
          if (2 == sscanf(arg1, "%d-%d", &y, &m)) {
            sqlite3_result_int(context, m);
          } else {
            // Should not happen
            ksu_err_msg(context, KSU_ERR_INV_DATE, arg1, "month");
          }
        }
}
