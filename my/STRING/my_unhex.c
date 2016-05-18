#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>
#include <math.h>

extern void     my_unhex(sqlite3_context * context, 
                         int               argc, 
                         sqlite3_value  ** argv) {
           unsigned char *s;
           char          *p;
           int            len;
           unsigned char *result;
           int            rlen;
           int            i;
           int            start = 0;
    static char          *valid = "0123456789ABCDEFabcdef";
        
    _ksu_null_if_null_param(argc, argv);
    s = (unsigned char *)sqlite3_value_text(argv[0]);
    p = (char *)s;
    while (*p) {
      if (strchr(valid, *p) == (char *)NULL) {
         sqlite3_result_null(context);
         return;
      }
      p++;
    }
    len = strlen((char *)s);
    rlen = (int)(0.5 + len / 2.0);
    if ((result = (unsigned char *)sqlite3_malloc(rlen))
            == (unsigned char *)NULL) {
      sqlite3_result_error_nomem(context);
      return;
    }
    if (len % 2) {
      // MySQL prepends a 0 if the number of characters
      // is odd
      char first[2];
      first[0] = '0';
      first[1] = *s;
      (void)sscanf(first, "%02X", (unsigned int *)&(result[0]));
      s++;
      start = 1;
    }
    for (i = start; i < rlen; i++) {
      (void)sscanf((char *)s, "%02X", (unsigned int *)&(result[i]));
      s += 2;
    }
    sqlite3_result_text(context, (char *)result, rlen, sqlite3_free);
}
