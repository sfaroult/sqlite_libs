#include <ksu_common.h>
#include <ksu_my.h>
#include <ksu_dates.h>

extern void     my_date_sub(sqlite3_context * context, 
                            int               argc, 
                            sqlite3_value  ** argv) {
   my_date_op(context, "date_sub",
              (char *)sqlite3_value_text(argv[0]),
              (char)0,
              (char *)sqlite3_value_text(argv[1]));
}
