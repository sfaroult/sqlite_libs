#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void  pg_octet_length(sqlite3_context * context,
                             int               argc,
                             sqlite3_value  ** argv) {

   unsigned char  *x;
   int             length;

   _ksu_null_if_null_param(argc, argv);
   x = (unsigned char *)sqlite3_value_text(argv[0]);
   length = strlen((char *)x);
   sqlite3_result_int(context, length);
}
