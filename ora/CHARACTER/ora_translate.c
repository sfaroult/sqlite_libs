#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>

#define ALLOC_SZ     20

/*
 *  TRANSLATE ( string text, from text, to text )
 * 
 * any occurrence of a character in string that matches a character in the from
 * text, is replaced with the corresponding character in the to text
 * 
 * if the from text is longer than the to text, occurrences of the extra
 * characters are removed
 */ 
extern void     ora_translate(sqlite3_context * context, 
                              int               argc, 
                              sqlite3_value  ** argv) {
        
    int             str_len;
    unsigned char  *string;
    unsigned char  *result;
    unsigned char  *r;
    int             resultlen;
    unsigned char  *from;
    unsigned char  *to;
    unsigned char  *p;
    unsigned char  *s;
    int             frompos;
    int             tobyte;
    unsigned char  *pin;
    int             offset;
                
    _ksu_null_if_null_param(argc, argv);
    string = (unsigned char *)sqlite3_value_text(argv[0]);
    from = (unsigned char *)sqlite3_value_text(argv[1]);
    to = (unsigned char *)sqlite3_value_text(argv[2]);
    str_len = strlen((char *)string);           
    if ((result = (unsigned char *)sqlite3_malloc(str_len + 1))
                 == (unsigned char *)NULL) {
       sqlite3_result_error_nomem(context);
       return;
    }
    r = result;
    resultlen = str_len;
    p = string;
    while (*p) {
      if ((s = ksu_uft8search(from, p)) != (unsigned char *)NULL) {
        // This is a character to translate
        // Find its character position in the "from" string
        frompos = ksu_bytes_to_charpos(from, s - from); 
        // Get the corresponding byte position in the "to" string
        tobyte = ksu_charpos_to_bytes(to, frompos);
        if (tobyte != -1) {
          if ((offset = r - result) > resultlen + 4) {
            if ((result = (unsigned char *)sqlite3_realloc(result,
                            resultlen+ALLOC_SZ)) == (unsigned char *)NULL) {
              sqlite3_result_error_nomem(context);
              return;
            }
            resultlen += ALLOC_SZ;
            r = result + offset;
          }
          pin = to + tobyte;
          _ksu_utf8_copychar(pin, r);
        }
        SQLITE_SKIP_UTF8(p);
      } else {
        // Copy as is
        if ((offset = r - result) > resultlen + 4) {
          if ((result = (unsigned char *)sqlite3_realloc(result,
                    resultlen+ALLOC_SZ)) == (unsigned char *)NULL) {
            sqlite3_result_error_nomem(context);
            return;
          }
          resultlen += ALLOC_SZ;
          r = result + offset;
        }
        _ksu_utf8_copychar(p, r);
      } 
   }
   *r = '\0';
   sqlite3_result_text(context, (char *)result, -1, sqlite3_free);
} 
