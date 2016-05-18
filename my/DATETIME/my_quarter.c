#include <stdio.h>
#include <stdlib.h>
#include <ksu_common.h>
#include <ksu_dates.h>

extern void     my_quarter(sqlite3_context * context,
                           int               argc,
                           sqlite3_value  ** argv) {

   char      *value;
   int        year;
   int        mon;
   int        day;
   KSU_TIME_T t;

   _ksu_null_if_null_param(argc, argv);
   value = (char *)sqlite3_value_text(argv[0]);
   if (ksu_is_datetime((const char *)value, &t, 0)) {
     if (sscanf(value, "%d-%d-%d", &year, &mon, &day) == 3) {
       sqlite3_result_int(context, 1 + (mon - 1) / 3);
     } else {
       sqlite3_result_null(context);
     }
   } else {
     sqlite3_result_null(context);
   }
}
