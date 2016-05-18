#include <stdio.h>
#include <stdlib.h>
#include <ksu_common.h>
#include <ksu_my.h>
#include <math.h>

/*
 * Returns the position of the second argument in the first one, 0 if not
 * found.
 */
extern void  my_atan(sqlite3_context * context,
                     int               argc,
                     sqlite3_value  ** argv) {
    double    val;
    int       typ;

    typ = sqlite3_value_type(argv[0]);
    if (typ == SQLITE_NULL) {
      sqlite3_result_null(context);
    } else {
      val = my_value_double(argv[0], 0);
      sqlite3_result_double(context, atan(val));
    }
}
