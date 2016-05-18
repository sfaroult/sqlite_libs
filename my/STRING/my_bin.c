#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>
#include <math.h>

extern void  my_bin(sqlite3_context * context,
                    int               argc,
                    sqlite3_value  ** argv) {
        
        int    i = 1;
        int    binaryNumber[100];
        int    v, quotient;
        int    typ;

        _ksu_null_if_null_param(argc, argv);
        typ = sqlite3_value_type(argv[0]);
        if ((typ != SQLITE_INTEGER)
            && (typ != SQLITE_FLOAT)) {
          sqlite3_result_text(context, "0", -1, SQLITE_STATIC);
          return;
        }
        v = sqlite3_value_int(argv[0]);
        quotient = v;
        while (quotient != 0) {
          binaryNumber[i++] = quotient % 2;
          quotient = quotient / 2;
        } 
        int   j;
        char  result[100];
        int   k = 0;
        
        for (j = i - 1; j > 0; j--) {
          result[k] = (binaryNumber[j] == 1 ? '1' : '0');
          k++;
        } 
        result[k] = '\0';
        sqlite3_result_text(context, result, -1, SQLITE_TRANSIENT);
} 
