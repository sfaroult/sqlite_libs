/*
 * 
 * Input : select power(2,3); Output : 8.0
 * 
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

// POWER(n2, n1)
// POWER returns n2 raised to the n1 power. The base n2 and the exponent
// n1 can be any numbers, but if n2 is negative, then n1 must be an integer.
extern void     ora_power(sqlite3_context * context, 
                          int               argc, 
                          sqlite3_value  ** argv) {
        double          val;
        double          val2;
                
        if (ksu_prm_ok(context, argc, argv, "power",
                       KSU_PRM_NUMERIC, KSU_PRM_NUMERIC)) {
           val = sqlite3_value_double(argv[0]);
           val2 = sqlite3_value_double(argv[1]);
           if (val < 0) {
              if (sqlite3_value_numeric_type(argv[1]) != SQLITE_INTEGER) {
                 ksu_err_msg(context, KSU_ERR_INV_POWER);
                 return;
              }
           } 
           val2 = sqlite3_value_double(argv[1]);
           sqlite3_result_double(context, pow(val, val2));
        } 
} 
