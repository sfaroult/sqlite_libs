#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <time.h>

#include <ksu_common.h>

#define _abs(n) (n < 0 ? -1 * n : n)

extern void my_timediff(sqlite3_context  *context,
                        int argc,
                        sqlite3_value   **argv) {
   int       year1 = 0;
   int       year2 = 0;
   int       month1 = 0;
   int       month2 = 0;
   int       days1 = 0;
   int       days2 = 0;
   int       hour1 = 0;
   int       hour2 = 0;
   int       min1 = 0;
   int       min2 = 0;
   double    seconds1 = 0;
   double    seconds2 = 0;
   time_t    ts1;
   time_t    ts2;
   int       hours;
   int       minutes;
   int       seconds;
   struct tm tm1;
   struct tm tm2;
   char     *v;
   char     *vv;
   char      neg;
   char      buffer[80];
   int       diff;

   _ksu_null_if_null_param(argc, argv);
   v = (char *)sqlite3_value_text(argv[0]);
   vv = (char *)sqlite3_value_text(argv[1]);
   (void)memset(&tm1, 0, sizeof(struct tm));
   (void)memset(&tm2, 0, sizeof(struct tm));
   // Test datetime format
   if (sscanf(v, "%d-%d-%d %d:%d:%lf",
              &year1, &month1, &(tm1.tm_mday),
              &(tm1.tm_hour), &(tm1.tm_min), &seconds1) >= 4) {
     // Negative values are ignored by MySQL (warning issued)
     tm1.tm_year = _abs(year1) - 1900;
     tm1.tm_mon = _abs(month1) - 1;
     tm1.tm_mday = _abs(tm1.tm_mday);
     tm1.tm_hour = _abs(tm1.tm_hour);
     tm1.tm_min = _abs(tm1.tm_min);
     tm1.tm_sec = (int)(_abs(seconds1) + 0.5);
     if ((tm1.tm_mon > 11)
        || ((tm1.tm_mday > 31) || (tm1.tm_mday < 1))
        || (tm1.tm_hour > 23)
        || (tm1.tm_min > 59)
        || (tm1.tm_sec > 60)
        || ((ts1 = mktime(&tm1)) == (time_t)-1)) {
       sqlite3_result_null(context);
       return;
     }
     if (sscanf(vv, "%d-%d-%d %d:%d:%lf",
                &year2, &month2, &(tm2.tm_mday),
                &(tm2.tm_hour), &(tm2.tm_min), &seconds2) >= 4) {
       // Negative values in the second one cause NULL to be returned
       tm2.tm_year = year2 - 1900;
       tm2.tm_mon = month2 - 1;
       tm2.tm_sec = (int)(seconds2 + 0.5);
       if (((tm2.tm_mon > 11) || (tm2.tm_mon < 0))
          || ((tm2.tm_mday > 31) || (tm2.tm_mday < 1))
          || ((tm2.tm_hour > 23) || (tm2.tm_hour < 0))
          || ((tm2.tm_min > 59) || (tm2.tm_min < 0))
          || ((tm2.tm_sec > 60) || (tm2.tm_sec < 0))
          || ((ts2 = mktime(&tm2)) == (time_t)-1)) {
         sqlite3_result_null(context);
         return;
       }
     } else {
       // Incompatible formats
       sqlite3_result_null(context);
       return;
     }
   } else {
     // Time format - can be "days hours:min:sec.frac"
     // or "hours:min:sec.frac" for either parameter
     neg = 0;
     if (sscanf(v, "%d %d:%d:%lf", &days1, &hour1, &min1, &seconds1) < 2) {
       days1 = 0;
       if (sscanf(v, "%d:%d:%lf", &hour1, &min1, &seconds1) == 0) {
         sqlite3_result_null(context);
         return;
       } else {
         if (hour1 < 0) {
           neg = 1;
           hour1 = -1 * hour1;
         }
       }
     } else {
       if (days1 < 0) {
         neg = 1;
         days1 = -1 * days1;
       }
       if (hour1 < 0) {
         sqlite3_result_null(context);
         return;
       }
     }
     if (((min1 > 59) || (min1 < 0))
         || ((seconds1 >= 60) || (seconds1 < 0))) {
       sqlite3_result_null(context);
       return;
     }
     ts1 = (neg ? -1 : 1) * (days1 * 86400 
                             + hour1 * 3600
                             + min1 * 60
                             + (int)(seconds1 + 0.5));
     neg = 0;
     if (sscanf(vv, "%d %d:%d:%lf", &days2, &hour2, &min2, &seconds2) < 2) {
       days2 = 0;
       if (sscanf(vv, "%d:%d:%lf", &hour2, &min2, &seconds2) == 0) {
         sqlite3_result_null(context);
         return;
       } else {
         if (hour2 < 0) {
           neg = 1;
           hour2 = -1 * hour2;
         }
       }
     } else {
       if (days2 < 0) {
         neg = 2;
         days2 = -1 * days2;
       }
       if (hour2 < 0) {
         sqlite3_result_null(context);
         return;
       }
     }
     if (((min2 > 59) || (min2 < 0))
         || ((seconds2 >= 60) || (seconds2 < 0))) {
       sqlite3_result_null(context);
       return;
     }
     ts2 = (neg ? -1 : 1) * (days2 * 86400 
                             + hour2 * 3600
                             + min2 * 60
                             + (int)(seconds2 + 0.5));
    }
    neg = 0;
    diff = (ts1 - ts2);
    if (diff < 0) {
      neg = 1;
      diff *= -1;
    }
    hours = diff / 3600;
    minutes = (diff - hours * 3600) / 60;
    seconds = diff - hours * 3600 - minutes * 60;
    sprintf(buffer, "%s%02d:%02d:%02d",
                    (neg ? "-" : ""),
                    hours, _abs(minutes), _abs(seconds));
    sqlite3_result_text(context, buffer, -1, SQLITE_TRANSIENT);
}
