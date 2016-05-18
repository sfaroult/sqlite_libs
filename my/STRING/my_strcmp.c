#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void my_strcmp(sqlite3_context * context, 
                      int               argc, 
                      sqlite3_value  ** argv) {
    int cmp;

    _ksu_null_if_null_param(argc, argv);
    cmp = strcmp((const char *)sqlite3_value_text(argv[0]),
                 (const char *)sqlite3_value_text(argv[1]));
    sqlite3_result_int(context, (cmp>0?1:(cmp<0?-1:0)));
}
