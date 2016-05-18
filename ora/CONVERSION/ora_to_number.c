#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <locale.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_ora.h>

#define FORMAT_LEN     200

#define LEADING    0
#define TRAILING   1

static int trim_not_numeric(char **fmtp,
                            char **strp,
                            int   *flenp,
                            int   *slenp,
                            char   which_end) {
    //
    //  This function removes at either end stuff that isn't numeric
    //  (other than what refers to sign) from the input string -
    //  mostly spaces and currency signs.
    //  It also checks for consistency between string and format.
    //
    //  Returns 1 when successful, 0 when input string doesn't
    //  match format, -1 when format is invalid.
    //  If successful, adjusts pointers.
    //
    char         *f;
    char         *s;
    char         *p;
    int           flen;
    int           slen;
    int           len;
    char          c;
    int           fpos;  // Position in format
    int           spos;  // Position in string
    struct lconv *lc;
   
    if (fmtp && strp && flenp && slenp) {
      if (*flenp && *slenp) {
        // Set locale-dependent stuff
        lc = localeconv();
        // For convenience
        f = *fmtp;
        s = *strp;
        flen = *flenp;
        slen = *slenp;
        //
        if (which_end == LEADING) {
          spos = 0;
          fpos = 0;
        } else {
          // Trailing
          spos = slen;
          fpos = flen - 1;
        }
        c = toupper(f[fpos]);
        while ((fpos >= 0)
               && (fpos < flen)
               && (spos >= 0)
               && (spos < slen)
               && !isdigit(c)
               && (c != 'E')
               && (c != 'X')) {
          switch(c) {
            case 'B': // Can be ignored
                 len = 0;
                 break;
            case 'D':
            case '.':
                 if (c == 'D') {
                   if (lc && lc->decimal_point) {
                     p = lc->decimal_point;
                     len = strlen(p);
                   } else {
                     // Default
                     p = ".";
                     len = 1;
                   }
                 } else {
                   p = &(f[fpos]);
                   len = 1;
                 }
                 if (which_end == TRAILING) {
                   spos -= len;
                 }
                 // Check that we have something similar in the
                 // string to convert
                 if (strncasecmp(&(s[spos]), p, len)) {
                   // Mismatch
                   // ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, "to_number");
                   return 0;
                 } else {
                   // OK
                   if (which_end == LEADING) {
                     spos += len;
                   } else {
                     spos -= len;
                   }
                   slen -= len;
                 }
                 break;
            case '$':
            case 'C':
            case 'L':
            case 'U':
                 if (c == '$') {
                   p = "$";
                   len = 1;
                 } else {
                   if (lc) {
                     switch (c) {
                       case 'C':   // 3-char ISO code
                            p = lc->int_curr_symbol;
                            len = 3;
                            break;
                       default:
                            p = lc->currency_symbol; // Beware that we may
                                                     // get Eu instead of €
                            len = 0;
                            if (p) {
                              len = strlen(p);
                              while (len && isspace(p[len-1])) {
                                len--;
                              }
                            }
                            if (len == 0) {
                              p = "$";
                              len = 1;
                            }
                            break;
                     }
                   } else {
                     // Default
                     if (c == 'C') {
                       p = "USD";
                       len = 3;
                     } else {
                       p = "$";
                       len = 1;
                     }
                   }
                 }
                 if (which_end == TRAILING) {
                   spos -= len;
                 }
                 // Check that we have something similar in the
                 // string to convert
                 if (strncasecmp(&(s[spos]), p, len)) {
                   // String doesn't match.
                   // BUT there is a special case: the locale 
                   // may say that the Euro symbol is 'eu'
                   // and the string may contain €
                   if (which_end == TRAILING) {
                     // recompute it: add back what was removed
                     // and remove the length of €
                     spos = spos + len - strlen("€");
                   }
                   if (strncasecmp(p, "eu", 2)
                      || strncasecmp(&(s[spos]), "€", strlen("€"))) {
                     // Mismatch
                //     ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, "to_number");
                     return 0;
                   } else {
                     // OK, that was €
                     slen -= strlen("€");
                   }
                 } else {
                   // Match
                   slen -= len;
                 }
                 break;
            default:
                 return -1;
                 break; /*NOTREACHED*/
          }
          if (which_end == TRAILING) {
            fpos--;
            while (spos && isspace(s[spos])) {
              spos--;
              slen--;
            }
          } else {
            fpos++;   // One character in all cases
            spos += len;
            while (isspace(s[spos])) {
              spos++;
              slen--;
            }
          }
          c = toupper(f[fpos]);
          flen--;
        }           
        // Successful exit
        // Adjust pointers
        if (which_end == LEADING) {
          *fmtp = &(f[fpos]);
          *strp = &(s[spos]);
        }
        *flenp = flen;
        *slenp = slen;
        return 1;
      }
    }
    return -1;
}

