#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ksu_common.h>

extern void pg_mod(sqlite3_context  *context,
                   int               argc,
                   sqlite3_value   **argv) {
   double modOf;
   double modBy;
   double result;
   short  mult = 1;

   if (ksu_prm_ok(context, argc, argv, "mod",
                  KSU_PRM_NUMERIC, KSU_PRM_NUMERIC)) {
      modOf = sqlite3_value_double(argv[0]);
      if (modOf < 0) {
        modOf *= -1;
        mult = -1;
      }
      modBy = sqlite3_value_double(argv[1]);
      if (modBy == (double)0) {
        result = modOf;
      } else {
        if (modBy < 0) {
          modBy *= -1;
        }
        result = modOf - modBy * floor(modOf/modBy);
      }
      if (result == (long)result) {
        sqlite3_result_int(context, (long)mult*result);
      } else {
        sqlite3_result_double(context, mult*result);
      }
   }
}
