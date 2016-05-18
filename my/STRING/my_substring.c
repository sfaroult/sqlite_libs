#include <stdio.h>
#include <string.h>
#include <ksu_common.h>
#include <ksu_my.h>

extern void my_substring(sqlite3_context * context,
                         int               argc,
                         sqlite3_value ** argv) {
     unsigned char *v;
     int            from_pos;
     int            start_byte;
     int            byte_count;
     int            len = 0;
     int            clen;

     _ksu_check_arg_cnt(argc, 2, 3, "substring");
     _ksu_null_if_null_param(argc, argv);
     v = (unsigned char *)sqlite3_value_text(argv[0]);
     clen = ksu_charlen((unsigned char *)v);
     from_pos = my_value_int(argv[1], 0);
     if (argc == 3) {
       len = my_value_int(argv[2], 0);
     }
     if (!from_pos
         || ((from_pos + clen) < 0)
         || ((argc == 3) && (len <= 0))) {
       sqlite3_result_text(context, "", -1, SQLITE_STATIC);
     } else {
       if (len == 0) {
         if (from_pos < 0) {
           len = clen + from_pos;
         } else {
           len = clen - from_pos + 1;
         }
       }
       if (from_pos < 0) {
         from_pos = clen + from_pos;
       } else {
         from_pos--;
       }
       start_byte = ksu_charpos_to_bytes(v, from_pos);
       byte_count = ksu_charpos_to_bytes(&(v[start_byte]), len);
       sqlite3_result_text(context, (const char *)&(v[start_byte]),
                           byte_count, NULL);
     }
}
