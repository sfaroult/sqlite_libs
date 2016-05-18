#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ksu_common.h>
#include <ksu_my.h>


#define _max(a, b)  (a > b ? a : b)
#define _min(a, b)  (a < b ? a : b)

extern void     my_export_set(sqlite3_context * context,
                              int               argc,
                              sqlite3_value  ** argv) {
   char           *result = (char *)NULL;
   int             resultlen = 0;
   int             i;
   unsigned int    bits;
   unsigned char  *on;
   unsigned char  *off;
   unsigned char  *sep = (unsigned char *)",";
   unsigned int    number_of_bits = 64;

   _ksu_check_arg_cnt(argc, 3, 5, "export_set");
   _ksu_null_if_null_param(argc, argv);
   bits = (unsigned int)my_value_int(argv[0], 0);
   on = (unsigned char *)sqlite3_value_text(argv[1]);
   off = (unsigned char *)sqlite3_value_text(argv[2]);
   if (argc > 3) {
     sep = (unsigned char *)sqlite3_value_text(argv[3]);
     if (argc == 5) {
       number_of_bits = (unsigned int)my_value_int(argv[4], 0);
       if (number_of_bits == 0) {
         sqlite3_result_text(context, "", -1, SQLITE_STATIC);
         return; 
       }
     }
   }
   if (number_of_bits > 64) {
     number_of_bits = 64;
   }
   resultlen = number_of_bits * _max(strlen((char *)on), strlen((char *)off))
               + (number_of_bits - 1) * strlen((char *)sep)
               + 1;
   if ((result = (char *)sqlite3_malloc(resultlen)) == (char *)NULL) {
      sqlite3_result_error_nomem(context);
      return;
   }
   *result = '\0';
   for (i = 0; i < number_of_bits; i++) {
     if (i) {
       strcat(result, (char *)sep);
     }
     if (bits & 0x01) {
       strcat(result, (char *)on);
     } else {
       strcat(result, (char *)off);
     }
     bits = bits >> 1;
   }
   sqlite3_result_text(context, (char *)result, -1, sqlite3_free);
}
