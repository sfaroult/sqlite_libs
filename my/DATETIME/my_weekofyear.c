#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>
#include <ksu_dates.h>

/*
 * Takes a date as a parameter in the string form
 * (YYYY-MM-DD) and converts to weeks between 1 and 53.
 */
extern void my_weekofyear(sqlite3_context *context,
                          int              argc,
                          sqlite3_value  **argv) {
   KSU_TIME_T t;

   if (ksu_prm_ok(context, argc, argv, "weekofyear", KSU_PRM_DATE)) {
     if (ksu_is_datetime((const char *)sqlite3_value_text(argv[0]),
                         &t, 0)) {
       sqlite3_result_int(context, ksu_iso_week(t)); 
     } else {
       sqlite3_result_null(context);
     }
   } else {
     sqlite3_result_null(context);
   }
}
