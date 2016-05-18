#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ksu_common.h>

#define DATE_LEN   20

extern void pg_localtimestamp(sqlite3_context * context,
                              int               argc,
                              sqlite3_value  ** argv) {
  time_t     tim;
  struct tm  tm;
  struct tm *t;
  char       timestamp[DATE_LEN];
        
  // No parameters
  tim = time(NULL);
  if ((t = localtime_r(&tim, &tm)) != (struct tm *)NULL) {
    sprintf(timestamp, "%4d-%02d-%02d %02d:%02d:%02d",
                       1900 + t->tm_year,
                       1 + t->tm_mon,
                       t->tm_mday,
                       t->tm_hour,
                       t->tm_min,
                       t->tm_sec);
    sqlite3_result_text(context, timestamp, -1, SQLITE_TRANSIENT);
  } else {
    sqlite3_result_null(context);
  }
} 
