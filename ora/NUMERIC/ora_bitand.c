#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

extern void     ora_bitand(sqlite3_context * context, 
                           int               argc, 
                           sqlite3_value  ** argv) {
        char            n1;
        char            n2;
                
        if (ksu_prm_ok(context, argc, argv, "bitand",
                       KSU_PRM_INT_GE_0,
                       KSU_PRM_INT_GE_0)) {
           n1 = sqlite3_value_int(argv[0]);
           n2 = sqlite3_value_int(argv[1]);
           sqlite3_result_int(context, (n1 & n2));
                
        } 
} 
