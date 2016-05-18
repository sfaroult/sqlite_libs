#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ksu_common.h>

extern void my_bit_length(sqlite3_context * context, 
                          int               argc, 
                          sqlite3_value  ** argv) {
    int byte_count;
        
    _ksu_null_if_null_param(argc, argv);
    byte_count = sqlite3_value_bytes(argv[0]);
    sqlite3_result_int(context, byte_count * 8);
} 
