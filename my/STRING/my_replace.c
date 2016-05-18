#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <ksu_common.h>

extern void my_replace(sqlite3_context  *context,
                       int               argc,
                       sqlite3_value   **argv) {
   char *str;
   char *s;
   char *p;
   char *from_str;
   char *to_str;
   char *result;
   int   len;
   
   _ksu_null_if_null_param(argc, argv);
   str = (char *)sqlite3_value_text(argv[0]);
   from_str = (char *)sqlite3_value_text(argv[1]);
   to_str = (char *)sqlite3_value_text(argv[2]);
   len = strlen(str);
   // Compute the maximum length we can need in the worst case
   if (strlen(to_str) > strlen(from_str)) {
     s = str;
     while ((p = strstr(s, from_str)) != (char *)NULL) {
       len += (strlen(to_str) - strlen(from_str));
       p += strlen(from_str);
       s = p;
     }
   }
   if ((result = sqlite3_malloc(len + 1)) == (char *)NULL) {
     sqlite3_result_error_nomem(context);
     return;
   }
   *result = '\0';
   s = str;
   while ((p = strstr(s, from_str)) != (char *)NULL) {
     if (p > s) {
       (void)strncat(result, s, p - s);
     }
     (void)strcat(result, to_str);
     p += strlen(from_str);
     s = p;
   }
   (void)strcat(result, s);
   sqlite3_result_text(context, result, -1, sqlite3_free);
}
