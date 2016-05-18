#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include <ksu_common.h>

extern void ora_localtimestamp(sqlite3_context * context, 
                               int               argc, 
                               sqlite3_value  ** argv) {
        time_t     timeNow;
        struct tm *timeinfo;
        struct tm  mytimeinfo;
        char       timestamp[20];

        timeNow = time(NULL);
        if ((timeinfo = localtime_r(&timeNow, &mytimeinfo))
                      != (struct tm *)NULL) {
          sprintf(timestamp, "%4d-%02d-%02d %02d:%02d:%02d",
                             1900 + timeinfo->tm_year,
                             1 + timeinfo->tm_mon,
                             timeinfo->tm_mday,
                             timeinfo->tm_hour,
                             timeinfo->tm_min,
                             timeinfo->tm_sec);
          sqlite3_result_text(context, timestamp, -1, SQLITE_TRANSIENT);
        } else {
          sqlite3_result_null(context);
        }
} 
