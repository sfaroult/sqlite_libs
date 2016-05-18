#include <ksu_common.h>
#include <ksu_my.h>

extern void my_addtime(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
  _ksu_null_if_null_param(argc, argv);
  my_time_op(context, "addtime", (char)1,
             (char *)sqlite3_value_text(argv[0]),
             (char *)sqlite3_value_text(argv[1]));
}
