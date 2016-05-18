#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ksu_common.h>

/*
 * The FLOOR function rounds the specified number down, and returns the
 * largest number that is less than or  equal to the specified number.
 * 
 */ 

extern void  pg_floor(sqlite3_context * context, 
                      int               argc, 
                      sqlite3_value  ** argv) {
        double          val;

        if (ksu_prm_ok(context, argc, argv, "floor", KSU_PRM_NUMERIC)) {
           val = sqlite3_value_double(argv[0]);
           sqlite3_result_int(context, (long)floor(val));
        } 
} 
