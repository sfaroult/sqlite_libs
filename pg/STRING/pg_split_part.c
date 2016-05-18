#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ksu_common.h>

/*
 * SPLIT_PART( string text, delimiter text, field int )
 * 
 * fields are divided into sections of the string separated by the delimiter
 * 
 * fields start at 1 from the left and the given field is returned
 */
extern void  pg_split_part(sqlite3_context * context,
                           int               argc,
                           sqlite3_value  ** argv) {
   char           *str;
   char           *s;
   char           *delimiter;
   char           *end;
   int             field; //the field to be returned
   int             count = 1; //the current field
   int             len;

   //the length of the string
   if (ksu_prm_ok(context, argc, argv, "split_part",
                  KSU_PRM_TEXT, KSU_PRM_TEXT, KSU_PRM_INT_GT_0 )) {
     str = (char *)sqlite3_value_text(argv[0]);
     delimiter = (char *)sqlite3_value_text(argv[1]);
     len = strlen(delimiter);
     field = sqlite3_value_int(argv[2]);
     s = str;
     end = strstr(s, delimiter);
     while (end && (count < field)) {
       s = end + len;
       count++;
       end = strstr(s, delimiter);
     }
     if (count < field) {
       sqlite3_result_null(context);
     } else {
       if (end) {
         sqlite3_result_text(context, s, end - s, NULL);
       } else {
         sqlite3_result_text(context, s, -1, NULL);
       }
     }
   }
}
