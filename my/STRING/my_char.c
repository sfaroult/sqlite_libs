#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>

#define CHUNK  30

static void add_char(int c, int pos,
                     unsigned char **str_ptr, int *sz_ptr) {
   unsigned char *s;
   if (str_ptr && sz_ptr) {
     if (*str_ptr) {
       if (pos == *sz_ptr) {
         *str_ptr = (unsigned char *)sqlite3_realloc(*str_ptr,
                                                     *sz_ptr + CHUNK + 1);
         if (*str_ptr) {
           *sz_ptr += CHUNK;
         }
       }
     } else {
       *str_ptr = (unsigned char *)sqlite3_malloc(CHUNK + 1);
       if (*str_ptr) {
         *sz_ptr = CHUNK;
       }
     }
     s = *str_ptr;
     if (s) {
       s[pos] = (unsigned char)c;
     }
   }
}

extern void     my_char(sqlite3_context * context,
                        int               argc,
                        sqlite3_value  ** argv) {
   unsigned char  *result = (unsigned char *)NULL;
   int             resultlen = 0;
   int             i;
   int             j;
   int             k;
   union {
          unsigned int n;
          unsigned char b[4];
         }         val;
   int             typ;

   _ksu_check_arg_cnt(argc, 1, -1, "char");
   // Uncharacteristically, NULLs are ignored. The result
   // is null only when all parameters are null.
   j = 0;
   for (i = 0; i < argc; i++) {
     typ = sqlite3_value_type(argv[i]);
     if (typ != SQLITE_NULL) {
       val.n = (unsigned int)sqlite3_value_int(argv[i]);
       // 0 will become a space
       // Non-printable characters are ignored
       if (val.n == 0) {
         add_char((int)' ', j, &result, &resultlen);
         j++;
       } else {
         if (val.n > 256) {
           for (k = 0; k < 4; k++) {
             if (val.b[k] != 0) {
               add_char((int)val.b[k], j, &result, &resultlen);
               j++;
             }
           }
         } else {
           add_char((int)val.n, j, &result, &resultlen);
           j++;
         }
       }
     }
   }
   if (j == 0) {
     sqlite3_result_null(context);
     return;
   }
   add_char('\0', j, &result, &resultlen);
   sqlite3_result_text(context, (char *)result, -1, sqlite3_free);
}
