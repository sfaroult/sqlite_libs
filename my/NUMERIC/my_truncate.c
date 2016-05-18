#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

#define BUFFER_LEN   50

extern void my_truncate(sqlite3_context * context,
                        int               argc,
                        sqlite3_value  ** argv) {
    double   val;
    int      typ;
    int      rounding;
    char     buffer[BUFFER_LEN];

    _ksu_null_if_null_param(argc, argv);
    typ = sqlite3_value_type(argv[0]);
    if ((typ != SQLITE_INTEGER)
        && (typ != SQLITE_FLOAT)) {
      sqlite3_result_int(context, 0);
      return;
    }
    val = sqlite3_value_double(argv[0]);
    typ = sqlite3_value_type(argv[1]);
    if ((typ != SQLITE_INTEGER)
        && (typ != SQLITE_FLOAT)) {
      rounding = 0;
    } else {
      if (typ == SQLITE_FLOAT) {
        rounding = (int)(sqlite3_value_double(argv[1]) + 0.5);
      } else {
        rounding = sqlite3_value_int(argv[1]);
      }
    }
    if (rounding == 0) {
      sqlite3_result_int64(context, (long)floor(val));
    } else if (rounding > 0) {
      // Cheat - SQLite stores everything as text anyhow
      val = trunc(val * pow(10.0, (double)rounding))
            / pow(10.0, (double)rounding);
      sprintf(buffer, "%.*lf", rounding, val);
      sqlite3_result_text(context, buffer, -1, SQLITE_TRANSIENT);
    } else { // Negative rounding
      val = trunc(val * pow(10.0, (double)rounding))
            / pow(10.0, (double)rounding);
      sqlite3_result_int64(context, (long)floor(val));
    }
}
