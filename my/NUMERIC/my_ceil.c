#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>
#include <ksu_my.h>
#include <math.h>

extern void     my_ceil(sqlite3_context * context, 
                        int               argc, 
                        sqlite3_value  ** argv) {
    double  val;
    int     typ;
        
    typ = sqlite3_value_type(argv[0]);
    if (typ == SQLITE_NULL) {
      sqlite3_result_null(context);
    } else {
      if ((typ == SQLITE_INTEGER)
          || (typ == SQLITE_FLOAT)) {
        val = sqlite3_value_double(argv[0]);
        sqlite3_result_int64(context, (long)ceil(val));
      } else {
        //Wrong input 
        sqlite3_result_int(context, 0);
      } 
    } 
} 
