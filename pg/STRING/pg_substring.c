#include <stdio.h>
#include <string.h>
#include <ksu_common.h>

extern void pg_substring(sqlite3_context * context,
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
     if (sqlite3_value_type(argv[1]) == SQLITE_INTEGER) {
       from_pos = sqlite3_value_int(argv[1]);
     } else {
       ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 2, "substring");
       return;
     }
     if (argc == 3) {
       if (sqlite3_value_type(argv[2]) == SQLITE_INTEGER) {
         len = sqlite3_value_int(argv[2]);
         if (len < 0) {
           ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT_GE_0,
                       3, "substring");
           return;
         }
       } else {
         ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 3, "substring");
         return;
       }
     } else {
       len = clen;
     }
     if (from_pos < 1) {
       if (len < clen) {
         len += (from_pos - 1);
       }
       from_pos = 1;
     }
     if (from_pos < 1) {
       sqlite3_result_text(context, "", -1, SQLITE_STATIC);
     } else {
       start_byte = ksu_charpos_to_bytes(v, from_pos - 1);
       byte_count = ksu_charpos_to_bytes(&(v[start_byte]), len);
       sqlite3_result_text(context, (const char *)&(v[start_byte]),
                           byte_count, NULL);
     }
}
