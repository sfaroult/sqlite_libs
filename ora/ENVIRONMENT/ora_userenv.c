#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void ora_env(sqlite3_context *context, char *what,
                    int len, char legacy);

extern void ora_userenv(sqlite3_context  *context,
                        int               argc,
                        sqlite3_value   **argv){
  char  *what;

   _ksu_null_if_null_param(argc, argv);
   what = (char *)sqlite3_value_text(argv[0]);
   ora_env(context, what, 256, 1);
}
