#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ksu_common.h>

#define TIME_LEN   10

extern void pg_localtime(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
  time_t     tim;
  struct tm  tm;
  struct tm *t;
  char       timeval[TIME_LEN];
        
  // No parameters
  tim = time(NULL);
  if ((t = localtime_r(&tim, &tm)) != (struct tm *)NULL) {
    sprintf(timeval, "%02d:%02d:%02d",
                     t->tm_hour,
                     t->tm_min,
                     t->tm_sec);
    sqlite3_result_text(context, timeval, -1, SQLITE_TRANSIENT);
  } else {
    sqlite3_result_null(context);
  }
} 
