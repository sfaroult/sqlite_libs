#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ksu_common.h>
#include <ksu_my.h>

extern void my_locate(sqlite3_context  *context,
                      int               argc,
                      sqlite3_value   **argv) {
   char *str;
   char *p;
   char *substr;
   int   pos = 0;
   int   frompos = 1;
   int   bytepos = 1;
   
   _ksu_check_arg_cnt(argc, 2, 3, "locate");
   _ksu_null_if_null_param(argc, argv);
   substr = (char *)sqlite3_value_text(argv[0]);
   str = (char *)sqlite3_value_text(argv[1]);
   if (argc == 3) {
     frompos = my_value_int(argv[2], 0);
   }
   if ((frompos < 1)
       || (frompos > ksu_charlen((unsigned char *)str))) {
     sqlite3_result_int(context, 0);
     return;
   }
   bytepos = ksu_lax_charpos_to_bytes((unsigned char *)str, frompos - 1);
   if ((p = strcasestr(&(str[bytepos]), substr)) != (char *)NULL) {
     pos = 1 + ksu_bytes_to_charpos((unsigned char *)str, p - str);
   }
   sqlite3_result_int(context, pos);
}
