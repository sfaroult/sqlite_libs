/*
 * my_period_diff.c
 * 
 * 
 * This program is used in SQLite and will return the number of months between
 * two dates
 * 
 * Input - First date, second date Output - Number of months between
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>
#include <math.h>

/*
 * Returns the number of months between two dates
 */

extern void     my_period_diff(sqlite3_context * context,
                               int               argc,
                               sqlite3_value  ** argv) {

        char           *v1;
        char           *v2;
        int             y1;
        int             m1;
        int             y2;
        int             m2;
        int             l1;
        int             l2;

        _ksu_null_if_null_param(argc, argv);
        v1 = (char *)sqlite3_value_text(argv[0]);
        v2 = (char *)sqlite3_value_text(argv[1]);
        l1 = strlen(v1);
        l2 = strlen(v2);

        //Checks the length of the inputs
        if (((l1 != 4) && (l1 != 6))
           || ((l2 != 4) && (l2 != 6))) {
          //Error
          ksu_err_msg(context, KSU_ERR_INV_PERIOD, "period_diff");
          return;
        }
        //Checks if the input is an int and the user has entered 2 dates
        if (sscanf(v1, (l1 == 6 ? "%4d%2d" : "%2d%2d"), &y1, &m1) == 2
            && sscanf(v2, (l2 == 6 ? "%4d%2d" : "%2d%2d"), &y2, &m2) == 2) {
          if ((m1 < 1) || (m1 > 12)) {
            ksu_err_msg(context, KSU_ERR_INV_MONTH,
                        (l1 == 6 ? &(v1[4]) : &(v1[2])),
                        "period_diff");
            return;
          }
          if ((m2 < 1) || (m2 > 12)) {
            ksu_err_msg(context, KSU_ERR_INV_MONTH,
                        (l2 == 6 ? &(v2[4]) : &(v2[2])),
                        "period_diff");
            return;
          }
          // Century hard-coded, not worth a call to time()
          if (l1 == 4) {
            if (y1 >= 50) {
              y1 += 1900;
            } else {
              y1 += 2000;
            }
          }
          if (l2 == 4) {
            if (y2 >= 50) {
              y2 += 1900;
            } else {
              y2 += 2000;
            }
          }
          //This is the computation
          sqlite3_result_int(context, (y1 - y2) * 12 + (m1 - m2));
        }
          //Incorrect input
          else {
          ksu_err_msg(context, KSU_ERR_INV_PERIOD, "period_diff");
        }
}
