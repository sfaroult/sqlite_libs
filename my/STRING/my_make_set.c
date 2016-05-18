#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>
#include <ksu_my.h>

#define CHUNK    50

extern void  my_make_set(sqlite3_context * context, 
                         int               argc, 
                         sqlite3_value  ** argv) {
    int          i;
    int          typ;
    char        *result = (char *)NULL;
    int          resultsz = 0;
    int          resultlen = 0;
    int          len;
    unsigned int mask;
                
    _ksu_check_arg_cnt(argc, 2, -1, "make_set");
    typ = sqlite3_value_type(argv[0]);
    // Returns null when the first parameter is null
    // Other nulls are ignored
    if (typ == SQLITE_NULL) {
      sqlite3_result_null(context);
      return;
    }
    mask = (unsigned int)my_value_int(argv[0], 0);
    if ((result = (char *)sqlite3_malloc(CHUNK)) == (char *)NULL) {
       sqlite3_result_error_nomem(context);
       return;
    }
    resultsz = CHUNK;
    *result = '\0';
    resultlen = 0;
    for (i = 1; i < argc; i++) {
      if (mask & 0x01) {
        typ = sqlite3_value_type(argv[i]);
        if (typ != SQLITE_NULL) {
          len = strlen((char *)sqlite3_value_text(argv[i]));
          while ((resultlen + len + 2) > resultsz) {
            if ((result = (char *)sqlite3_realloc(result, (resultsz + CHUNK)))
                  == (char *)NULL) {
              sqlite3_result_error_nomem(context);
              return;
            }
            resultsz += CHUNK;
          }
          strcat(result, (char *)sqlite3_value_text(argv[i]));
          strcat(result, ",");
          resultlen += (len + 1);
        }
      }
      mask = mask >> 1;
    }
    while (result[resultlen-1] == ',') {
      resultlen--;
    }
    result[resultlen] = '\0';
    sqlite3_result_text(context, result, -1, sqlite3_free);
} 
