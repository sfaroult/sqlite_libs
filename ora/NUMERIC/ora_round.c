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

extern void ora_round(sqlite3_context  *context,
                      int argc,
                      sqlite3_value   **argv) {
   int         typ;
   int         dlen;    // date length
   char       *date;
   short       first_month;
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

   _ksu_check_arg_cnt(argc, 1, 2, "round");
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
                  ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 2, "round");
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
              // The following is the formula provided 
              // in the Oracle docs (plus sign handling)
              rval = floor(val * pow(10, rvalue) + 0.5)
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
              ksu_err_msg(context, KSU_ERR_INV_DATE, date, "round");
              return;
            }
            dlen = strlen(date);
            format = (char *)sqlite3_value_text(argv[1]);
            format_code = oratim_search(format);
            switch (format_code) {
              // Invalid format -> error
              case ORATIM_NOT_FOUND:
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, format, "round");
                   return;
              case ORATIM_CC:
              case ORATIM_SCC:
                   // Round up on January 1st of the 50th year
                   // Rounding to the century returns the date and time
                   // at which the century begins. For example, the 21st
                   // century began in 2001, not 2000.
                   ptm->sec = 0;
                   ptm->min = 0;
                   ptm->hour = 0;
                   ptm->mday = 1;
                   ptm->mon = 0;
                   neg = 0;
                   if (ptm->year < 0) {
                     ptm->year *= -1;
                     neg = 1;
                   }
                   if ((ptm->year % 100) >= 50) {
                     ptm->year = (ptm->year / 100) * 100 + 101;
                   } else {
                     if ((ptm->year % 100) == 0) {
                       // Round up 
                       (ptm->year)++;
                     } else {
                       // Round down
                       ptm->year = (ptm->year / 100) * 100 + 1;
                     }
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
                   // Round up on July 1st
                   if (ptm->mon >= 6) {
                     (ptm->year)++;
                     if (ptm->year == 0) {
                       ptm->year = 1;
                     }
                   }
                   ptm->mon = 0;
                   break;
              case ORATIM_IYYY:
              case ORATIM_IY:
              case ORATIM_I:
                   // Beware of ISO
                   // An ISO year has 52 or 53 full weeks.
                   // We round up from the 27th week on for short
                   // years, and from the Thursday of the 27th
                   // week for long years.
                   // Sample iso date: 2009-W01-1 (1 = Monday, 7 = Sunday)
                   if (ksu_iso_date(t, isodate)
                       && (sscanf(isodate, "%d-W%d-%d",
                                  &isoyear, &isoweek, &isoday) == 2)) {
                      if (isoweek > 27) {
                         isoyear++;
                      } else {
                        if (isoweek == 27) {
                          if (ksu_long_iso_year(isoyear)) {
                            if (isoday >= 4) {
                               isoyear++;
                            }
                          } else {
                            isoyear++;
                          }
                        }
                      }
                      // Return result now
                      sprintf(isodate, "%4d-W01-1", isoyear);
                      sqlite3_result_text(context,
                                          isodate, -1, SQLITE_TRANSIENT);
                      return; 
                   } else {
                     ksu_err_msg(context, KSU_ERR_INV_DATE, date, "round");
                     return;
                   }
                   break;
              case ORATIM_Q:
                   // Rounds up on the 16th day of the second month
                   // of the quarter
                   // Jan, Feb, Mar => Jan 1st
                   // Apr, May, Jun => Apr 1st
                   // Jul, Aug, Sep => Jul 1st
                   // Oct, Nov, Dec => Oct 1st
                   ptm->sec = 0;
                   ptm->min = 0;
                   ptm->hour = 0;
                   ptm->mday = 1;
                   first_month = (ptm->mon/3) * 3;
                   if ((ptm->mon == (first_month + 2))
                       || ((ptm->mon == (first_month + 1))
                           && (ptm->mday >= 16))) {
                      // Round up
                      first_month += 3;
                      if (first_month > 11) {
                        first_month = 0;
                        (ptm->year)++;
                        if (ptm->year == 0) {
                          ptm->year = 1;
                        }
                      }
                   }
                   ptm->mon = first_month;
                   break;
              case ORATIM_MONTH:
              case ORATIM_MON:
              case ORATIM_MM:
              case ORATIM_RM:
                   // Rounds up on the 16th day of the month
                   // Set the date to the first of the month
                   ptm->sec = 0;
                   ptm->min = 0;
                   ptm->hour = 0;
                   if (ptm->mday >= 16) {
                     (ptm->mon)++;
                     if (ptm->mon > 11) {
                        ptm->mon = 0;
                        (ptm->year)++;
                        if (ptm->year == 0) {
                          ptm->year = 1;
                        }
                     }
                   }
                   ptm->mday = 1;
                   break;
              case ORATIM_DDD:
              case ORATIM_DD:
              case ORATIM_J:
                   // Same as no format : set time to 00:00:00
                   ptm->sec = 0;
                   ptm->min = 0;
                   if (ptm->hour >= 12) {
                      // Directly operate on the KSU_TIME_T structure
                      t.jdn++;
                      t.sec = 0;
                      if ((ptm = ksu_localtime(t, &my_time))
                              == (KSU_TM_T *)NULL) {
                        ksu_err_msg(context, KSU_ERR_INV_DATE, date, "round");
                        return;
                      }
                   } else {
                     ptm->hour = 0;
                   }
                   break;
              default: 
                   ksu_err_msg(context, KSU_ERR_UNSUP_FORMAT,
                               format, "round");
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

