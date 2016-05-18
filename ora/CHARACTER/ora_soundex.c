#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <ksu_common.h>

static char code(char c) {
    char x;
    switch (tolower(c)) {
       case 'b' :
       case 'f' :
       case 'p' :
       case 'v' :
            x = '1';
            break;
       case 'c' :
       case 'g' :
       case 'j' :
       case 'k' :
       case 'q' :
       case 's' :
       case 'x' :
       case 'z' :
            x = '2';
            break;
       case 'd' :
       case 't' :
            x = '3';
            break;
       case 'l' :
            x = '4';
            break;
       case 'm' :
       case 'n' :
            x = '5';
            break;
       case 'r' :
            x = '6';
            break;
       default:
            x = '0';
            break;
    }
    return x;
}

extern void  ora_soundex(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
    char *p;
    int   len;
    char *soundex = (char *)NULL;
    char c;
    char c2;
    int  i;
    int  j;

    _ksu_null_if_null_param(argc, argv);
    p = (char *)sqlite3_value_text(argv[0]);
    len = strlen(p);
    // Allocate the same size for the soundex
    if ((soundex = (char *)sqlite3_malloc(len + 1)) == (char *)NULL) {
      sqlite3_result_error_nomem(context);
      return;
    }
    soundex[0] = toupper(*p);
    soundex[1] = '\0';
    j = 1;
    i = 1;
    while ((p[j] != '\0') && (i < 4)) {
      c = code(p[j]);
      c2 = code(p[j-1]);
      if (c != '0') {
        if (!((c == c2)
            || ((soundex[i-1] == c)
                && ((tolower(p[j-1]) == 'h')
                    || (tolower(p[j-1]) == 'w'))))) {
           soundex[i] = c;
           i++;
         }
       }
       j++;
     }
     while (i < 4) {
       soundex[i] = '0';
       i++;
     }
     soundex[4] = '\0';
     sqlite3_result_text(context, soundex, -1, sqlite3_free);
}
