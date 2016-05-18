#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>

extern void my_concat(sqlite3_context * context,
                      int               argc,
                      sqlite3_value  ** argv) {
        unsigned char  *result;
        unsigned int    stringLength = 0;
        int             i;
        int             j;
        const char     *inStr;
        unsigned char  *temp;

        _ksu_check_arg_cnt(argc, 1, -1, "concat");
        for (i = 0; i < argc; i++) {
          //Check whether the current parameter is null
          if (sqlite3_value_type(argv[i]) == SQLITE_NULL) {
            sqlite3_result_null(context);
            return;
          }
          inStr = (const char *)sqlite3_value_text(argv[i]);
          stringLength += strlen(inStr);
        }
        result = (unsigned char *)sqlite3_malloc((stringLength + 1)
                                                  * sizeof(char));
        temp = result;
        for (j = 0; j < argc; j++) {
          inStr = (const char *)sqlite3_value_text(argv[j]);
          memcpy(result, inStr, (strlen(inStr) * sizeof(char)));
          result += strlen(inStr) * sizeof(char);
        }
        *(result) = '\0';
        result = temp;
        sqlite3_result_text(context, (char *)result, -1, sqlite3_free);
}
