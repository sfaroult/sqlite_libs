#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

extern void  my_find_in_set(sqlite3_context * context, 
                            int               argc, 
                            sqlite3_value  ** argv) {
     
    char   found = 0;
    int    len;
    char  *needle;
    char  *haystack;
    char  *h;
    char  *p;
    int    pos;
                
    _ksu_null_if_null_param(argc, argv);
    needle = (char *)sqlite3_value_text(argv[0]);
    len = strlen((char *)sqlite3_value_text(argv[1]));
    if (len) {
      if ((haystack = (char *)sqlite3_malloc(len +1)) == (char *)NULL) {
        sqlite3_result_error_nomem(context);
        return;
      }
      strcpy(haystack, (char *)sqlite3_value_text(argv[1]));
      h = haystack;
      pos = 1;
      while (!found && ((p = strchr(h, ',')) != (char *)NULL)) {
        *p = '\0';
        p++;
        if (strcmp(needle, h) == 0) {
          found = 1;
        } else {
          h = p;
          pos++;
        }
      }
      if (!found) {
        if (strcmp(needle, h) == 0) {
          found = 1;
        }
      }
      sqlite3_free(haystack);
    }
    sqlite3_result_int(context, (found ? pos : 0));
} 
