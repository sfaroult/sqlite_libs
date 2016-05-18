#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <ksu_common.h>
#include <ksu_dates.h>

// Code in pg_to_date.c
extern void pg_to_date_timestamp(sqlite3_context *context,
                                 int              argc,
                                 sqlite3_value  **argv,
                                 char             *funcname);

extern void pg_to_timestamp(sqlite3_context *context,
                            int              argc,
                            sqlite3_value  **argv) {
    int        typ;
    time_t     unixtime;
    struct tm *lt;
    struct tm  loctime;
    char       dat[DATE_LEN];

    _ksu_check_arg_cnt(argc, 1, 2, "to_timestamp");
    if (argc == 1) {
       // Unix timestamp
       typ = sqlite3_value_type(argv[0]);
       if (typ != SQLITE_INTEGER) {
          ksu_err_msg(context, KSU_ERR_ARG_NOT_INT, "to_timestamp");
          return;
       }
       unixtime = (time_t)sqlite3_value_int(argv[0]);
       if ((lt = localtime_r(&unixtime, &loctime)) != (struct tm *)NULL) {
          sprintf(dat, "%d-%02d-%02d %02d:%02d:%02d",
                       lt->tm_year + 1900,
                       lt->tm_mon + 1,
                       lt->tm_mday,
                       lt->tm_hour,
                       lt->tm_min,
                       lt->tm_sec);
          sqlite3_result_text(context, dat, -1, SQLITE_TRANSIENT);
       } else {
          ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_timestamp");
       }
    } else {
      pg_to_date_timestamp(context, argc, argv, "to_timestamp");
    }
}
