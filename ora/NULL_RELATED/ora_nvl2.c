#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <ksu_common.h>

/*
 * NVL2 lets you determine the value returned by a query based on whether a
 * specified expression is null or not null. If expr1 is not null, then NVL2
 * returns expr2. If expr1 is null, then NVL2 returns expr3.
 */
extern void     ora_nvl2(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
       //Check if the expr1 is not NULL
       if (sqlite3_value_type(argv[0]) != SQLITE_NULL) {
          //return expr2
          sqlite3_result_value(context, argv[1]);
       }
       //expr1 is NULL, return expr3
       else {
          sqlite3_result_value(context, argv[2]);
       }
}
