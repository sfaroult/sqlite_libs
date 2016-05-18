#include <stdio.h>

#include <ksu_common.h>

extern void ora_vsize(sqlite3_context * context,
                      int               argc,
                      sqlite3_value  ** argv) {
        int sz;

        _ksu_null_if_null_param(argc, argv);
        sz = sqlite3_value_bytes(argv[0]);
        sqlite3_result_int(context, sz);
}
