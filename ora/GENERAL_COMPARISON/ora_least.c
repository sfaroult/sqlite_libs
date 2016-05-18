#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ksu_common.h>

extern void ora_least(sqlite3_context *context,
                      int               argc,
                      sqlite3_value   **argv){
  int        typ;
  char       only_number = 1;
  char       only_int = 1;
  int        c;
  int        min = 0;

  _ksu_check_arg_cnt(argc, 1, -1, "least");
  for (c = 0; c < argc; c++) {
     typ = sqlite3_value_type(argv[c]);
     switch (typ) {
         case SQLITE_NULL:
              sqlite3_result_null(context);
              return;
              break; /*NOTREACHED*/
         case SQLITE_FLOAT:
              only_int = 0;
              break;
         case SQLITE_INTEGER:
              break;
         case SQLITE_BLOB:
              ksu_err_msg(context, KSU_ERR_ARG_N_BINARY,
                          c+1, "least");
              return;
              break; /*NOTREACHED*/
         default:
              only_number = 0;
              break;
     }
  }
  c = 1;
  while (c < argc) {
     if (only_number) {
        if (sqlite3_value_double(argv[c])
                < sqlite3_value_double(argv[min])) {
           min = c;
        }
     } else {
        if (strcmp((const char *)sqlite3_value_text(argv[c]),
                   (const char *)sqlite3_value_text(argv[min]))<0) {
           min = c;
        }
     }
     c++;
  }
  if (only_number) {
     if (only_int) {
        sqlite3_result_int(context,
                       sqlite3_value_int(argv[min]));
     } else {
        sqlite3_result_double(context,
                       sqlite3_value_double(argv[min]));
     }
  } else {
     sqlite3_result_text(context,
                    (const char *)sqlite3_value_text(argv[min]),
                    -1, NULL);
  }
}
