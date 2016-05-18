/*
   This function takes in one or two dates, if one, it subtracts the given date with the current system date at midnight.
   If two, it subtracts the two dates from each other.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>
#include <time.h>

#define AGE_LEN   100
#define BUFF_LEN   20

//Finds the difference between two dates.
static struct tm *findDifference(struct tm *end, struct tm *beg) {
   int year = (beg)->tm_year;
   int mon = (beg)->tm_mon;

   if ((end)->tm_mday < (beg)->tm_mday) {
     if ((end)->tm_mon == 0) {
       (end)->tm_mon = 12;
       (end)->tm_year--;
       (end)->tm_mday += 31;
     } else   {
       (end)->tm_mon--;
       if ((mon == 2) || (mon == 4) || (mon == 6) ||
           (mon == 7) || (mon == 9) || (mon == 11)) {
         (end)->tm_mday += 31;
       } else if (mon == 1) {
         if (year % 4 == 0) {
           (end)->tm_mday += 29;
         } else {
           (end)->tm_mday += 28;
         }
       } else {
         (end)->tm_mday += 30;
       }
     }
   }
   (end)->tm_mday -= (beg)->tm_mday;
   if ((end)->tm_mon < (beg)->tm_mon) {
     (end)->tm_year--;
     (end)->tm_mon += 12;
   }
   (end)->tm_mon -= (beg)->tm_mon;
   (end)->tm_year -= (beg)->tm_year;
   return end;
}

extern void pg_age(sqlite3_context  *context,
                   int argc,
                   sqlite3_value   **argv) {
   struct tm  age;
   struct tm  beg;
   struct tm  tmbuff;
   struct tm *bp;
   struct tm *bigger;
   struct tm *smaller;
   time_t     currTime;
   double     seconds;
   char      *rp;
   int        rplen;
   char       buff[BUFF_LEN];
   char      *arg1;
   char      *arg2;

   _ksu_check_arg_cnt(argc, 1, 2, "age");
    if (ksu_prm_ok(context, argc, argv,
                   "age", KSU_PRM_DATETIME, KSU_PRM_DATETIME)) {
      (void)memset(&age, 0, sizeof(struct tm));
      arg1 = (char *)sqlite3_value_text(argv[0]);
      if ((sscanf(arg1, "%d-%d-%d %d:%d:%d",
                  &age.tm_year, &age.tm_mon, &age.tm_mday,
                  &age.tm_hour, &age.tm_min, &age.tm_sec) < 3)) {
        ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                    1, "age");
        return;
      }
      age.tm_mon -= 1;
      age.tm_year -= 1900;
      if (argc == 1) {
        time(&currTime);
        if (currTime != -1) {
          bp = localtime_r(&currTime, &tmbuff);
          if (bp) {
            bp->tm_hour = 0;
            bp->tm_min = 0;
            bp->tm_sec = 0;
            bp->tm_gmtoff = 0;
            bp->tm_isdst = 0;
            seconds = currTime - mktime(&age);
            if (seconds > 0) {
              bigger = bp;
              smaller = &age;
            } else {
              bigger = &age;
              smaller = bp;
            }
          } else {
            // Not expected
            sqlite3_result_null(context);
            return;
          }
        } else {
          // Not expected
          sqlite3_result_null(context);
          return;
        }
      } else   {
        (void)memset(&beg, 0, sizeof(struct tm));
        arg2 = (char *)sqlite3_value_text(argv[1]);
        if ((sscanf(arg2, "%d-%d-%d %d:%d:%d",
            &beg.tm_year, &beg.tm_mon, &beg.tm_mday,
            &beg.tm_hour, &beg.tm_min, &beg.tm_sec) < 3)) {
          ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                      2, "age");
          return;
        }
        beg.tm_mon -= 1;
        beg.tm_year -= 1900;
        seconds = mktime(&age) - mktime(&beg);
        if (seconds > 0) {
          bigger = &age;
          smaller = &beg;
        } else   {
          bigger = &beg;
          smaller = &age;
        }
        bp = &age;
      }
      findDifference(bigger, smaller);
      if ((rp = (char *)sqlite3_malloc(AGE_LEN)) == NULL) {
        sqlite3_result_error_nomem(context);
        return;
      }
      rp[0] = '\0';
      rplen = 0;
      if (bigger->tm_year) {
        snprintf(buff, BUFF_LEN, "%s%d years",
                 (bigger == bp ? "" : "-"), bigger->tm_year);
        strncat(rp, buff, AGE_LEN - rplen);
        rplen = strlen(rp);
      }
      if (bigger->tm_mon) {
        snprintf(buff, BUFF_LEN, "%s%s%d mons",
                 (rplen ? " " : ""),
                 (bigger == bp ? "" : "-"), bigger->tm_mon);
        strncat(rp, buff, AGE_LEN - rplen);
        rplen = strlen(rp);
      }
      if (bigger->tm_mday) {
        snprintf(buff, BUFF_LEN, "%s%s%d days",
                 (rplen ? " " : ""),
                 (bigger == bp ? "" : "-"),  bigger->tm_mday);
        strncat(rp, buff, AGE_LEN - rplen);
      }
      sqlite3_result_text(context, rp, -1, sqlite3_free);
   }
}