static short process_body(sqlite3_context *context,
                          char            *fmt,
                          char            *str,
                          int              flen,
                          int              slen,
                          char             neg) {
    char         *s;
    char         *f;
    char         *p;
    char         *q;
    int           len;
    struct lconv *lc;
    short         decpoint = 0;
    short         thseps = 0;
    char          localth = 0;
    char          exponent = 0;
    short         hex = 0;
    short         before_decpoint = 0;
    short         after_decpoint = 0;
    char          is_int = 0;
    double        val;
    long          intval;
    int           i;
    int           l;
    char          format[FORMAT_LEN];
    char          str2[FORMAT_LEN];   // Copy so that we can modify it
    char          result[FORMAT_LEN]; // In fact, we are using a character
                                         // string ...

    static unsigned char thinspace[4] = {226, 128, 137, 0};
    // Error set to 1 if invalid format, 2 if failure to read.
   
    if (fmt && str && flen && slen) {
      strncpy(str2, str, slen);
      s = str2;
      f = fmt;
      // First scan the format
      if ((*f == ',')
         || (*f == 'G')) {
         // Cannot come first
        return (short)1;
      }
      while (f < (fmt + flen)) {
        if (exponent) {
          // Wrong format
          return (short)1;
        }
        switch (toupper(*f)) {
          case ',': 
          case 'G':
               thseps++;
               if (*f != ',') {
                 localth = 1;
               }
               break;
          case '0':
          case '9':
               if (decpoint) {
                 after_decpoint++;
               } else {
                 before_decpoint++;
               }
               break;
          case 'X':
               hex++;
               break;
          case 'D':
          case '.':
               if (decpoint) {
                 // Can only have one
                 return (short)1;
               }
               decpoint = 1;
               break;
          case 'E': //EEEE expected - there should be nothing after it
               if (strncasecmp(f, "EEEE", 4)) {
                 // Wrong format
                 return (short)1;
               }
               f += 3;   // Move to the last E
               exponent = 1;
               break;
          default: // Nothing else makes any sense
               return (short)1;
               break; /*NOTREACHED*/
        }
        f++;
      }           
      p = (char *)NULL;
      if (localth) {
        // Set locale-dependent stuff
        lc = localeconv();
        if (lc) {
          p = lc->thousands_sep;
          if (p) {
            if ((*p == '\0') || isspace(*p)) {
              p = (char *)thinspace;
            }
          }
        }
      }
      if (!p) {
        p = ",";
      }
      len = strlen(p);
      // Remove thousands separators
      i = 0;
      // We can find fewer separators than specified in the format,
      // but not more
      while (i < thseps) {
        if (((q = strstr(s, p)) != NULL)
           || ((p == (char *)thinspace)
              && ((q = strchr(s, ' ')) != NULL))) {
          if (strncmp(q, p, len)) { // We found a space
            l = 1;
          } else {
            l = len;
          }
          while (q < (str2 + slen)) {
            *q = *(q + len);
            q++;
          }
          i++;
        } else {
          break;
        }
      }
      // Prepare the format for scanning
      if (hex) {
        if (decpoint) { // Wrong format
          return (short)1;
        } 
        sprintf(format, "%%%dx", hex);
        is_int = 1;
      } else {
        if (decpoint) {
          sprintf(format, "%%%d.%dlf",
                          1 + before_decpoint + after_decpoint,
                          after_decpoint);
        } else {
          is_int = 1;
          sprintf(format, "%%%dld", before_decpoint);
        }
      }
      if (is_int) {
        if (sscanf(s, format, &intval) == 0) {
          return (short)2;
        }
        if (hex) {
          sprintf(result, "%ld", (neg ? -1 : 1) * intval);
        } else {
          sprintf(result, format, (neg ? -1 : 1) * intval);
        }
      } else {
        if (sscanf(s, "%lg", &val) == 0) {
          return (double)2;
        }
        sprintf(result, format, (neg ? -1 : 1) * val);
      }
      sqlite3_result_text(context, result, -1, SQLITE_TRANSIENT);
      return (short)0;
    }
    return (short)1;
}

