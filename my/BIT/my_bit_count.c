#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

/*
 * unsigned int v; // count the number of bits set in v
 * unsigned int c; // c accumulates the total bits set in v
 * for (c = 0; v; c++)
 * {
 *   v &= v - 1; // clear the least significant bit set
 *   }
 */
extern void my_bit_count(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
    int             i;
    char           *binstring;
    unsigned char  *val;
    int             sz;
    unsigned int    v;
    unsigned int    cnt = 0;
    unsigned int    c;
    int             ok = 1;
        
    _ksu_null_if_null_param(argc, argv);
    binstring = (char *)sqlite3_value_text(argv[0]);
    if (*binstring == 'b') {
      binstring++;
      while (ok && *binstring) {
        switch(*binstring) {
           case '0':
                break;
           case '1':
                cnt++;
                break;
           default: // Not a binary string representation
                cnt = 0;
                ok = 0;
                break;
        }
        binstring++;
      }
      if (ok) {
        sqlite3_result_int(context, cnt);
        return;
      }
    }
    if (sqlite3_value_type(argv[0]) == SQLITE_INTEGER) {
       v = (unsigned int)sqlite3_value_int(argv[0]);
       // Brian Kernighan's method
       for (cnt = 0; v; cnt++) {
          v &= v - 1; // clear the least significant bit set
       }
    } else {
       sz = sqlite3_value_bytes(argv[0]);
       val = (unsigned char *)sqlite3_value_blob(argv[0]);
       for (i = 0; i < sz; i++) {
         v = (unsigned int)val[i];
         for (c = 0; v; c++) {
           v &= v - 1;
         }
         cnt += c;
       }
    }
    sqlite3_result_int(context, cnt);
} 
