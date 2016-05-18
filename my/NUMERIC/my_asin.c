/*
 * Title: my_asin.c
 * 
 * Description: Implementation of the arcsin function for SQLite
 * 
 * History: March 25, 2015: Date Created
 */

#include <stdio.h>
#include <stdlib.h>
#include <ksu_common.h>
#include <ksu_my.h>
#include <math.h>

extern void     my_asin(sqlite3_context * context,
                        int               argc,
                        sqlite3_value  ** argv) {
   char   *c;
   double  value;

   //Check for null
   c = (char *)sqlite3_value_text(argv[0]);
   if (c == NULL) {
     sqlite3_result_null(context);
   } else {
     //Non null value
     // mysql issues a warning if the argument isn't a number,
     // but tries to interpret it as a number. So do we.
     value = my_value_double(argv[0], 0);
     if (value >= -1 && value <= 1) {
       sqlite3_result_double(context, asin(value));
     } else {
       sqlite3_result_null(context);
     }
   }
}
