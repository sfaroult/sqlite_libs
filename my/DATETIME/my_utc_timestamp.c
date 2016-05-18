#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <ksu_common.h>

extern void     my_utc_timestamp(sqlite3_context * context, 
                                 int argc, 
                                 sqlite3_value  ** argv) {
   time_t     timeNow;
   struct tm *timeinfo;
   char       buffer[20];

        
   timeNow = time(NULL);
   if ((timeinfo = gmtime(&timeNow)) != (struct tm *)NULL) {
     (void)sprintf(buffer, "%4d-%02d-%02d %02d:%02d:%02d",
                   timeinfo->tm_year + 1900,
                   timeinfo->tm_mon + 1,
                   timeinfo->tm_mday,
                   timeinfo->tm_hour,
                   timeinfo->tm_min,
                   timeinfo->tm_sec);
     sqlite3_result_text(context, buffer, -1, SQLITE_TRANSIENT);
   } else {
     sqlite3_result_null(context);
   }
} 
