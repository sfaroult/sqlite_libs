/*
 * Purpose: input numerical value with a return of -1 if value was
 * negative 0 if value was equal to 0 1 if value was positive error message
 * if non-numerical input
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>
#include <math.h>

extern void  pg_sign(sqlite3_context * context,
                     int               argc,
                     sqlite3_value  ** argv) {
    double  val;

    if (ksu_prm_ok(context, argc, argv, "sign", KSU_PRM_NUMERIC)) {
      val = sqlite3_value_double(argv[0]);
      sqlite3_result_int(context, (val > 0) - (val < 0));
    }
}
