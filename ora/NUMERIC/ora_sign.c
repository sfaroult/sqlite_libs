#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <ksu_common.h>

/*
 * The SIGN function returns a value indicating the sign of a number. If
 * number < 0, then sign returns -1. If number = 0, then sign returns 0. If
 * number > 0, then sign returns 1.
 */
extern void     ora_sign(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
        double          value;

        if (ksu_prm_ok(context, argc, argv, "sign", KSU_PRM_NUMERIC)) {
           value = sqlite3_value_double(argv[0]);

           int             result = 0;

           if (value < 0) {
              result = -1;
           } else if (value > 0) {
              result = 1;
           }
           //Return the result
           sqlite3_result_int(context, result);
        }
}
