#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void  pg_repeat(sqlite3_context * context, 
                       int               argc, 
                       sqlite3_value  ** argv) {
     char     *str;
     char     *result;
     int       count;
     int       i;

     _ksu_null_if_null_param(argc, argv);
     if (ksu_prm_ok(context, argc, argv, "repeat",
                    KSU_PRM_TEXT, KSU_PRM_INT)) {
       str = (char *)sqlite3_value_text(argv[0]);
       count = sqlite3_value_int(argv[1]);
       if (count <= 0) {
         sqlite3_result_text(context, "", -1, SQLITE_STATIC);
       } else {
         if ((result = (char *)sqlite3_malloc(strlen(str) * count + 1))
                   == (char *)NULL) {
           sqlite3_result_error_nomem(context);
           return;
         }
         *result = '\0';
         for (i = 0; i < count; i++) {
           strcat(result, str);
         }
         sqlite3_result_text(context, result, -1, sqlite3_free);
       }
     }
}
