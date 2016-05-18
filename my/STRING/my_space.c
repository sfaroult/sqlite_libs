#include <stdio.h>
#include <string.h>
#include <ksu_common.h>
#include <ksu_my.h>

extern void my_space(sqlite3_context * context,
                     int               argc,
                     sqlite3_value ** argv) {
     int   len;
     char *sp;

     _ksu_null_if_null_param(argc, argv);
     len = my_value_int(argv[0], 0);
     if (len) {
       if ((sp = (char *)sqlite3_malloc(len)) == (char *)NULL) {
         sqlite3_result_error_nomem(context);
         return;
       }
       (void)memset(sp, ' ', len);
       sqlite3_result_text(context, sp, len, sqlite3_free);
     } else {
       sqlite3_result_text(context, "", -1, SQLITE_STATIC);
     }
}
