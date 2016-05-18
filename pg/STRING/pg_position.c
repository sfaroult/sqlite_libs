#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ksu_common.h>

extern void pg_position(sqlite3_context  *context,
                        int               argc,
                        sqlite3_value   **argv) {
   char *str;
   char *p;
   char *substr;
   int   pos = 0;
   int   frompos = 1;
   int   bytepos = 1;
   
   _ksu_null_if_null_param(argc, argv);
   substr = (char *)sqlite3_value_text(argv[0]);
   str = (char *)sqlite3_value_text(argv[1]);
   bytepos = ksu_lax_charpos_to_bytes((unsigned char *)str, frompos - 1);
   // Case-sensitive
   if ((p = strstr(&(str[bytepos]), substr)) != (char *)NULL) {
     pos = 1 + ksu_bytes_to_charpos((unsigned char *)str, p - str);
   }
   sqlite3_result_int(context, pos);
}
