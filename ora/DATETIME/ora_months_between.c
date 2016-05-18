#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#include <ksu_common.h>
#include <ksu_dates.h>

static int check_last_day(int mon, KSU_TIME_T sec) {
    KSU_TM_T  *t;
    KSU_TM_T   mydate;
                
    sec.jdn += 1;
    t = ksu_localtime(sec, &mydate);
    if (t && (t->mon != mon)) {
      return 1;
    } else {
      return 0;
    } 
} 

/*
 *  months_between takes two dates (date1 and date2) and returns number of
 * months between dates date1 and date2. If date1 is later than date2, then
 * the result is positive. If date1 is earlier than date2, then the result
 * is negative. If date1 and date2 are either the same days of the month or
 * both last days of months,  then the result is always an integer.
 * Otherwise Oracle Database calculates the  fractional portion of the
 * result based on a 31-day month and considers the  difference in time
 * components date1 and date2.
 */ 
extern void ora_months_between(sqlite3_context * context, 
                               int                argc, 
                               sqlite3_value   ** argv) {
      char      *v1;
      char      *v2;
      KSU_TM_T   date1;
      KSU_TM_T   date2;
      KSU_TIME_T sec1;
      KSU_TIME_T sec2;
        
      if (ksu_prm_ok(context, argc, argv, "months_between",
                     KSU_PRM_DATETIME, KSU_PRM_DATETIME)) {
        v1 = (char *)sqlite3_value_text(argv[0]);
        v2 = (char *)sqlite3_value_text(argv[1]);
        if (!ksu_is_datetime((const char *)v1, &sec1, (char)0)
            || !ksu_localtime(sec1, &date1)) {
          ksu_err_msg(context, KSU_ERR_INV_DATE,
                      v1, "months_between");
          return;
        }
        if (!ksu_is_datetime((const char *)v2, &sec2, (char)0)
            || !ksu_localtime(sec2, &date2)) {
          ksu_err_msg(context, KSU_ERR_INV_DATE,
                      v2, "months_between");
          return;
        }
        
        if ((date1.mday == date2.mday) 
           ||(check_last_day(date1.mon, sec1) 
              && check_last_day(date2.mon, sec2))) {
           //integer result 
           int m = (int)((double)(sec1.jdn - sec2.jdn) / 31 + 0.5);
           sqlite3_result_int(context, m);
        } else {
           double m = (double)(sec1.jdn - sec2.jdn) / 31;
           sqlite3_result_double(context, m);
        } 
      } 
} 
