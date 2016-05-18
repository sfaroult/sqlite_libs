#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sqlite3.h>
#include <math.h>
#include <ksu_common.h>

extern void  pg_cbrt(sqlite3_context * context,
                     int               argc,
                     sqlite3_value  ** argv) {

    if (ksu_prm_ok(context, argc, argv, "cbrt", KSU_PRM_NUMERIC)) {
      double result = cbrt(sqlite3_value_double(argv[0]));
      sqlite3_result_double(context, result);
    }
}
