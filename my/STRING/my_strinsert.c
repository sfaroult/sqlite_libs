#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>
#include <ksu_my.h>

extern void  my_strinsert(sqlite3_context * context,
                          int               argc,
                          sqlite3_value  ** argv) {
     unsigned char *result;
     int            resultlen;
     unsigned char *base;
     int            baselen;
     unsigned char *graft;
     unsigned char *g;
     unsigned char *b;
     unsigned char *r;
     int            pos;
     int            len;
     int            i;

     _ksu_null_if_null_param(argc, argv);
     base = (unsigned char *)sqlite3_value_text(argv[0]);
     graft = (unsigned char *)sqlite3_value_text(argv[3]);
     pos = my_value_int(argv[1], -1);
     if (pos < 1) {
       sqlite3_result_text(context, (char *)base, -1, NULL);
       return;
     }
     len = my_value_int(argv[2], 0);
     baselen = ksu_charlen(base);
     // Compute result length (in chars)
     if ((len < 0) || (len > baselen)) {
       resultlen = pos + ksu_charlen(graft);
       len = -1;
     } else {
       resultlen = baselen - len + ksu_charlen(graft);
     }
     if ((result = (unsigned char *)sqlite3_malloc(4 * resultlen + 1))
             == (unsigned char *)NULL) {
       sqlite3_result_error_nomem(context);
       return;
     }
     b = base;
     g = graft;
     r = result;
     for (i = 0; i < pos - 1; i++) {
       _ksu_utf8_copychar(b, r);
     }
     for (i = 0; i < ksu_charlen(graft); i++) {
       _ksu_utf8_copychar(g, r);
     }
     for (i = 0; i < len; i++) {
       SQLITE_SKIP_UTF8(b);
     }
     if (len >= 0) {
       for (i = 0; i < baselen - len - pos + 2; i++) {
         _ksu_utf8_copychar(b, r);
       }
     } else {
       *r = '\0';
     }
     sqlite3_result_text(context, (char *)result, -1, sqlite3_free);
}
