/*
 * A very basic implementation of mysql's sin function.
 * 
 * http://dev.mysql.com/doc/refman/5.7/en/mathematical-functions.html
 * 
 * It expects to be given a single integer or a single double. It uses radians
 * and not degrees.
 */
#include <stdio.h>
#include <math.h>
#include <ksu_common.h>

extern void     my_sin(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {

        int     valueType = sqlite3_value_type(argv[0]);
        double  value;
        
        if (valueType == SQLITE_NULL) {
          sqlite3_result_null(context);
        } else {
          if (valueType != SQLITE_INTEGER && valueType != SQLITE_FLOAT) {
            sqlite3_result_int(context, 0);
          } else {
            value = sqlite3_value_double(argv[0]);
            sqlite3_result_double(context, sin(value));
          }
        }
}
