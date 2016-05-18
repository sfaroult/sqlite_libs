#include <stdio.h>
#include <ksu_common.h>

// Takes a char and int and returns number of chars from left
// using int parameter.
extern void pg_strleft(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
    int   number;
    int   bytes;
    char *word;

    if (ksu_prm_ok(context, argc, argv, "strleft",
                   KSU_PRM_TEXT, KSU_PRM_INT)) {
      word = (char *)sqlite3_value_text(argv[0]);
      number = sqlite3_value_int(argv[1]);
      if (number <= 0) {
         // Return all but the "number" last characters
        sqlite3_result_text(context, "", -1, SQLITE_STATIC);
      } else if (number >= ksu_charlen((unsigned char *)word)) {
        sqlite3_result_text(context, word, -1, NULL);
      } else {
        bytes = ksu_charpos_to_bytes((unsigned char *)word, number);
        sqlite3_result_text(context, word, bytes, NULL);
      }
    }
}