extern void ora_to_number(sqlite3_context *context,
                          int              argc,
                          sqlite3_value  **argv) {

   char       num = 0;
   char       neg = 0;
   char       fmt[FORMAT_LEN];
   char       str[FORMAT_LEN];
   int        flen;
   int        slen;
   int        typ;
   int        ret;
   char      *f;
   char      *s;

   _ksu_check_arg_cnt(argc, 1, 2, "to_number");
   typ = sqlite3_value_type(argv[0]);
   switch (typ) {
       case SQLITE_INTEGER:
       case SQLITE_FLOAT:
            num = 1;   // Minimum effort
            break;
       case SQLITE_TEXT:
            break;
       case SQLITE_NULL:
            sqlite3_result_null(context);
            return;
       default: // BLOB ? Won't do anyhow.
            ksu_err_msg(context, KSU_ERR_INV_DATATYPE, "to_number");
            return;
   }
   if ((argc == 1)
      || (strcasecmp((char *)sqlite3_value_text(argv[1]), "tm9") == 0)
      || (strcasecmp((char *)sqlite3_value_text(argv[1]), "tme") == 0)
      || (strcasecmp((char *)sqlite3_value_text(argv[1]), "tm") == 0)) {
     if (num) {
       // Return as is
       sqlite3_result_value(context, argv[0]);
     } else {
       double  val;
       double  valcheck;

       (void)strncpy(str, (char *)sqlite3_value_text(argv[0]), FORMAT_LEN);
       slen = strlen(str);
       while (slen && isspace(str[slen-1])) {
         slen--;
       }
       str[slen] = '\0';
       if (slen && sscanf(str, "%lg", &val) == 1) {
         // Check that we got everything by removing one character at the
         // end. If the result is the same, then the string isn't
         // purely numerical
         str[slen-1] = '\0';
         if ((sscanf(str, "%lg", &valcheck) != 1)
             || (valcheck != val)) {
           // OK
           // Return as is
           sqlite3_result_value(context, argv[0]);
           return;
         }
       }
       // Else won't work
       ksu_err_msg(context, KSU_ERR_CONV, "to_number");
     }
     return;
   }
   f = (char *)sqlite3_value_text(argv[1]);
   while (*f && isspace(*f)) {
     f++;
   }
   if (*f == '\0') {
     sqlite3_result_null(context);
     return;
   }
   // We want to be able to modify the string
   strncpy(fmt, f, FORMAT_LEN);
   flen = strlen(fmt);
   while (flen && isspace(fmt[flen-1])) {
     flen--;
   }
   fmt[flen] = '\0';
   //
   s = (char *)sqlite3_value_text(argv[0]);
   while (*s && isspace(*s)) {
     s++;
   }
   if (*s == '\0') {
     sqlite3_result_null(context);
     return;
   }
   // We want to be able to modify the string
   (void)strncpy(str, s, FORMAT_LEN);
   slen = strlen(str);
   while (slen && isspace(str[slen-1])) {
     slen--;
   }
   str[slen] = '\0';
   // Special case: Roman numbers
   if (strcasecmp(fmt, "rn") == 0) {
     int rom_num;

     rom_num = ksu_compute_roman(str);
     if ((rom_num < 1) || (rom_num >= 4000)) {
       // Value out of range or invalid roman number
       ksu_err_msg(context, KSU_ERR_INV_ARG_VAL_RANGE, "to_number");
     } else {
       sqlite3_result_int(context, rom_num);
     }
     return;
   }
   // Then check for MI or PR at the end of the format
   // and if found check the string for anything that 
   // indicates that the result is negative.
   if (flen >= 2) {
     if (strcasecmp(&(f[flen - 2]), "mi") == 0) {
       if ((str[slen-1] == '-')
           || (str[slen-1] == '+')) {
         neg = (str[slen-1] == '-');
         slen--;
         while (slen && isspace(str[slen-1])) {
           slen--;
         }
         str[slen] = '\0';
       }
       flen -= 2;
     } else if (strcasecmp(&(f[flen - 2]), "pr") == 0) {
       if (str[slen-1] == '>') {
         if (str[0] == '<') {
           str[0] = '-';
           slen--;
           while (slen && isspace(str[slen-1])) {
             slen--;
           }
           str[slen] = '\0';
         } else {
           ksu_err_msg(context, KSU_ERR_CONV, "to_number");
           return;
         }
       }
       flen -= 2;
     }
   }
   f = fmt;
   s = str;
   ret = trim_not_numeric(&f, &s, &flen, &slen, TRAILING);
   switch(ret) {
     case 1 : // OK
          break;
     case 0 : 
          ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, "to_number");
          return;
     case -1 : 
          ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                      (char *)sqlite3_value_text(argv[1]), "to_number");
          return;
     default : 
          ksu_err_msg(context, KSU_ERR_CONV, "to_number");
          return;
   }
   ret = trim_not_numeric(&f, &s, &flen, &slen, LEADING);
   switch(ret) {
     case 1 : // OK
          break;
     case 0 : 
          ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, "to_number");
          return;
     case -1 : 
          ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                      (char *)sqlite3_value_text(argv[1]), "to_number");
          return;
     default : 
          ksu_err_msg(context, KSU_ERR_CONV, "to_number");
          return;
   }
   ret = (int)process_body(context, f, s, flen, slen, neg);
   switch(ret) {
     case 0:  // OK
          break;
     case 1 : 
          ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                      (char *)sqlite3_value_text(argv[1]), "to_number");
          break;
     default : 
          ksu_err_msg(context, KSU_ERR_CONV, "to_number");
          break;
   }
}
