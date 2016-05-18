/*
 * ora_cosh.c Reads in number and returns hyperbolic
 * cosine of that number.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void     ora_cosh(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
        double          number;

        if (ksu_prm_ok(context, argc, argv, "cosh", KSU_PRM_NUMERIC)) {
           number = sqlite3_value_double(argv[0]);
           sqlite3_result_double(context, cosh(number));
        }
}
