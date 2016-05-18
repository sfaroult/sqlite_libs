#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ksu_common.h>
#include <ksu_my.h>

#define FORMATTED_LEN  50

extern void my_format(sqlite3_context * context,
                      int               argc,
                      sqlite3_value ** argv) {

   double          val;
   int             d;
   char    buffer[FORMATTED_LEN];

   _ksu_check_arg_cnt(argc, 2, -1, "format");
   _ksu_null_if_null_param(argc, argv);
   if (argc == 3) {
     (void)setlocale(LC_NUMERIC, (char *)sqlite3_value_text(argv[2]));
   } else {
     // Set locale if not done already
     ksu_i18n();
   }
   switch(sqlite3_value_type(argv[0])) {
     case SQLITE_INTEGER:
     case SQLITE_FLOAT:
          val = sqlite3_value_double(argv[0]);
          break;
     default:
          val = 0.0;
          break;
   }
   d = my_value_int(argv[1], 0);
   if (d < 0) d = 0;
   (void)snprintf(buffer, FORMATTED_LEN, "%'.*lf", d, val);
   sqlite3_result_text(context, buffer, -1, SQLITE_TRANSIENT);
}
