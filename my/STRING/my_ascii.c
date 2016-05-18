#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ksu_common.h>

extern void my_ascii(sqlite3_context * context, 
                     int               argc, 
                     sqlite3_value  ** argv) {
    unsigned char *p;
        
    _ksu_null_if_null_param(argc, argv);
    p = (unsigned char *)sqlite3_value_text(argv[0]);
    sqlite3_result_int(context, (int)*p);
} 
