/*
 * Input : select exp(3); Output : 20.0855369231877
 * 
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>
#include <math.h>

extern void my_exp(sqlite3_context * context, 
                   int               argc, 
                   sqlite3_value  ** argv) {
       double          val;
       int             typ;
                
       _ksu_null_if_null_param(argc, argv);
       typ = sqlite3_value_type(argv[0]);
       if ((typ == SQLITE_INTEGER)
           || (typ == SQLITE_FLOAT)) {
          val = sqlite3_value_double(argv[0]);
       } else {
          val = 0;
       }
       val = exp(val);
       if ((long)val == val) {
         sqlite3_result_int64(context, (long)val);
       } else {
         sqlite3_result_double(context, val);
       } 
} 
