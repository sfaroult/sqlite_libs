/*
 * Title: my_log.c
 * 
 * Description: Implementation of the log function for SQLite
 * 
 * 
 * History: March 25, 2015: Date Created
 */

#include <stdio.h>
#include <ksu_common.h>
#include <math.h>

extern void my_log(sqlite3_context * context,
                   int               argc,
                   sqlite3_value  ** argv) {
   int       typ;
   double    value;
   double    base;
   double    result;

   _ksu_check_arg_cnt(argc, 1, 2, "log");
   typ = sqlite3_value_type(argv[0]);
   if ((typ == SQLITE_NULL)
       || ((typ != SQLITE_INTEGER)
           && (typ != SQLITE_FLOAT))) {
     sqlite3_result_null(context);
     return;
   }
   value = sqlite3_value_double(argv[0]);
   if (value <= 0) {
     sqlite3_result_null(context);
     return;
   }
   result = log(value);
   if (argc == 2) {
     if (result == (double)0) {
       sqlite3_result_null(context);
       return;
     }
     typ = sqlite3_value_type(argv[1]);
     if ((typ == SQLITE_NULL)
         || ((typ != SQLITE_INTEGER)
             && (typ != SQLITE_FLOAT))) {
       sqlite3_result_null(context);
       return;
     }
     base = sqlite3_value_double(argv[1]);
     if (base <= 0) {
       sqlite3_result_null(context);
       return;
     }
     result = (log(base) / result);
   }
   if ((long)result == result) {
     sqlite3_result_int64(context, (long)result);
   } else {
     sqlite3_result_double(context, result);
   }
}
