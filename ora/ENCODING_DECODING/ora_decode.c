/*
 * http://docs.oracle.com/cd/B19306_01/server.102/b14200/functions040.htm
 * 
 * First argument should be some value or type integer, double, or text. It is
 * then followed by an even number of arguments (making the total number of
 * arguments odd), which are logically thought to be in pairs. The first of
 * each pair is of type integer, double, or text, and is compared to the
 * very first argument. If the two are equal, then the second of that pair
 * (again either integer, double, or text) is returned. If no match is found,
 * NULL is returned.
 * 
 * given: "a", 4.1,"orange", 11,"grape", "a","apple" return: "apple"
 * ^^^^^^^^^^^
 * 
 * given: 3.14, "pi","3.14", 3.14,"pi" return: "pi"              ^^^^^^^^^
 */
#include <stdio.h>
#include <string.h>

#include <ksu_common.h>

extern void ora_decode(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {

     _ksu_check_arg_cnt(argc, 3, -1, "decode");
     // One of the rare functions that can take a null
     // and return something not null
     int  dataType = sqlite3_value_type(argv[0]);
     int  i;
     int  n;
     char match = 0;

     if (dataType == SQLITE_BLOB) {
        // Unsupported
        ksu_err_msg(context, KSU_ERR_ARG_N_BINARY, 1, "decode");
        return;
     }
     i = 1;
     while (!match && (i < argc)) {
       if (dataType == sqlite3_value_type(argv[i])) {
          switch (dataType) {
             case SQLITE_NULL:
                  match = 1;
                  break;
             case SQLITE_INTEGER:
                  if (sqlite3_value_int(argv[0])
                          == sqlite3_value_int(argv[i])) {
                    match = 1;
                  }
                  break;
             case SQLITE_FLOAT:
                  if (sqlite3_value_double(argv[0])
                          == sqlite3_value_double(argv[i])) {
                    match = 1;
                  }
                  break;
             case SQLITE_TEXT:
                  if (!strcmp((char *)sqlite3_value_text(argv[0]),
                              (char *)sqlite3_value_text(argv[i]))) {
                    match = 1;
                  }
                  break;
             default :
                  break;
          }
       }
       if (!match) {
         i += 2;
       }
     }
     if (match) {
       n = i + 1;
     } else {
       if (argc % 2) {
          sqlite3_result_null(context);
          return;
       }
       n = argc - 1;
     }
     dataType = sqlite3_value_type(argv[n]);
     if (dataType == SQLITE_BLOB) {
        // Unsupported
        ksu_err_msg(context, KSU_ERR_ARG_N_BINARY, n-1, "decode");
        return;
     }
     switch (dataType) {
        case SQLITE_NULL:
             sqlite3_result_null(context);
             break;
        case SQLITE_INTEGER:
             sqlite3_result_int(context,
                                sqlite3_value_int(argv[n]));
             break;
        case SQLITE_FLOAT:
             sqlite3_result_double(context,
                                   sqlite3_value_double(argv[n]));
             break;
        default: //  SQLITE_TEXT:
             sqlite3_result_text(context,
                                 (char *)sqlite3_value_text(argv[n]),
                                 -1, NULL);
             break;
     }
}
