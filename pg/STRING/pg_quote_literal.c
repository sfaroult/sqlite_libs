#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ksu_common.h>

#define  SAFETY_MARGIN    10

/*
 *   Doesn't exactly behaves as the PostgreSQL version as a \
 *   leaves sqlite completely cold. Serves, however, the
 *   same purpose, which is making data extracted from the
 *   database usable in a statement.
 */
extern void pg_quote_literal(sqlite3_context * context,
                             int               argc,
                             sqlite3_value  ** argv) {
  int   len;
  char *str;
  char *result = (char *)NULL;
  int   resultsz;
  int   i;
  int   j;

  _ksu_null_if_null_param(argc, argv);
  len = sqlite3_value_bytes(argv[0]);
  str = (char *)sqlite3_value_blob(argv[0]);
  if ((result = (char *)sqlite3_malloc(len + SAFETY_MARGIN + 1))
        == (char *)NULL) {
    sqlite3_result_error_nomem(context);
    return;
  }
  resultsz = len + SAFETY_MARGIN;
  *result = '\'';
  j = 1;
  for (i = 0; i < len; i++) {
    switch (str[i]) {
      case '\'':     // Escape quote
           result[j++] = '\'';
           break;
      default :
           break;
    }
    result[j++] = str[i];
    if (j >= resultsz - 2) { // Time to realloc
      if ((result = (char *)sqlite3_realloc(result,
                               resultsz + SAFETY_MARGIN + 1))
            == (char *)NULL) {
        sqlite3_result_error_nomem(context);
        return;
      }
      resultsz += SAFETY_MARGIN;
    }
  }
  result[j++] = '\'';
  sqlite3_result_text(context, result, j, sqlite3_free);
}
