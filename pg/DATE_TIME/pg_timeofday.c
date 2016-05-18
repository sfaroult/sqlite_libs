#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ksu_common.h>

#define DATESTR_LEN   40

extern void pg_timeofday(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
  time_t     tim;
  struct tm  tm;
  struct tm *t;
  char       datestr[DATESTR_LEN];
        
  // No parameters
  tim = time(NULL);
  if (((t = localtime_r(&tim, &tm)) != (struct tm *)NULL)
      && strftime(datestr, DATESTR_LEN,
                  "%a %b %d %H:%M:%S %Y %Z",
                  (const struct tm *)t)) {
    sqlite3_result_text(context, datestr, -1, SQLITE_TRANSIENT);
  } else {
    sqlite3_result_null(context);
  }
} 
