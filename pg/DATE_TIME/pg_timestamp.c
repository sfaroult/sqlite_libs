#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_pg.h>

/*
 *   This is a special function for implementing PostgreSQL pre-defined
 *   date names.
 */

extern void pg_timestamp(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
        
    char      *d;
    int        code;
    time_t     now;
    struct tm *t;
    struct tm  mydate;
    char       buffer[DATE_LEN];
    KSU_TIME_T kt;

    _ksu_null_if_null_param(argc, argv);
    d = (char *)sqlite3_value_text(argv[0]); 
    switch((code = pgspecial_search(d))) {
      case PGSPECIAL_PLUSINFINITY :
      case PGSPECIAL_MINUSINFINITY:
           // Return as is
           sqlite3_result_text(context, pgspecial_keyword(code),
                               -1, SQLITE_STATIC);
           break;
      case PGSPECIAL_INFINITY:
           // Return as +infinity
           sqlite3_result_text(context,
                               pgspecial_keyword(PGSPECIAL_PLUSINFINITY),
                               -1, SQLITE_STATIC);
           break;
      case PGSPECIAL_EPOCH:
           sqlite3_result_text(context,
                               "1970-01-01 00:00:00",
                               -1,
                               SQLITE_STATIC);
           break;
      case PGSPECIAL_NOW:
      case PGSPECIAL_TODAY:
      case PGSPECIAL_TOMORROW:
      case PGSPECIAL_YESTERDAY:
           now = time(NULL) + (code == PGSPECIAL_TOMORROW ?
                               86400 : (code == PGSPECIAL_YESTERDAY ?
                                        -86400 : 0));
           if ((t = localtime_r(&now, &mydate)) != (struct tm *)NULL) {
              if (code != PGSPECIAL_NOW)  {
                snprintf(buffer, DATE_LEN, "%4d-%02d-%02d 00:00:00",
                         t->tm_year + 1900,
                         t->tm_mon + 1,
                         t->tm_mday);
              } else {
                snprintf(buffer, DATE_LEN, "%4d-%02d-%02d %02d:%02d:%02d",
                         t->tm_year + 1900,
                         t->tm_mon + 1,
                         t->tm_mday,
                         t->tm_hour,
                         t->tm_min,
                         t->tm_sec);
              }
              sqlite3_result_text(context, buffer, -1, SQLITE_TRANSIENT);
           } else {
             sqlite3_result_null(context);
           }
           break;
      case PGSPECIAL_NOT_FOUND:
           // If valid date or date time, return the parameter
           // Else return an error (no attempt to parse and guess)
           if (ksu_is_datetime((const char *)d, &kt, 0)) {
             sqlite3_result_text(context, d, -1, NULL);
           } else {
             ksu_err_msg(context, KSU_ERR_INV_DATE, "timestamp");
           }
           break;
    }
} 
