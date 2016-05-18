#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_pg.h>

/*
 *   This is a special function for implementing PostgreSQL pre-defined
 *   time names.
 */

extern void pg_time(sqlite3_context * context,
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
      case PGSPECIAL_ALLBALLS:
           sqlite3_result_text(context,
                               "00:00:00",
                               -1,
                               SQLITE_STATIC);
           break;
      case PGSPECIAL_NOW:
           now = time(NULL);
           if ((t = localtime_r(&now, &mydate)) != (struct tm *)NULL) {
             snprintf(buffer, DATE_LEN, "%02d:%02d:%02d",
                      t->tm_hour,
                      t->tm_min,
                      t->tm_sec);
             sqlite3_result_text(context, buffer, -1, SQLITE_TRANSIENT);
           } else {
             sqlite3_result_null(context);
           }
           break;
      case PGSPECIAL_NOT_FOUND:
           // If time, return the parameter
           // Else return an error (no attempt to parse and guess)
           if (ksu_is_time((const char *)d, &kt, 1)) {
             sqlite3_result_text(context, d, -1, NULL);
           } else {
             ksu_err_msg(context, KSU_ERR_ARG_NOT_TIME, "time");
           }
           break;
    }
} 
