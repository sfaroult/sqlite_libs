#include <stdio.h>
#include <ksu_common.h>
#include <ksu_my.h>

extern void my_mod(sqlite3_context * context,
                  int               argc,
                  sqlite3_value  ** argv) {
  int     a;
  int     b;
       
  _ksu_null_if_null_param(argc, argv);
  a = my_value_int(argv[0], 0);
  b = my_value_int(argv[1], 0);
  if (b == 0) {
    sqlite3_result_null(context);
  } else {
    sqlite3_result_int(context, a % b);
  }
}
