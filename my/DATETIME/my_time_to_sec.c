#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include <ksu_common.h>
/*
 * This Program takes a string in hr:min:sec format, and makes it into a total of seconds.
 */

void my_time_to_sec(sqlite3_context  *context,
                    int               argc,
                    sqlite3_value   **argv) {
   char *v;
   int   result;
   int   hour;
   int   min;
   int   sec;
   short sign = 1;

   if (ksu_prm_ok(context, argc, argv, "time_to_sec", KSU_PRM_TIME)) { 
     v = (char *)sqlite3_value_text(argv[0]);
     if (3 == sscanf(v, "%d:%d:%d", &hour, &min, &sec)) {
       if (hour < 0) {
          sign = -1;
          hour = hour * -1;
       }
       if (min > 59 || min < 0) {
         ksu_err_msg(context, KSU_ERR_INV_MIN, min, "time_to_sec");
         return;
       }
       if (sec > 59 || sec < 0) {
         ksu_err_msg(context, KSU_ERR_INV_MIN, sec, "time_to_sec");
         return;
       }
       result = sign * (sec + (hour * 3600) + (min * 60));
       sqlite3_result_int(context, result);
     } else {
       ksu_err_msg(context, KSU_ERR_ARG_NOT_TIME, "time_to_sec");
     }
   }
   return;
}
