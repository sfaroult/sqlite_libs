#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <errno.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_pg.h>

extern void pg_date_trunc(sqlite3_context  *context,
                          int argc,
                          sqlite3_value   **argv) {
   char       *date;
   KSU_TIME_T  t;
   KSU_TM_T    my_time;
   KSU_TM_T   *ptm;
   char       *format;
   int         format_code;
   char       *result;
   char        neg = 0;

   if (ksu_prm_ok(context, argc, argv, "date_trunc",
                  KSU_PRM_TEXT, KSU_PRM_DATETIME)) {
     date = (char *)sqlite3_value_text(argv[1]);
     if (!ksu_is_datetime((const char *)date, &t, (char)0)
        || ((ptm = ksu_localtime(t, &my_time)) == (KSU_TM_T *)NULL)) {
        ksu_err_msg(context, KSU_ERR_INV_DATE, date, "date_trunc");
        return;
     }
     format = (char *)sqlite3_value_text(argv[0]);
     format_code = pgtim_search(format);
     switch (format_code) {
        // Invalid format -> error
        case PGTIM_NOT_FOUND:
             ksu_err_msg(context, KSU_ERR_INV_FORMAT, format, "date_trunc");
             return;
        case PGTIM_MILLENNIUM:
             ptm->sec = 0;
             ptm->min = 0;
             ptm->hour = 0;
             ptm->mday = 1;
             ptm->mon = 0;
             if (ptm->year < 0) {
                ptm->year *= -1;
                neg = 1;
             }
             if ((ptm->year % 1000) != 1) {
                ptm->year = (ptm->year / 1000) * 1000 + 1;
             }
             if (neg) {
                ptm->year *= -1;
             }
             break;
        case PGTIM_CENTURY:
             ptm->sec = 0;
             ptm->min = 0;
             ptm->hour = 0;
             ptm->mday = 1;
             ptm->mon = 0;
             if (ptm->year < 0) {
                ptm->year *= -1;
                neg = 1;
             }
             if ((ptm->year % 100) != 1) {
                ptm->year = (ptm->year / 100) * 100 + 1;
             }
             if (neg) {
                ptm->year *= -1;
             }
             break;
        case PGTIM_DECADE:
             ptm->sec = 0;
             ptm->min = 0;
             ptm->hour = 0;
             ptm->mday = 1;
             ptm->mon = 0;
             if (ptm->year < 0) {
                ptm->year *= -1;
                neg = 1;
             }
             if ((ptm->year % 10) != 1) {
                ptm->year = (ptm->year / 10) * 10 + 1;
             }
             if (neg) {
                ptm->year *= -1;
             }
             break;
        case PGTIM_YEAR:
             ptm->sec = 0;
             ptm->min = 0;
             ptm->hour = 0;
             ptm->mday = 1;
             ptm->mon = 0;
             break;
        case PGTIM_QUARTER:
             ptm->sec = 0;
             ptm->min = 0;
             ptm->hour = 0;
             ptm->mday = 1;
             ptm->mon = (ptm->mon/3) * 3;
             break;
        case PGTIM_MONTH:
             ptm->sec = 0;
             ptm->min = 0;
             ptm->hour = 0;
             ptm->mday = 1;
             break;
        case PGTIM_WEEK:
             // Return the date of the preceding Monday
             if (ptm->wday != 1) {
               // If not a Monday ... Sunday is 0, beware
               t.jdn -= (ptm->wday == 0 ? 6 : (ptm->wday - 1));
               // Recompute
               ptm = ksu_localtime(t, &my_time);
               if (ptm == (KSU_TM_T *)NULL) {
                 // Not supposed to happen
                 ksu_err_msg(context, KSU_ERR_CONV, "date_trunc");
                 return;
               }
             }
             ptm->sec = 0;
             ptm->min = 0;
             ptm->hour = 0;
             break;
        // Let's cascade through here
        case PGTIM_MIN:
             ptm->sec = 0;
        case PGTIM_HOUR:
             ptm->min = 0;
        case PGTIM_DAY:
             ptm->hour = 0;
             break;
        case PGTIM_SECOND:
        case PGTIM_MILLISECONDS:
        case PGTIM_MICROSECONDS:
             // Do nothing
             break;
        default: 
             ksu_err_msg(context, KSU_ERR_UNSUP_FORMAT,
                         format, "date_trunc");
             return;
      }
      result = (char *)sqlite3_malloc(DATE_LEN);
      sprintf(result, "%4d-%02d-%02d %02d:%02d:%02d",
                      ptm->year, 1+ptm->mon, ptm->mday,
                      ptm->hour, ptm->min, ptm->sec);
      sqlite3_result_text(context, result, -1, sqlite3_free);
   }
}

