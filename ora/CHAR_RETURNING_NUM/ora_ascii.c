#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ctype.h>

#include <ksu_common.h>

/*
 *  Returns the ascii value corresponding to the first char entered.
 *  Returns 0 if not an ASCII character
 */ 
extern void     ora_ascii(sqlite3_context * context, 
                          int               argc, 
                          sqlite3_value  ** argv) {
        unsigned char  *c;

        c = (unsigned char *)sqlite3_value_text(argv[0]);
        if (c == NULL) {
           sqlite3_result_null(context);
        } else {
           if (*c < 128) {
              sqlite3_result_int(context, (int)*c);
           } else {
              sqlite3_result_int(context, 0);
           }
        } 
} 
