#include <stdio.h>
#include <math.h>

#include <ksu_common.h>

extern void pg_trunc(sqlite3_context  *context,
                     int argc,
                     sqlite3_value   **argv) {
   int         typ;
   double      val;
   double      rval;
   int         rvalue = 0;
   char        neg = 0;

   _ksu_check_arg_cnt(argc, 1, 2, "trunc");
   // Check whether at least one parameter is null
   if (((typ = sqlite3_value_type(argv[0])) == SQLITE_NULL)
        || ((argc == 2) && (sqlite3_value_type(argv[1]) == SQLITE_NULL))) {
      sqlite3_result_null(context);
      return;
   }
   // Check whether we have a number
   switch (typ) {
     case SQLITE_INTEGER :
     case SQLITE_FLOAT :
          if (argc == 2) {
             if (sqlite3_value_type(argv[1]) != SQLITE_INTEGER) {
                ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 2, "trunc");
                return;
             }
             rvalue = sqlite3_value_int(argv[1]);
          }
          val = sqlite3_value_double(argv[0]);
          if (val == (double)0) {
            sqlite3_result_int(context, 0);
          } else {
            if (val < 0) {
              neg = 1;
              val *= -1;
            }
            // The following is the formula provided 
            // in the Oracle docs (plus sign handling)
            rval = floor(val * pow(10, rvalue))
                   * pow(10, -rvalue) * (neg ? -1 : 1);
            if (rval == (long)rval) {
              sqlite3_result_int(context, (long)rval);
            } else {
              sqlite3_result_double(context, rval);
            }
          }
          break;
     default :
          // Contrary to Oracle, PostgreSQL has another function
          // for truncating dates (date_trunc()) 
          ksu_err_msg(context, KSU_ERR_ARG_N_NOT_NUM, 1, "trunc");
          break;
   }
}

