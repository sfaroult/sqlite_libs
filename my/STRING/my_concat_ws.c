/*
 * Input : select concat_ws(',','test','complete'); Output : test,complete
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>

extern void     my_concat_ws(sqlite3_context * context,
                             int               argc,
                             sqlite3_value  ** argv) {
        int             len = 0;
        //Required length
        int             seplen;
        char           *con;
        char           *result;

        _ksu_check_arg_cnt(argc, 2, -1, "concat_ws");
        //Check whether the separator is null
        if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
          sqlite3_result_null(context);
          return;
         }
        seplen = strlen((char *)sqlite3_value_text(argv[0]));
        int             j;

        for (j = 1; j < argc; j++) {
          if (sqlite3_value_type(argv[j]) != SQLITE_NULL) {
            len = len + seplen + strlen((char *)sqlite3_value_text(argv[j]));
          }
        }
        if ((result = (char *)sqlite3_malloc(len + 1)) == NULL) {
          sqlite3_result_null(context);
          return;
        }
        con = (char *)sqlite3_value_text(argv[0]);
        int x;

        result[0] = '\0';
        for (x = 1; x < argc; x++) {
          if (sqlite3_value_type(argv[x]) != SQLITE_NULL) {
            if (x > 1) {
              strcat(result, con);
            }
            strcat(result, (char *)sqlite3_value_text(argv[x]));
          }
        }
        sqlite3_result_text(context,
                            (char *)result,
                            -1,
                            sqlite3_free);
}
