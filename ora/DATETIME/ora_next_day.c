#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>
#include <ksu_dates.h>

extern void     ora_next_day(sqlite3_context * context, 
                             int               argc, 
                             sqlite3_value  ** argv) {
        char       *input_date;
        int         wday;
        char        date[DATE_LEN];
        int         delta;
        KSU_TIME_T  t;
        KSU_TM_T    tm;
                
        if (ksu_prm_ok(context, argc, argv,
                       "next_day", KSU_PRM_DATETIME, KSU_PRM_TEXT)) {
          input_date = (char *)sqlite3_value_text(argv[0]);
          wday = ksu_weekday((char *)sqlite3_value_text(argv[1]));
          if (wday == -1) {
            ksu_err_msg(context, KSU_ERR_INV_DAY,
                        (char *)sqlite3_value_text(argv[1]), "next_day");
            return;
          } 
          if (!ksu_is_datetime(input_date, &t, (char)0)
              || !ksu_localtime(t, &tm)) {
            ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                        1, "next_day");
            return;
          } 
          if (tm.wday < wday) {
            delta = (wday - tm.wday) * 86400;
          } else {
            delta = (7 + wday - tm.wday) * 86400;
          }
          t = ksu_add_secs(t, delta);
          if (!ksu_localtime(t, &tm)) {
            ksu_err_msg(context, KSU_ERR_DATE_CONV, "next_day");
            return;
          } 
          if (strlen(input_date) > 15) {
            // Give the time component 
            (void)sprintf((char *)date,
                          "%4hd-%02hd-%02hd %02hd:%02hd:%02hd", 
                          tm.year, (short)(1 + tm.mon), tm.mday,
                          tm.hour, tm.min, tm.sec);
          } else {
            (void)sprintf((char *)date,
                          "%4hd-%02hd-%02hd", 
                          tm.year, (short)(1 + tm.mon), tm.mday);
          }
          sqlite3_result_text(context, date, -1, SQLITE_TRANSIENT);
        } 
} 
