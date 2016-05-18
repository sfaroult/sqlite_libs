#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <ksu_common.h>
#include <ksu_dates.h>

#define INTV_LEN    64
#define TMP_LEN     20

#define _abs(x)  (x < 0 ? -1 * x : x)

extern void  pg_justify_hours(sqlite3_context * context,
                              int               argc,
                              sqlite3_value  ** argv) {
        char       *arg;
        char        tmp[TMP_LEN];
        KSU_INTV_T  intv;

        _ksu_null_if_null_param(argc, argv);
        arg = (char *)sqlite3_value_text(argv[0]);
        if (ksu_is_interval(arg, &intv)) {
          char  *msg = (char *)sqlite3_malloc(INTV_LEN);
          int    time_sec = 0;
          int    len;
                
          if (msg == (char *)NULL) {
            sqlite3_result_error_nomem(context);
            return;
          }
          *msg = '\0';
          // First adjust quantities
          // Don't forget that values may be SIGNED
          // First turn the time component (if there is one)
          // to seconds
          time_sec = intv.qty[KSU_INTV_SECOND]
                   + 60 * intv.qty[KSU_INTV_MINUTE]
                   + 3600 * intv.qty[KSU_INTV_HOUR];
          if (time_sec) {
            if (_abs(time_sec) > 86400) {
              intv.qty[KSU_INTV_DAY] += (time_sec / 86400);
              time_sec -= (time_sec/86400)*86400;
            }
            if (intv.qty[KSU_INTV_DAY] * time_sec < 0) {
              if (time_sec < 0) {
                time_sec += 86400;
                (intv.qty[KSU_INTV_DAY])--;
              } else {
                time_sec -= 86400;
                (intv.qty[KSU_INTV_DAY])++;
              }
            }
          }
          // Get rid of weeks
          if (intv.qty[KSU_INTV_WEEK]) {
            intv.qty[KSU_INTV_DAY] += (intv.qty[KSU_INTV_WEEK] * 7);
            intv.qty[KSU_INTV_WEEK] = 0;
          }
          // Don't propagate days to months with this function
          if (_abs(intv.qty[KSU_INTV_MONTH]) >= 12) {
            intv.qty[KSU_INTV_YEAR] += (intv.qty[KSU_INTV_MONTH] / 12);
            intv.qty[KSU_INTV_MONTH] -= (intv.qty[KSU_INTV_MONTH]/12)*12;
          }
          if (intv.qty[KSU_INTV_YEAR]) {
            snprintf(tmp, TMP_LEN, "%d %s ",
                                   intv.qty[KSU_INTV_YEAR],
                                   (intv.qty[KSU_INTV_YEAR] != 1 ?
                                      "years" : "year"));
            strncat(msg, tmp, INTV_LEN);
          } 
          if (intv.qty[KSU_INTV_MONTH]) {
            snprintf(tmp, TMP_LEN, "%d %s ",
                                   intv.qty[KSU_INTV_MONTH],
                                   (intv.qty[KSU_INTV_MONTH] != 1 ?
                                      "mons" : "mon"));
            strncat(msg, tmp, INTV_LEN - strlen(msg));
          } 
          if (intv.qty[KSU_INTV_DAY]) {
            snprintf(tmp, TMP_LEN, "%d %s ",
                                   intv.qty[KSU_INTV_DAY],
                                   (intv.qty[KSU_INTV_DAY] != 1 ?
                                      "days" : "day"));
            strncat(msg, tmp, INTV_LEN - strlen(msg));
          } 
          if (time_sec) {
            char neg = 0;
            if (time_sec < 0) {
              neg = 1;
              time_sec *= -1;
            }
            snprintf(tmp, TMP_LEN, "%s%02d:%02d:%02d",
                           (neg ? "-" : (intv.qty[KSU_INTV_DAY] < 0 ? "+":"")),
                           time_sec / 3600,
                           (time_sec % 60) / 60,
                           time_sec % 60);
            strncat(msg, tmp, INTV_LEN - strlen(msg));
            len = strlen(msg);
          } else {
            len = strlen(msg) - 1;
          }
          sqlite3_result_text(context, msg, len, sqlite3_free);
        } else {
          ksu_err_msg(context, KSU_ERR_ARG_NOT_INTV, "justify_days");
        }
} 
