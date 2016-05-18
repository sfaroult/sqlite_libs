#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>
#include <ksu_my.h>

extern void  my_substring_index(sqlite3_context * context, 
                                int               argc, 
                                sqlite3_value  ** argv) {
     
    int    found = 0;
    unsigned char  *needle;
    int    nlen;
    unsigned char  *haystack;
    int    count;
    unsigned char  *h;
    unsigned char  *p;
                
    _ksu_null_if_null_param(argc, argv);
    haystack = (unsigned char *)sqlite3_value_text(argv[0]);
    needle = (unsigned char *)sqlite3_value_text(argv[1]);
    nlen = strlen((char *)needle);
    count = my_value_int(argv[2], 0);
    if (count == 0) {
      sqlite3_result_text(context, "", -1, SQLITE_STATIC);
    } else {
      if (count > 0) {
        h = haystack;
        while ((found < count)
               && ((p = (unsigned char *)strstr((char *)h,
                                (char *)needle)) != (unsigned char *)NULL)) {
          found++;
          p += nlen;
          h = p;
        }
        // Return everything to the left of the delimiter
        if (p) {
          sqlite3_result_text(context, (char *)haystack,
                              p - haystack - nlen, NULL);
        } else {
          sqlite3_result_text(context, (char *)haystack, -1, NULL);
        }
      } else {
        if ((p = (unsigned char *)strrchr((char *)haystack,
                                   (char)*needle)) != (unsigned char *)NULL) {
          if (strncmp((char *)p, (char *)needle, nlen) == 0) {
            found--;
          }
          _ksu_utf8_decr(p);
          while ((p >= haystack)
                 && (found > count)) {
            if ((*p == *needle)
                && (strncmp((char *)p, (char *)needle, nlen) == 0)) {
              found--;
            }
            _ksu_utf8_decr(p);
          }
          if (p >= haystack) {
            // Re-increment p to point to "needle"
            SQLITE_SKIP_UTF8(p);
            // Return everything to the right
            p += nlen;
            sqlite3_result_text(context, (char *)p, -1, NULL);
          } else {
            if (found == count) {
               p = haystack + nlen;
            } else {
               p = haystack;
            }
            sqlite3_result_text(context, (char *)p, -1, NULL);
          }
        } else {
          sqlite3_result_text(context, (char *)haystack, -1, NULL);
        }
      }
    }
} 
