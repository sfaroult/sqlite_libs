#include <ksu_common.h>

extern void ora_nvl(sqlite3_context * context,
                    int               argc,
                    sqlite3_value  ** argv) {

    if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
      sqlite3_result_value(context, argv[1]);
    } else {
      sqlite3_result_value(context, argv[0]);
    }
}
