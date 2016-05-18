/*
 * Reverse Algorithm found from:
 * http://stackoverflow.com/questions/198199
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

#define SWP( x, y ) ( x^=y, y^=x, x^=y )

// Reverses a normal string
static void  strrev(char *p) {
   char  *q = p;

   while (q && *q) {
     ++q;
   } //finds the end of the string
   for (--q; p < q; ++p, --q) {
     SWP(*p, *q);
   }
}

//Reverses a UTF8 string
static char *strrev_utf8(char *p) {
   char *str;

   str = (char *)sqlite3_malloc(strlen(p) + 1);
   strcpy(str, p);
   char  *q = str;

   strrev(str); //call base case
   while (q && *q) {
     ++q;
   } //find end of string
   while (str < --q) {
     switch ((*q & 0xF0) >> 4) {
       case 0xF:
            //four bytes.
            SWP(*(q - 0), *(q - 3));
            SWP(*(q - 1), *(q - 2));
            q -= 3;
            break;
       case 0xE:
            //three bytes.
            SWP(*(q - 0), *(q - 2));
            q -= 2;
            break;
       case 0xC:
       case 0xD:
            //two bytes.
            SWP(*(q - 0), *(q - 1));
            q--;
            break;
     }
   }
   return str;
}

extern void my_reverse(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
   char           *str;
   char           *v;

   _ksu_null_if_null_param(argc, argv)
   v = (char *)sqlite3_value_text(argv[0]);
   str = strrev_utf8(v);
   sqlite3_result_text(context, str, -1, sqlite3_free);
}
