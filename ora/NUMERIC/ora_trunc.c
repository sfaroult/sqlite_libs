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
#include <ksu_ora.h>

extern void ora_trunc(sqlite3_context  *context,
                      int argc,
                      sqlite3_value   **argv) {
   int         typ;
   int         dlen;    // date length
   char       *date;
   KSU_TIME_T  t;
   KSU_TM_T    my_time;
   KSU_TM_T   *ptm;
   char       *format;
   int         format_code;
   char       *result;
   double      val;
   double      rval;
   int         rvalue = 0;
   char        neg = 0;
   char        isodate[DATE_LEN];
   int         isoday;
   int         isoweek;
   int         isoyear;

   _ksu_check_arg_cnt(argc, 1, 2, "trunc");
   // Check whether at least one parameter is null
   if (((typ = sqlite3_value_type(argv[0])) == SQLITE_NULL)
        || ((argc == 2) && (sqlite3_value_type(argv[1]) == SQLITE_NULL))) {
      sqlite3_result_null(context);
      return;
   }
   // Check whether we have a number or a date
   switch (typ) {
       case SQLITE_INTEGER :
       case SQLITE_FLOAT :
            if (argc == 2) {
               if (sqlite3_value_type(argv[1]) != SQLITE_INTEGER) {
                  ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 2, "trunc");
                  return;
               }
               rvalue = sqlite3_value_int(argv[1]);
            }
            val = sqlite3_value_double(argv[0]);
            if (val == (double)0) {
              sqlite3_result_int(context, 0);
            } else {
              if (val < 0) {
                neg = 1;
                val *= -1;
              }
              // Based on the formula given in the Oracle docs
              // for the round() function (plus sign handling)
              rval = floor(val * pow(10, rvalue))
                     * pow(10, -rvalue) * (neg ? -1 : 1);
              if (rval == (long)rval) {
                sqlite3_result_int(context, (long)rval);
              } else {
                sqlite3_result_double(context, rval);
              }
            }
            break;
       default :
            date = (char *)sqlite3_value_text(argv[0]);
            if (!ksu_is_datetime((const char *)date, &t, (char)0)
               || ((ptm = ksu_localtime(t, &my_time)) == (KSU_TM_T *)NULL)) {
              ksu_err_msg(context, KSU_ERR_INV_DATE, date, "trunc");
              return;
            }
            dlen = strlen(date);
            format = (char *)sqlite3_value_text(argv[1]);
            format_code = oratim_search(format);
            switch (format_code) {
              // Invalid format -> error
              case ORATIM_NOT_FOUND:
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, format, "trunc");
                   return;
              case ORATIM_CC:
              case ORATIM_SCC:
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
              case ORATIM_SYYYY:
              case ORATIM_YYYY:
              case ORATIM_YYY:
              case ORATIM_YY:
              case ORATIM_Y:
              case ORATIM_YEAR:
              case ORATIM_SYEAR:
                   ptm->sec = 0;
                   ptm->min = 0;
                   ptm->hour = 0;
                   ptm->mday = 1;
                   ptm->mon = 0;
                   break;
              case ORATIM_IYYY:
              case ORATIM_IY:
              case ORATIM_I:
                   if (ksu_iso_date(t, isodate)
                       && (sscanf(isodate, "%d-W%d-%d",
                                  &isoyear, &isoweek, &isoday) == 2)) {
                      // Return result now
                      sprintf(isodate, "%4d-W01-1", isoyear);
                      sqlite3_result_text(context,
                                          isodate, -1, SQLITE_TRANSIENT);
                      return; 
                   } else {
                     ksu_err_msg(context, KSU_ERR_INV_DATE, date, "trunc");
                     return;
                   }
                   break;
              case ORATIM_Q:
                   ptm->sec = 0;
                   ptm->min = 0;
                   ptm->hour = 0;
                   ptm->mday = 1;
                   ptm->mon = (ptm->mon/3) * 3;
                   break;
              case ORATIM_MONTH:
              case ORATIM_MON:
              case ORATIM_MM:
              case ORATIM_RM:
                   ptm->sec = 0;
                   ptm->min = 0;
                   ptm->hour = 0;
                   ptm->mday = 1;
                   break;
              case ORATIM_DDD:
              case ORATIM_DD:
              case ORATIM_J:
                   // Same as no format : set time to 00:00:00
                   ptm->sec = 0;
                   ptm->min = 0;
                   ptm->hour = 0;
                   break;
              default: 
                   ksu_err_msg(context, KSU_ERR_UNSUP_FORMAT,
                               format, "trunc");
                   return;
            }
            result = (char *)sqlite3_malloc(dlen + 1);
            // Return the same type of date as input
            if (dlen > 15) {   // May be 19 or 20 if there is a minus sign
               sprintf(result, "%4d-%02d-%02d %02d:%02d:%02d",
                               ptm->year, 1+ptm->mon, ptm->mday,
                               ptm->hour, ptm->min, ptm->sec);
            } else {
               sprintf(result, "%4d-%02d-%02d",
                               ptm->year, 1+ptm->mon, ptm->mday);
            }
            sqlite3_result_text(context, result, -1, sqlite3_free);

            break;
   }
}

