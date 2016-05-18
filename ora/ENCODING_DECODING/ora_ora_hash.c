#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
 
#include <ksu_common.h>

/*
 *  Computes a hash value for a given expression.
 *  This function returns a * number value.
 *   ora_hash(expr,max_bucket,seed_value)
 *   expr determines the * data that is needed to compute a hash value
 *   max_bucket determines max_bucket value returned by hash function.
 *   0 - 4294967295 seed_value
 */ 

/*
 *  djb2 algorithm found at http://www.cse.yorku.ca/~oz/hash.html
 *  (code by Dan Bernstein)
 */
static unsigned long hash(unsigned char *str) {
   unsigned long hash = 5381;

   int c;
   while ((c = *str++) != '\0') {
     hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
   }
   return hash;
}

extern void ora_ora_hash(sqlite3_context * context, 
                         int               argc, 
                         sqlite3_value  ** argv) {
    long max_buckets = 4294967295;
    long seed_value = 0;
    long hash_value;

    _ksu_check_arg_cnt(argc, 1, 3, "ora_hash");
    if (ksu_prm_ok(context, argc, argv, "ora_hash",
                   KSU_PRM_TEXT, KSU_PRM_INT_GE_0, KSU_PRM_INT_GE_0)) {
       if (argc > 1) {
         max_buckets = (long)sqlite3_value_int(argv[1]);
         if (argc == 3) {
           seed_value = (long)sqlite3_value_int(argv[2]);
         }
       }
       hash_value = (hash((unsigned char *)sqlite3_value_text(argv[0]))
                    + seed_value) % max_buckets;
       sqlite3_result_int64(context, hash_value);
    }
} 
