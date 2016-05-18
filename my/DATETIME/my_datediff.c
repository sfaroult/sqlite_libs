#include <ksu_common.h>
#include <ksu_my.h>
#include <ksu_dates.h>

extern void     my_datediff(sqlite3_context * context, 
                            int               argc, 
                            sqlite3_value  ** argv) {
   KSU_TIME_T  t1;
   KSU_TIME_T  t2;

   _ksu_null_if_null_param(argc, argv);
   if (my_is_datetime((const char *)sqlite3_value_text(argv[0]), &t1, 0)
       && my_is_datetime((const char *)sqlite3_value_text(argv[1]), &t2, 0)) {
     sqlite3_result_int(context, (int)(t1.jdn - t2.jdn));
   } else {
     sqlite3_result_null(context);
   }
}
