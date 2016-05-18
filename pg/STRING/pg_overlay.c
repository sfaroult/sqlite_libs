#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>

extern void pg_overlay(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
        unsigned char  *from_string;
        unsigned char  *f;
        int             from_len;
        unsigned char  *overlay;
        unsigned char  *o;
        int             overlay_len;
        unsigned char  *result;
        unsigned char  *r;
        int             result_len;
        int             from_char;
        int             suppr_len;
        int             i;

        _ksu_check_arg_cnt(argc, 3, 4, "overlay");
        _ksu_null_if_null_param(argc, argv);
        // 3rd parameter must be a strictly positive integer
        // 4th parameter can be negative, it which case it 
        // just prefixes the string and returns something as
        // long as the two strings concatenated
        from_string = (unsigned char *)sqlite3_value_text(argv[0]);
        from_len = ksu_charlen(from_string);
        overlay = (unsigned char *)sqlite3_value_text(argv[1]);
        overlay_len = ksu_charlen(overlay);
        if (sqlite3_value_type(argv[2]) != SQLITE_INTEGER) {
          ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT_GT_0,
                      3, "overlay");
          return;
        }
        from_char = sqlite3_value_int(argv[2]);
        if (from_char < 1) {
          ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT_GT_0,
                      3, "overlay");
          return;
        }
        if (argc == 4) {
          if (sqlite3_value_type(argv[3]) != SQLITE_INTEGER) {
            ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT,
                        4, "overlay");
            return;
          }
          suppr_len = sqlite3_value_int(argv[3]);
        } else {
          suppr_len = overlay_len;
        }
        /*
         *   Length of the result: complicated.
         *   overlay('hello' placing 'world' from 2)  ->hworld
         *       from 3 ->heworld
         *       from 10 ->helloworld
         *   overlay('hello' placing 'world' from 2 for 2) ->hworldo
         *       for 4  ->hworld
         *       for 10 ->hworld
         *       for 0 ->hworldello
         *       for -1 ->hworldhello
         *       for -10 ->hworldhello
         *       from 4 for -10 ->helworldhello
         *       from 10 for -10 ->helloworldhello
         *       from 10 for 1 ->helloworld
         *       from 4 for 1 ->helworldo
         */
        // Result length in CHARs
        result_len = from_char
                     + from_len - (suppr_len > 0 ? suppr_len : 0)
                     + overlay_len
                     + (suppr_len < 0 ? from_len : 0);
        result = (unsigned char *)sqlite3_malloc(result_len * 4);
        f = from_string;
        o = overlay;
        r = result;
        i = 1;
        while (*f && (i < from_char)) {
          _ksu_utf8_copychar(f, r);
          i++;
        }
        for (i = 1; i <= overlay_len; i++) {
          _ksu_utf8_copychar(o, r);
        }
        i = 1;
        while (*f && (i <= suppr_len)) {
          SQLITE_SKIP_UTF8(f);
          i++;
        }
        if (suppr_len < 0) {
          f = from_string;
          while (*f) {
            _ksu_utf8_copychar(f, r);
          }
        } else {
          while (*f) {
            _ksu_utf8_copychar(f, r);
          }
        }
        *r = '\0';
        sqlite3_result_text(context, (char *)result, -1, sqlite3_free);
}
