/*
 * my_elt.c Reads in index and list of strings and
 * returns string at the index number.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>

extern void my_elt(sqlite3_context * context,
                   int               argc,
                   sqlite3_value  ** argv) {
    int      index;
    int      typ;
    char    *n;

    _ksu_check_arg_cnt(argc, 2, -1, "elt");
    typ = sqlite3_value_type(argv[0]);
    if (typ == SQLITE_NULL) {
      sqlite3_result_null(context);
      return;
    }
    if ((typ != SQLITE_INTEGER)
        && (typ != SQLITE_FLOAT)) {
      sqlite3_result_null(context);
      return;
    }
    if (typ == SQLITE_FLOAT) {
      index = (int)(0.5 + sqlite3_value_double(argv[0]));
    } else {
      index = sqlite3_value_int(argv[0]);
    }
    if (index <= 0 || index >= argc) {
      sqlite3_result_null(context);
      return;
    }
    n = (char *)sqlite3_value_text(argv[index]);
    sqlite3_result_text(context, n, -1, NULL);
}
