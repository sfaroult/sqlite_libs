#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void  ora_upper(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
   _ksu_null_if_null_param(argc, argv);
   sqlite3_result_text(context,
                       (char *)ksu_utf8_upper(sqlite3_value_text(argv[0])),
                       -1, sqlite3_free);
}
