#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <ksu_common.h>

extern void my_instr(sqlite3_context  *context,
                     int               argc,
                     sqlite3_value   **argv) {
   char *str;
   char *p;
   char *substr;
   int   pos = 0;
   
   _ksu_null_if_null_param(argc, argv);
   str = (char *)sqlite3_value_text(argv[0]);
   substr = (char *)sqlite3_value_text(argv[1]);
   if ((p = strcasestr(str, substr)) != (char *)NULL) {
     pos = 1 + ksu_bytes_to_charpos((unsigned char *)str, p - str);
   }
   sqlite3_result_int(context, pos);
}
