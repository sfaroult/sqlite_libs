#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ksu_common.h>

extern void pg_ltrim(sqlite3_context * context, 
                     int               argc, 
                     sqlite3_value  ** argv) {

    char          *trim_what = (char *)NULL;
    char          *object = (char *)NULL;
    unsigned char *p;
    int            nbytes;
    char           utf8[5];
    char           flop = 0;

    // 1 parameter: left trim spaces from parameter
    // 2 parameters: left trim letters in 2nd parameter from 1st one
    _ksu_check_arg_cnt(argc, 1, 2, "ltrim");
    _ksu_null_if_null_param(argc, argv);
    object = (char *)sqlite3_value_text(argv[0]);
    if (argc == 1) {
      trim_what = " ";
    } else {
      trim_what = (char *)sqlite3_value_text(argv[1]);
    }
    p = (unsigned char *)object;
    if ((nbytes = ksu_is_utf8((const unsigned char *)p)) == 0) {
      flop++;
    }
    if (*p && !flop) {
      strncpy(utf8, (char *)p, nbytes);
      utf8[nbytes] = '\0';
      while (*p && !flop && (strstr(trim_what, utf8) != NULL)) {
        SQLITE_SKIP_UTF8(p);
        if ((nbytes = ksu_is_utf8((const unsigned char *)p)) == 0) {
          flop++;
        } else {
          strncpy(utf8, (char *)p, nbytes);
          utf8[nbytes] = '\0';
        }
      }
    }
    sqlite3_result_text(context, (char *)p, -1, NULL);
}
