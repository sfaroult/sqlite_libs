#include <stdio.h>
#include <stdlib.h>
#include <ksu_common.h>

extern void my_sec_to_time(sqlite3_context  *context,
                           int argc,
                           sqlite3_value   **argv) {
   int    sec;
   char  *result;
   short  sign = 1;

   if (ksu_prm_ok(context, argc, argv, "sec_to_time",
                  KSU_PRM_INT)) {
     sec = sqlite3_value_int(argv[0]);
     if (sec < 0) {
       sec *= -1;
       sign = -1;
     }
     if ((result = (char *)sqlite3_malloc(30)) == NULL) {
        sqlite3_result_null(context);
        return;
     }
     int h = sign * (sec / 3600);
     int m = (sec % 3600) / 60;
     int s = (sec % 3600) % 60;
     sprintf(result, "%d:%02d:%02d", h, m, s);
     sqlite3_result_text(context,
                         result,
                         -1,           // Length - -1 means terminated by \0
                         sqlite3_free); // Function for freeing memory
  }
}
