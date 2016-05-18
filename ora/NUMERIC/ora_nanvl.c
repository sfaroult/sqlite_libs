#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void ora_nanvl(sqlite3_context  *context,
                      int               argc,
                      sqlite3_value   **argv) {
   int     typ;
   double  val; 

   if (ksu_prm_ok(context, argc, argv, "nanvl",
                  KSU_PRM_TEXT, KSU_PRM_NUMERIC)) {
     typ = sqlite3_value_type(argv[0]);
     if ((typ == SQLITE_INTEGER)
         || (typ == SQLITE_FLOAT)) {
       val = sqlite3_value_double(argv[0]);
     } else {
       val = sqlite3_value_double(argv[1]);
     }
     if (val == (long)val) {
       sqlite3_result_int(context, (long)val);
     } else {
       sqlite3_result_double(context, val);
     }
   }
}
