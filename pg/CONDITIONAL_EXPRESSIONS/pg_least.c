#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ksu_common.h>

extern void pg_least(sqlite3_context *context,
                        int               argc,
                        sqlite3_value   **argv){
  int        typ;
  char       only_number = 1;
  char       only_int = 1;
  char       only_null = 1;
  int        i;
  int        least = 0;

  _ksu_check_arg_cnt(argc, 1, -1, "least");
  for (i = 0; i < argc; i++) {
     typ = sqlite3_value_type(argv[i]);
     switch (typ) {
         case SQLITE_NULL:
              break;
         case SQLITE_INTEGER:
              only_null = 0;
              break;
         case SQLITE_FLOAT:
              only_int = 0;
              only_null = 0;
              break;
         case SQLITE_BLOB:
              ksu_err_msg(context, KSU_ERR_ARG_N_BINARY,
                          i+1, "least");
              return;
              break; /*NOTREACHED*/
         default:
              only_number = 0;
              only_null = 0;
              break;
     }
  }
  if (only_null) {
     sqlite3_result_null(context);
     return;
  }
  for (i = 1; i < argc; i++) {
     if (sqlite3_value_type(argv[i]) != SQLITE_NULL) { 
       if (only_number) {
          if (sqlite3_value_double(argv[i])
                  < sqlite3_value_double(argv[least])) {
             least = i;
          }
       } else {
          if (strcmp((const char *)sqlite3_value_text(argv[i]),
                     (const char *)sqlite3_value_text(argv[least]))<0) {
             least = i;
          }
       }
     }
  }
  if (only_number) {
     if (only_int) {
        sqlite3_result_int(context,
                           sqlite3_value_int(argv[least]));
     } else {
        sqlite3_result_double(context,
                              sqlite3_value_double(argv[least]));
     }
  } else {
     sqlite3_result_text(context,
                    (const char *)sqlite3_value_text(argv[least]),
                    -1, NULL);
  }
}
