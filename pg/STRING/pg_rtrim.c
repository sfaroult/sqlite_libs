#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ksu_common.h>

extern void pg_rtrim(sqlite3_context * context, 
                     int               argc, 
                     sqlite3_value  ** argv) {

    char          *trim_what = (char *)NULL;
    char          *object = (char *)NULL;
    unsigned char *p;
    unsigned char *q;
    int            nbytes;
    char           utf8[5];
    char           flop = 0;

    // 1 parameter: right trim spaces from parameter
    // 2 parameters: right trim letters in 2nd parameter from 1st one
    _ksu_check_arg_cnt(argc, 1, 2, "rtrim");
    _ksu_null_if_null_param(argc, argv);
    object = (char *)sqlite3_value_text(argv[0]);
    if (argc == 1) {
      trim_what = " ";
    } else {
      trim_what = (char *)sqlite3_value_text(argv[1]);
    }
    p = (unsigned char *)object;
    if (*p) {
      q = &(p[strlen((char *)p)]);  // Point on \0
      _ksu_utf8_decr(q);
      if (q > p) {
        if ((nbytes = ksu_is_utf8((const unsigned char *)q)) == 0) {
          flop++;
        }
        if (!flop) {
          strncpy(utf8, (char *)q, nbytes);
          utf8[nbytes] = '\0';
          while ((q > p) && !flop && (strstr(trim_what, utf8) != NULL)) {
            _ksu_utf8_decr(q);
            if ((nbytes = ksu_is_utf8((const unsigned char *)q)) == 0) {
              flop++;
            } else {
              strncpy(utf8, (char *)q, nbytes);
              utf8[nbytes] = '\0';
            }
          }
        }
        sqlite3_result_text(context, (char *)p, nbytes + q - p, NULL);
      } else {
        sqlite3_result_text(context, (char *)p, -1, NULL);
      }
    } else {
      sqlite3_result_text(context, "", 0, NULL);
    }
}
