// Behavior of ORD function gotten from
// https://dev.mysql.com/doc/refman/5.0/en/string-functions.html#function_ord

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ksu_common.h>

/*
 * Returns the value of the first character
 */
extern void  my_ord(sqlite3_context * context,
                    int               argc,
                    sqlite3_value  ** argv) {
    unsigned char  *v;
    unsigned char  *p;
    unsigned int    coef = 1;
    unsigned long   val = 0;
    int             n = 0;
    int             i = 0;

    _ksu_null_if_null_param(argc, argv);
    v = (unsigned char *)sqlite3_value_text(argv[0]);
    p = v;
    if (*p >= 0xc0) { 
      // First count bytes
      n = 1;
      p++;
      while ((*p & 0xc0) == 0x80) {
        n++;
        coef *= 256;
        p++;
      }
      // Then compute value
      p = v;
      for (i = 0; i < n; i++) {
        val += (*p * coef);
        p++;
        coef /= 256;
      }
    } else {
      val += *p;
    }
    sqlite3_result_int64(context, (long)val);
}
