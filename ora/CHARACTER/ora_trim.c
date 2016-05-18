#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>

#define TRIM_LEADING  1
#define TRIM_TRAILING 2

extern void ora_trim(sqlite3_context * context, 
                     int               argc, 
                     sqlite3_value  ** argv) {

    short trimming = TRIM_LEADING | TRIM_TRAILING;    
    char *trim_what = (char *)NULL;
    char *object = (char *)NULL;
    char *p;
    char *q;
    int   trimlen;

    // 1 parameter: trim (both) spaces from parameter
    // 2 parameters: trim (both ends) 1st parameter from 2nd one
    // 3 parameters: 1st parameter must be "both", "leading"
    //               or "trailing", 2nd what is to trim, 3rd one
    //               the object
    _ksu_check_arg_cnt(argc, 1, 3, "trim");
    switch (argc) {
      case 1:
           _ksu_null_if_null_param(argc, argv);
           object = (char *)sqlite3_value_text(argv[0]);
           trim_what = " ";
           trimlen = 1;
           break;
      case 2:
           _ksu_null_if_null_param(argc, argv);
           trim_what = (char *)sqlite3_value_text(argv[0]);
           trimlen = strlen(trim_what);
           object = (char *)sqlite3_value_text(argv[1]);
           break;
      case 3:
           // NULL is a syntax error for the first parameter
           p = (char *)sqlite3_value_text(argv[0]);
           if (p == NULL) {
             ksu_err_msg(context, KSU_ERR_SYNTAX_ERROR_NEAR,
                         "NULL", "trim");
             return;
           }
           if (strcasecmp(p, "both") == 0) {
             trimming = TRIM_LEADING | TRIM_TRAILING;    
           } else if (strcasecmp(p, "leading") == 0) {
             trimming = TRIM_LEADING;    
           } else if (strcasecmp(p, "trailing") == 0) {
             trimming = TRIM_TRAILING;    
           } else {
             ksu_err_msg(context, KSU_ERR_SYNTAX_ERROR_NEAR,
                         p, "trim");
             return;
           }
           if ((sqlite3_value_type(argv[1]) == SQLITE_NULL)
              || (sqlite3_value_type(argv[2]) == SQLITE_NULL)) {
              sqlite3_result_null(context);
              return;
           }
           trim_what = (char *)sqlite3_value_text(argv[1]);
           trimlen = strlen(trim_what);
           object = (char *)sqlite3_value_text(argv[2]);
           break;
      default:
           break;
    }
    p = object;
    if (trimming & TRIM_LEADING) {
      while (strncmp(p, trim_what, trimlen) == 0) {
        p += trimlen;
      }
    }
    q = &(p[strlen(p)]);
    if (trimming & TRIM_TRAILING) {
      while (strncmp(q - trimlen, trim_what, trimlen) == 0) {
        q -= trimlen;
      }
    }
    sqlite3_result_text(context, p, q - p, NULL);
}
