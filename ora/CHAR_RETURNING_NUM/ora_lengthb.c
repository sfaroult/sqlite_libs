#include <stdio.h>
#include <stdlib.h>
#include <ksu_common.h>

extern void  ora_lengthb(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
   int             length;

   _ksu_null_if_null_param(argc, argv);
   length = sqlite3_value_bytes(argv[0]);
   sqlite3_result_int(context, length);
}
