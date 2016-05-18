#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_my.h>

#define TZ_LEN    6

#define _abs(x) (x < 0 ? -1 *x : x)

extern void my_convert_tz(sqlite3_context * context, 
                          int               argc, 
                          sqlite3_value  ** argv) {
        KSU_TM_T    time_str;
        KSU_TM_T   *bdp; 
        KSU_TIME_T  clock;
        int         tzlen;
        int         from_tz_hour;
        int         from_tz_minute;
        int         to_tz_hour;
        int         to_tz_minute;
        char       *q;
        char        date[DATE_LEN];
        int         secdiff;
        
        _ksu_null_if_null_param(argc, argv);
        if (my_is_datetime((const char *)sqlite3_value_text(argv[0]),
                             &clock, (char)0)) {
          q = (char *)sqlite3_value_text(argv[1]);
          tzlen = strlen(q);
          if ((tzlen != TZ_LEN)
              || (sscanf(q, "%d:%d",
                            &from_tz_hour, &from_tz_minute) != 2)) {
            ksu_err_msg(context, KSU_ERR_INV_TZ_FORMAT, "convert_tz");
            return; 
          } 
          if (from_tz_hour < 0) {
            from_tz_minute = -1 * _abs(from_tz_minute);
          }
          q = (char *)sqlite3_value_text(argv[2]);
          tzlen = strlen(q);
          if ((tzlen != TZ_LEN) 
              ||(sscanf(q, "%d:%d", &to_tz_hour, &to_tz_minute) != 2)) {
            ksu_err_msg(context, KSU_ERR_INV_TZ_FORMAT, "convert_tz");
            return;
          } 
          if (to_tz_hour < 0) {
            to_tz_minute = -1 * _abs(to_tz_minute);
          }
          secdiff = (to_tz_hour - from_tz_hour) * 3600
                    + (to_tz_minute - from_tz_minute) * 60;
          clock = ksu_add_secs(clock, secdiff);
          bdp = ksu_localtime(clock, &time_str);
          if (bdp) {
            sprintf(date, "%4d-%02d-%02d %02d:%02d:%02d",
                          (int)bdp->year,
                          (int)(1 + bdp->mon),
                          (int)bdp->mday, (int)bdp->hour,
                          (int)bdp->min, (int)bdp->sec);
            sqlite3_result_text(context, date, -1, SQLITE_TRANSIENT);
          } else {
            sqlite3_result_null(context);
          } 
        } else {
          sqlite3_result_null(context);
        }
} 
