#include <stdio.h>
#include <ksu_common.h>
#include <ksu_my.h>

// Takes a char and int and returns number of chars from left
// using int parameter.
extern void my_strleft(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
    int   number;
    int   bytes;
    char *word;

    _ksu_null_if_null_param(argc, argv);
    word = (char *)sqlite3_value_text(argv[0]);
    number = my_value_int(argv[1], 0);
    if (number <= 0) {
      sqlite3_result_text(context, "", -1, SQLITE_STATIC);
    } else if (number >= ksu_charlen((unsigned char *)word)) {
      sqlite3_result_text(context, word, -1, NULL);
    } else {
      bytes = ksu_charpos_to_bytes((unsigned char *)word, number);
      sqlite3_result_text(context, word, bytes, NULL);
    }
}
