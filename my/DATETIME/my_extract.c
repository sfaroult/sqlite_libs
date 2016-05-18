#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_my.h>

#define BUFFER_LEN   100

extern void my_extract(sqlite3_context *context,
                       int              argc,
                       sqlite3_value  **argv) {
   char      *date;
   int        dateLen;
   char      *piece;
   int        pieceToExtract;
   int        year = 0;
   int        month = 0;
   int        day = 0;
   int        minute = 0;
   int        hour = 0;
   int        second = 0;
   KSU_TIME_T t;
   KSU_TM_T   tm;
   KSU_TM_T  *tmp;
   char       date_ok = 0;

   if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
      ksu_err_msg(context, KSU_ERR_SYNTAX_ERROR_NEAR,
                  "NULL", "extract"); 
      return;
   }
   piece = (char *)sqlite3_value_text(argv[0]);
   pieceToExtract = mytim_search(piece);
   if (pieceToExtract == MYTIM_NOT_FOUND) {
      ksu_err_msg(context, KSU_ERR_SYNTAX_ERROR_NEAR,
                  piece, "extract"); 
      return;
   }
   date = (char *)sqlite3_value_text(argv[1]);
   dateLen = strlen(date);
   if (ksu_is_datetime((const char *)date, &t, 0)) {
     date_ok = 1;
     tmp = ksu_localtime(t, &tm);
     year = (int)tmp->year;
     month = 1 + (int)tmp->mon;
     day = (int)tmp->mday;
     hour = (int)tmp->hour;
     minute = (int)tmp->min;
     second = (int)tmp->sec;
     if (!(t.flags && KSU_TIME_DATETIME)) {
       // The MySQL interpretation is rather weird ...
       // It seems to take the year as minutes / seconds
       minute = (int)tmp->year / 100;
       second = (int)tmp->year % 100;
     }
   } else {
     // If the value is an int, we try to interpret it as
     // something suitable
     if (dateLen > 8) {
       date_ok = my_int_datetime(argv[1], &tm);
     } else {
       // Interpretation depends on what we try to extract
       switch (pieceToExtract) {
         case MYTIM_DAY:
         case MYTIM_MONTH:
         case MYTIM_QUARTER:
         case MYTIM_WEEK:
         case MYTIM_YEAR:
         case MYTIM_YEAR_MONTH:
              date_ok = my_int_date(argv[1], &tm);
              break;
         case MYTIM_DAY_MICROSECOND:
         case MYTIM_DAY_HOUR:
         case MYTIM_DAY_MINUTE:
         case MYTIM_DAY_SECOND:
         case MYTIM_HOUR:
         case MYTIM_HOUR_MICROSECOND:
         case MYTIM_MICROSECOND:
         case MYTIM_HOUR_MINUTE:
         case MYTIM_HOUR_SECOND:
         case MYTIM_MINUTE:
         case MYTIM_MINUTE_MICROSECOND:
         case MYTIM_MINUTE_SECOND:
         case MYTIM_SECOND:
         case MYTIM_SECOND_MICROSECOND:
              date_ok = my_int_datetime(argv[1], &tm);
              break;
         default:
              date_ok = 0;
              break;
       }
     }
     if (date_ok) {
       date_ok = 2;
       year = (int)tm.year;
       month = 1 + (int)tm.mon;
       day = (int)tm.mday;
       hour = (int)tm.hour;
       minute = (int)tm.min;
       second = (int)tm.sec;
     } else {
       second = 0;
       minute = 0;
       hour = 0;
       day = 0;
       month = 0;
       year = 0;
     }
   }
   if (!date_ok) {
     sqlite3_result_null(context);
     return;
   }
   // Beware: what is returned when the date isn't one
   // depends on the piece ...
   switch (pieceToExtract) {
     case MYTIM_DAY:
          sqlite3_result_int(context, day);
          break;
     case MYTIM_DAY_HOUR:
          if (!(t.flags && KSU_TIME_DATETIME)) {
            sqlite3_result_int(context, 0);
            return;
          }
          sqlite3_result_int(context, day * 100 + hour);
          break;
     case MYTIM_DAY_MICROSECOND:
          {
           char buf[BUFFER_LEN];

           // Returned as text because too big a number
           sprintf(buf, "%d%02d%02d%02d000000",
                        day, hour, minute, second);
           sqlite3_result_text(context, buf, -1, SQLITE_TRANSIENT);
          }
          break;
     case MYTIM_DAY_MINUTE:
          if (minute > 59) { // May happen if not a datetime
            sqlite3_result_null(context);
            return;
          }
          sqlite3_result_int(context, (int)(minute
                                            + 100 * hour
                                            + 10000 * day));
          break;
     case MYTIM_DAY_SECOND:
          if ((minute > 59) || (second > 59)) { // May happen if not a datetime
            sqlite3_result_null(context);
            return;
          }
          sqlite3_result_int64(context,
                   (sqlite3_int64)(day * 1000000
                                  + 10000 * hour
                                  + 100 * minute
                                  + second));
          break;
     case MYTIM_HOUR:
          sqlite3_result_int(context, hour);
          break;
     case MYTIM_HOUR_MICROSECOND:
          {
           char buf[BUFFER_LEN];

           // Returned as text because too big a number
           sprintf(buf, "%d%02d%02d000000",
                        hour, minute, second);
           sqlite3_result_text(context, buf, -1, SQLITE_TRANSIENT);
          }
          break;
     case MYTIM_MICROSECOND:
          // Return 0 in all cases
          sqlite3_result_int(context, 0);
          break;
     case MYTIM_HOUR_MINUTE:
          sqlite3_result_int(context, (int)(minute
                                            + 100 * hour));
          break;
     case MYTIM_HOUR_SECOND:
          sqlite3_result_int(context, (int)(second
                                            + 100 * minute 
                                            + 10000 * hour));
          break;
     case MYTIM_MINUTE:
          if ((date_ok == 1)
              && !(t.flags && KSU_TIME_DATETIME)) {
            // That's what MySQl does
            sqlite3_result_int(context, (int)(year / 100));
            return;
          }
          sqlite3_result_int(context, minute);
          break;
     case MYTIM_MINUTE_MICROSECOND:
          {
           char buf[BUFFER_LEN];

           // Returned as text because too big a number
           sprintf(buf, "%d%02d000000", minute, second);
           sqlite3_result_text(context, buf, -1, SQLITE_TRANSIENT);
          }
          break;
     case MYTIM_MINUTE_SECOND:
          sqlite3_result_int(context, (int)(second
                                            + 100 * minute));
          break;
     case MYTIM_MONTH:
          sqlite3_result_int(context, month); 
          break;
     case MYTIM_QUARTER:
          if (date_ok) {
            sqlite3_result_int(context, 1 + (tmp->mon/3));
          } else {
            sqlite3_result_null(context);
          }
          break;
     case MYTIM_SECOND:
          if ((date_ok == 1)
              && !(t.flags && KSU_TIME_DATETIME)) {
            // That's what MySQl seems to be doing
            if (year % 100 < 60) {
              sqlite3_result_int(context, (int)(year % 100));
            } else {
              sqlite3_result_null(context);
            }
            return;
          }
          sqlite3_result_int(context, second);
          break;
     case MYTIM_SECOND_MICROSECOND:
          sqlite3_result_int64(context, (sqlite3_int64)(second * 1000000));
          break;
     case MYTIM_WEEK:
          {
           long w;

           if ((date_ok == 1)
               && (ksu_extract_int(t, KSU_DATE_PART_WEEK, &w) == 1)) {
             sqlite3_result_int(context, (int)w);
           } else {
             sqlite3_result_null(context);
           }
          }
          break;
     case MYTIM_YEAR:
          sqlite3_result_int(context, year);
          break;
     case MYTIM_YEAR_MONTH:
          sqlite3_result_int(context, (int)(year * 100 + month));
          break;
     default:
          break;
   }
}
