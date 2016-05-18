#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

extern void my_sqrt(sqlite3_context * context,
                    int               argc,
                    sqlite3_value  ** argv) {
    double  val;
    int     typ;

    _ksu_null_if_null_param(argc, argv);
    typ = sqlite3_value_type(argv[0]);
    if ((typ == SQLITE_INTEGER) 
        || (typ == SQLITE_FLOAT)) {
      val = sqlite3_value_double(argv[0]);
      if (val >= 0) {
        val = sqrt(val);
        if ((long)val == val) {
          sqlite3_result_int64(context, (long)val);
        } else {
          sqlite3_result_double(context, val);
        }
      } else {
        sqlite3_result_null(context);
      }
    } else {
      sqlite3_result_int(context, 0);
    }
}
