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

extern void  my_soundex(sqlite3_context * context,
                        int               argc,
                        sqlite3_value  ** argv) {
    char *p;
    int   len;
    char *soundex = (char *)NULL;
    char c;
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
    while (p[j] != '\0') {  // Not limited to 4 with MySQL
      c = code(p[j]);
      if (c != '0') {
        if (!(soundex[i-1] == c)) {  // Not quite the same
                                     // algorithm as others
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
     soundex[i] = '\0';
     sqlite3_result_text(context, soundex, -1, sqlite3_free);
}
