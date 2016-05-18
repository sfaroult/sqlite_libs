#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void     ora_rowidtochar(sqlite3_context * context,
                                int               argc,
                                sqlite3_value  ** argv) {

        unsigned long oid;
        char          charrowid[20];

        if (ksu_prm_ok(context, argc, argv, "rowidtochar",
                       KSU_PRM_INT_GT_0)) {
           oid = (unsigned long)sqlite3_value_int64(argv[0]);
           sprintf(charrowid, "%018lx", oid);
           sqlite3_result_text(context, charrowid, -1, SQLITE_TRANSIENT);
        }
}
