#include <ksu_common.h>

extern void  my_char_length(sqlite3_context * context,
                            int               argc,
                            sqlite3_value  ** argv) {
    _ksu_null_if_null_param(argc, argv);
    sqlite3_result_int(context, 
          ksu_charlen((unsigned char *)sqlite3_value_text(argv[0])));
}
