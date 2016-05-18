#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>
#include <ksu_my.h>
#include <math.h>

/*
 * IF(expression, string_or_number_if_true, string_or_number_if_false)
 * 
 * The IF() fuction takes three parameters. If the first parameter evaluates to
 * true, then the second parameter is printed. If the first parameter is
 * false, then the third parameter is printed.
 * 
 * expression refers to the expression that the user would like to be evaluated
 * -- MUST EVALUATE TO A BOOLEAN.
 * 
 * string_or_number_if_true is an integer or a string that will be printed if
 * the expression is true.
 * 
 * string_or_number_if_false is an integer or a string that will be printed if
 * the expression is false.
 */
extern void my_if(sqlite3_context * context,
                  int               argc,
                  sqlite3_value  ** argv) {
    char  *yesIfString;
    char  *noIfString;
    int    state;

    _ksu_null_if_null_param(argc, argv);
    state = my_value_int(argv[0], 0);
    if (state) {
      yesIfString = (char *)sqlite3_value_text(argv[1]);
      sqlite3_result_text(context, yesIfString, -1, NULL);
    } else {
      noIfString = (char *)sqlite3_value_text(argv[2]);
      sqlite3_result_text(context, noIfString, -1, NULL);
    }
}
