#include <stdio.h>
#include <stdlib.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_my.h>

extern void  my_dayofweek(sqlite3_context * context,
                          int               argc,
                          sqlite3_value ** argv) {
   KSU_TIME_T  t;
   KSU_TM_T   *tmptr;
   KSU_TM_T    mydate;

   _ksu_null_if_null_param(argc, argv);
   if (my_is_datetime((const char *)sqlite3_value_text(argv[0]), &t, 0)
       && ((tmptr = ksu_localtime(t, &mydate)) != (KSU_TM_T *)NULL)) {
     sqlite3_result_int(context, 1 + (int)tmptr->wday);
   } else {
     sqlite3_result_null(context);
   }
}
