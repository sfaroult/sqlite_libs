#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ksu_common.h>
#include <ksu_my.h>

#define TM_CNT   6

#define TM_YEAR     0
#define TM_MON      1
#define TM_MDAY     2
#define TM_HOUR     3
#define TM_MIN      4
#define TM_SEC      5


static short G_lim[TM_CNT] = {0, 12, 31, 24, 60, 60};

extern void my_timestampdiff(sqlite3_context  *context,
                             int               argc,
                             sqlite3_value   **argv) {
   struct tm  dt1;
   struct tm  dt2;
   int        dtlen;
   time_t     firstone;
   time_t     secondone;
   long       diffsecond;
   int        diff[TM_CNT];
   int        i;
   char      *unit;
   char      *p;
   int        unitcode;
   char       neg = 0;

   // MySQL doesn't return NULL if the first parameter
   // is null, but a syntax error (because it's a keyword,
   // not a string as in this implementation)
   if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
     ksu_err_msg(context, KSU_ERR_SYNTAX_ERROR, "timestampdiff");
     return;
   }
   if (ksu_prm_ok(context, argc, argv, "timestampdiff",
                  KSU_PRM_TEXT, KSU_PRM_DATETIME, KSU_PRM_DATETIME)) {
     unit = (char *)sqlite3_value_text(argv[0]);
     // A SQL_TSI_ prefix is legal
     if (strncasecmp(unit, "SQL_TSI_", 8) == 0) {
       unit += 8;
     }
     unitcode = mytim_search(unit);
     if (unitcode == MYTIM_NOT_FOUND) {
       ksu_err_msg(context, KSU_ERR_INV_TIME_UNIT,
                   (char *)sqlite3_value_text(argv[0]),
                   "timestampdiff");
       return;
     }
     p = (char *)sqlite3_value_text(argv[1]);
     (void)memset(&dt1, 0, sizeof(struct tm));
     dtlen = strlen(p);
     if (dtlen == 19) {
       if (sscanf(p, "%d-%d-%d %d:%d:%d",
                  &dt1.tm_year, &dt1.tm_mon, &dt1.tm_mday,
                  &dt1.tm_hour, &dt1.tm_min, &dt1.tm_sec) != 6) {
         ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                     2, "timestampdiff");
         return;
       }
     } else if (dtlen == 10)   {
       if (sscanf(p, "%d-%d-%d",
                  &dt1.tm_year, &dt1.tm_mon, &dt1.tm_mday) != 3) {
         ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATE,
                     2, "timestampdiff");
         return;
       }
     } else {
       ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                   2, "timestampdiff");
       return;
     }
     dt1.tm_year -= 1900;
     dt1.tm_mon -= 1;
     firstone = mktime(&dt1);
     p = (char *)sqlite3_value_text(argv[2]);
     (void)memset(&dt2, 0, sizeof(struct tm));
     dtlen = strlen(p);
     if (dtlen == 19) {
       if (sscanf(p, "%d-%d-%d %d:%d:%d",
                  &dt2.tm_year, &dt2.tm_mon, &dt2.tm_mday,
                  &dt2.tm_hour, &dt2.tm_min, &dt2.tm_sec) != 6) {
         ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                     3, "timestampdiff");
         return;
       }
     } else if (dtlen == 10)   {
       if (sscanf(p, "%d-%d-%d",
                     &dt2.tm_year, &dt2.tm_mon, &dt2.tm_mday) != 3) {
         ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATE,
                     3, "timestampdiff");
         return;
       }
     } else {
       ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                   3, "timestampdiff");
       return;
     }
     dt2.tm_year -= 1900;
     dt2.tm_mon -= 1;
     secondone = mktime(&dt2);
     if (secondone < firstone) {
       neg = 1;
       diffsecond = (long)(firstone - secondone);
       diff[TM_YEAR] = dt1.tm_year - dt2.tm_year;
       diff[TM_MON] = dt1.tm_mon - dt2.tm_mon;
       diff[TM_MDAY] = dt1.tm_mday - dt2.tm_mday;
       diff[TM_HOUR] = dt1.tm_hour - dt2.tm_hour;
       diff[TM_MIN] = dt1.tm_min - dt2.tm_min;
       diff[TM_SEC] = dt1.tm_sec - dt2.tm_sec;
     } else {
       diffsecond = (long)(secondone - firstone);
       diff[TM_YEAR] = dt2.tm_year - dt1.tm_year;
       diff[TM_MON] = dt2.tm_mon - dt1.tm_mon;
       diff[TM_MDAY] = dt2.tm_mday - dt1.tm_mday;
       diff[TM_HOUR] = dt2.tm_hour - dt1.tm_hour;
       diff[TM_MIN] = dt2.tm_min - dt1.tm_min;
       diff[TM_SEC] = dt2.tm_sec - dt1.tm_sec;
     }
     for (i = TM_SEC; i >= TM_MON; i--) {
       if (diff[i] < 0) {
         diff[i] += G_lim[i];
         diff[i-1] -= 1;
       }
     }
     switch (unitcode) {
       case MYTIM_MICROSECOND:
            sqlite3_result_int(context,
                              (int)((neg ? -1 : 1) * diffsecond * 1000000));
            break;
       case MYTIM_SECOND:
            sqlite3_result_int(context, (int)((neg ? -1 : 1) * diffsecond));
            break;
       case MYTIM_MINUTE:
            sqlite3_result_int(context,
                               (int)((neg ? -1 : 1) * diffsecond / 60));
            break;
       case MYTIM_HOUR:
            sqlite3_result_int(context,
                               (int)((neg ? -1 : 1) * diffsecond / 3600));
            break;
       case MYTIM_DAY:
            sqlite3_result_int(context,
                               (int)((neg ? -1 : 1) * diffsecond / 86400));
            break;
       case MYTIM_WEEK:
            sqlite3_result_int(context,
                               (int)((neg ? -1 : 1) * diffsecond / 345600));
            break;
       case MYTIM_MONTH:
            sqlite3_result_int(context,
                               (neg ? -1 : 1) * (diff[TM_YEAR] * 12
                                                 + diff[TM_MON]));
            break;
       case MYTIM_QUARTER:
            sqlite3_result_int(context,
                               (neg ? -1 : 1) * (diff[TM_YEAR] * 4
                                                 + diff[TM_MON] / 4));
            break;
       case MYTIM_YEAR:
            sqlite3_result_int(context, (neg ? -1 : 1) * diff[TM_YEAR]);
            break;
       default:
            ksu_err_msg(context, KSU_ERR_INV_TIME_UNIT,
                        (char *)sqlite3_value_text(argv[0]),
                        "timestampdiff");
            break;
      }
   }
}
