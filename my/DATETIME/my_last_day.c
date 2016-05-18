#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_my.h>

extern void     my_last_day(sqlite3_context * context, 
                            int               argc, 
                            sqlite3_value  ** argv) {
   KSU_TIME_T  t;
   KSU_TM_T    tm;
   KSU_TM_T   *ptm;
   char        date[DATE_LEN];
                
   _ksu_null_if_null_param(argc, argv);
   if (my_is_datetime((const char *)sqlite3_value_text(argv[0]), &t, 0)) {
     if ((ptm = ksu_localtime(t, &tm)) != (KSU_TM_T *)NULL) {
       // Compute the date for the first of the given month
       tm.mday = 1;
       // Add one month
       t = ksu_add_months(ptm, 1);
       // Remove one day
       t.jdn--;
       // Compute month, day etc.
       if ((ptm = ksu_localtime(t, &tm)) != (KSU_TM_T *)NULL) {
         (void)sprintf((char *)date, "%4d-%02d-%02d",
                       tm.year, 1 + tm.mon, tm.mday);
         sqlite3_result_text(context, date, -1, SQLITE_TRANSIENT);
       } else {
         sqlite3_result_null(context);
       }
     } else {
       sqlite3_result_null(context);
     }
   } else {
     sqlite3_result_null(context);
   }
} 
