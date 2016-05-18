#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

extern void my_ln(sqlite3_context * context,
                  int               argc,
                  sqlite3_value  ** argv) {
  int     valueType = sqlite3_value_type(argv[0]);
  double  result;
        
  if (valueType == SQLITE_NULL) {
    sqlite3_result_null(context);
  } else {
    if (valueType != SQLITE_INTEGER && valueType != SQLITE_FLOAT) {
      sqlite3_result_null(context);
    } else {
      result = sqlite3_value_double(argv[0]);
      if (result <= (double)0) {
        sqlite3_result_null(context);
      } else {
        sqlite3_result_double(context, log(result));
      }
    }
  }
}
