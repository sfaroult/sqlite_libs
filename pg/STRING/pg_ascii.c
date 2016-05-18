#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>

extern void pg_ascii(sqlite3_context * context,
                     int               argc,
                     sqlite3_value  ** argv) {
        unsigned int result;
        int          len;

        _ksu_null_if_null_param(argc, argv);
        result = ksu_utf8_to_codepoint(sqlite3_value_text(argv[0]), &len);
        if (len) {
           sqlite3_result_int64(context, (long)result);
        } else {
          ksu_err_msg(context, KSU_ERR_INV_UTF8, "ascii");
        }
}
