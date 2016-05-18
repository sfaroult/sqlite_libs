#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <ksu_common.h>
#include <ksu_my.h>
#include <ksu_dates.h>

extern void my_timestampadd(sqlite3_context * context,
                            int               argc,
                            sqlite3_value  ** argv) {
        char       *unit;
        int         intv;
        char       *dt;
        KSU_TIME_T  ts;
        KSU_TM_T   *t;
        KSU_TM_T    mydate;
        char        result_date[DATE_LEN];
        char       *d;
        int         secs = 0;
        int         months = 0;

        // MySQL doesn't return NULL if the first parameter
        // is null, but a syntax error (because it's a keyword,
        // not a string as in this implementation)
        if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
          ksu_err_msg(context, KSU_ERR_SYNTAX_ERROR, "timestampadd");
          return;
        }
        dt = (char *)sqlite3_value_text(argv[2]);
        if (!my_is_datetime((const char *)dt, &ts, 0)) {
          sqlite3_result_null(context);
          return;
        }
        unit = (char *)sqlite3_value_text(argv[0]);
        intv = my_value_int(argv[1], 0);
        // A SQL_TSI_ prefix is legal
        if (strncasecmp(unit, "SQL_TSI_", 8) == 0) {
          unit += 8;
        }
        switch(mytim_search(unit)) {
          case MYTIM_MICROSECOND :
               secs = (int)((double)intv/1000000 + 0.5);
               break;
          case MYTIM_SECOND :
               secs = intv;
               break;
          case MYTIM_MINUTE :
               secs = intv * 60;
               break;
          case MYTIM_HOUR :
               secs = intv * 3600;
               break;
          case MYTIM_DAY :
               secs = intv * 86400;
               break;
          case MYTIM_WEEK :
               secs = intv * 604800;
               break;
          case MYTIM_MONTH :
               months = intv;
               break;
          case MYTIM_QUARTER :
               months = intv * 3;
               break;
          case MYTIM_YEAR :
               months = intv * 12;
               break;
          default :
               ksu_err_msg(context, KSU_ERR_SYNTAX_ERROR_NEAR,
                           (char *)sqlite3_value_text(argv[0]),
                           "timestampadd");
               return;
        }
        if ((secs + months) == 0) {
          // No change
          sqlite3_result_text(context, (char *)dt, -1, NULL);
          return;
        }
        if (secs) {
          ts = ksu_add_secs(ts, secs);
        } else {
          t = ksu_localtime(ts, &mydate);
          if (t) {
            ts = ksu_add_months(t, months);
          }
        }
        if ((d = ksu_datetime(ts, result_date)) != (char *)NULL) {
          sqlite3_result_text(context, result_date, -1, SQLITE_TRANSIENT);
        } else {
          sqlite3_result_null(context);
        }
}
