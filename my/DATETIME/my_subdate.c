#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ksu_common.h>
#include <ksu_my.h>
#include <ksu_dates.h>

#define TIME_LEN   100

extern void my_subdate(sqlite3_context * context, 
                       int               argc, 
                       sqlite3_value  ** argv) {
   char   timespec[TIME_LEN];
   int    typ;
                       
   // Three forms are supported:
   // subdate(<date>, <expr>)   -- unit defaults to days
   // subdate(<date>, <expr>, <unit>)
   // subdate(<date>, 'interval <something> <unit>')
   //
   // The MySQL function returns null (and a warning) if
   // the first parameter isn't a datetime, and returns
   // the first parameter if the second parameter is invalid.
   //
   _ksu_check_arg_cnt(argc, 2, 3, "subdate");
   if (argc == 2) {
     typ = sqlite3_value_type(argv[1]);
     if (typ == SQLITE_NULL) {
       sqlite3_result_null(context);
     } else {
       if ((typ == SQLITE_INTEGER)
           || (typ == SQLITE_FLOAT)) {
          snprintf(timespec, TIME_LEN, "interval %d day",
                   sqlite3_value_int(argv[1]));
          my_date_op(context, "subdate",
                     (char *)sqlite3_value_text(argv[0]),
                     (char)0,
                     (char *)timespec);
       } else {
         my_date_op(context, "subdate",
                    (char *)sqlite3_value_text(argv[0]),
                    (char)0,
                    (char *)sqlite3_value_text(argv[1]));
       }
     }
   } else {
     snprintf(timespec, TIME_LEN, "interval %s %s",
              (char *)sqlite3_value_text(argv[1]),
              (char *)sqlite3_value_text(argv[2]));
     my_date_op(context, "subdate",
                (char *)sqlite3_value_text(argv[0]),
                (char)0,
                timespec);
   }
} 
