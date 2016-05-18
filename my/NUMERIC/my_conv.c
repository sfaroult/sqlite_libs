#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <ksu_common.h>
#include <ksu_my.h>

#define _abs(x)   (x < 0 ? -1 * x : x)

extern void my_conv(sqlite3_context * context, 
                    int               argc, 
                    sqlite3_value  ** argv) {
           char              *nstr;
           int                nlen;
           char              *p;
           char              *q;
           int                from_base;
           int                to_base;
    static char              *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
           char               neg = 0;
           char               unsign = 1;
           unsigned long long n = 0; // numerical value
           unsigned long      coef;
           int                d;
           char              *result;
           int                resultlen;
           int                i;
        
    _ksu_null_if_null_param(argc, argv);
    nstr = (char *)sqlite3_value_text(argv[0]);
    from_base = _abs(my_value_int(argv[1], 0));  // Sign is ignored
    to_base = my_value_int(argv[2], 0);
    // An invalid base returns null; invalid characters for
    // from_base are ignored
    if ((from_base < 2)
        || (from_base > 36)
        || (_abs(to_base) < 2)
        || (_abs(to_base) > 36)) {
      sqlite3_result_null(context);
      return;
    }

    while (isspace(*nstr)) {
      nstr++;
    }
    nlen = strlen(nstr);
    while (nlen && isspace(nstr[nlen-1])) {
      nlen--;
    }
    if (nlen == 0) {
      sqlite3_result_int(context, 0);
      return;
    }
    switch (*nstr) {
      case '+':
           nstr++;
           nlen--;
           break;
      case '-':
           neg = 1;
           if (to_base < 0) {
             unsign = 0;
           } 
           // Else number to be interpreted as unsigned
           nstr++;
           nlen--;
           break;
      default:
           break;
    }
    if (to_base < 0) {
      to_base *= -1;
    }
    p = &(nstr[nlen]);
    coef = 1;
    while (p != nstr) {
      p--;
      if (((q = strchr(digits, toupper(*p))) != (char *)NULL)
           && ((q - digits) < from_base)) {
        n += (q - digits) * coef;
        coef *= from_base;
      } else {
        // Oops! suppose that's where the number ends
        n = 0;
        coef = 1;
      }
    }
    if (unsign && neg) {
      n = (unsigned long long)(-1 * n);
    }
    resultlen = (int)ceil(log((double)n)/log(to_base));
    if (neg && !unsign) {
      resultlen++; // For the minus sign
    }
    if ((result = (char *)sqlite3_malloc(resultlen)) == (char *)NULL) {
      sqlite3_result_error_nomem(context);
      return;
    }
    i = resultlen - 1;
    while (n > 0) {
      d = (n % to_base);
      result[i] = *(digits + d);
      n /= to_base;
      i--;
    }
    if (neg && !unsign) {
      result[0] = '-';
    }
    sqlite3_result_text(context, result, resultlen, sqlite3_free);
} 
