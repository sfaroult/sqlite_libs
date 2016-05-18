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
#define FORMATTED_LEN   64
#define STACK_SZ        20

// Values for computing the output width
#define  RN_SZ           15    // Roman number
#define  CURRENCY_SZ     10    // Added for currency symbol other than $ (U/L)
#define  CUR_SZ           7    // Added for international currency symbol (C)
#define  EEEE_SZ          5    // Added for scientific notation

// Just for Roman conversion ...

#define RN_SIZE  8

typedef struct {
   int  val;
   char roman;
} RN_CONV_T;

static RN_CONV_T G_roman[RN_SIZE] =
               {{1, 'I'},    // can prefix the two next ones
                {5, 'V'},
                {10, 'X'},   // can prefix the two next ones
                {50, 'L'},
                {100, 'C'},  // can prefix the two next ones
                {500, 'D'},
                {1000, 'M'},
                {5000, '?'}};

#define _right_case(x) (upper ? x : tolower(x))

static void right_align(char *b, int width) {
   int len = strlen(b);
   int i;
   int j;

   if (len < width) {
     b[width] = '\0';
     i = width - 1;
     j = len - 1;
     while (j >= 0) {
       b[i--] = b[j--];
     }
     while (i >= 0) {
       b[i--] = ' ';
     }
   }
}

static char *roman(int num, char *buf, char upper, char ltrim){
   int   i;
   int   n;
   char *b = buf;
   char *bb = buf;

   if(num <= 0 || num >=4000){
     *buf='\0';
     return NULL;
   }
   while (num > 0) {
     i = 0;
     while (G_roman[i+1].val < num) {
       i++;
     }
     if (G_roman[i+1].val != num) {
        if (!(i % 2)) {
          if (i < RN_SIZE - 2) {
            if (num >= (G_roman[i+2].val - G_roman[i].val)) {
              n = G_roman[i+2].val - G_roman[i].val;
              *buf++ = _right_case(G_roman[i].roman);
              *buf++ = _right_case(G_roman[i+2].roman);
            } else if (num >= (G_roman[i+1].val - G_roman[i].val)) {
              n = G_roman[i+1].val - G_roman[i].val;
              *buf++ = _right_case(G_roman[i].roman);
              *buf++ = _right_case(G_roman[i+1].roman);
            } else {
              n = G_roman[i].val;
              *buf++ = _right_case(G_roman[i].roman);
            }
          } else {
            n = G_roman[i].val;
            *buf++ = _right_case(G_roman[i].roman);
          }
        } else {
          if (num >= (G_roman[i+2].val - G_roman[i-1].val)) {
            n = G_roman[i+2].val - G_roman[i-1].val;
            *buf++ = _right_case(G_roman[i-1].roman);
            *buf++ = _right_case(G_roman[i+2].roman);
          } else if (num >= (G_roman[i+1].val - G_roman[i-1].val)) {
            n = G_roman[i+1].val - G_roman[i-1].val;
            *buf++ = _right_case(G_roman[i-1].roman);
            *buf++ = _right_case(G_roman[i+1].roman);
          } else {
            n = G_roman[i].val;
            *buf++ = _right_case(G_roman[i].roman);
          }
        }
     } else {
        n = G_roman[i+1].val;
        *buf++ = _right_case(G_roman[i+1].roman);
     }
     num -= n;
   }
   *buf='\0';
   if (!ltrim) {
     right_align(b, RN_SZ);
   }
   return bb;
}

static char *embellish(char *strval, const char *fmt) {
          char  result[FORMATTED_LEN];
          char *gs;
          short gslen;
          char *curr;
          short currlen;
          int   j;
          int   i;
          int   k;
          char  significant = 0;
          char  fmt_done = 0;
   struct lconv * lc;
   static unsigned char thinspace[4] = {226, 128, 137, 0};
   
   if (strval && fmt) {
      // Set locale-dependent stuff
      lc = localeconv();
      gs = lc->thousands_sep;
      if ((*gs == '\0') || isspace(*gs)) {
         gs = (char *)thinspace;
      }
      gslen = (short)strlen(gs);
      while (gslen && isspace(gs[gslen-1])) {
        gslen--;
      }
      curr = lc->currency_symbol;
      if (strncasecmp(curr, "Eu", 2) == 0) {
        curr = "€";
      }
      currlen = (short)strlen(curr);
      while (currlen && isspace(curr[currlen-1])) {
        currlen--;
      }
      // Start processing the format
      //
      // Three different indexes to manage:
      //   i = number in which to insert separators
      //       (and currency symbols?), our input, result of sprintf
      //   j = formatted result, our output
      //   k = Oracle format (for reference)
      //
      j = 1;
      i = 1;
      // Skip sign. There will be a difference of 1 position
      // in the Oracle format, unless it starts with S
      result[0] = strval[0];  // sign or space, depends
      // Skip "FM" if present
      k = 0;
      if (strncmp(fmt, "FM", 2) == 0) {
        k = 2;
      } 
      // Ignore "B"
      if (fmt[k] == 'B') {
        k++;
      }
      k += (1 - (fmt[k] == 'S' ? 0 : 1));
      //
      while (strval[i] || fmt[k]) {
         if (!significant) {
           if ((strval[i] == '.')
               || (isdigit(strval[i]) && (strval[i] != '0'))) {
             significant = 1;
           }
         }
         if (!fmt_done) {
           switch (fmt[k]) {
              case '\0':
                   fmt_done = 1;
                   break;
              case 'G':
              case ',':
                   if (significant) {
                     if (fmt[k] == ',') {
                      result[j++] = ',';
                     } else {
                       (void)memcpy(&(result[j]), gs, (size_t)gslen);
                       j += gslen;
                     }
                   }
                   break;
              case '0':
              case 'D':  // Nothing to do. sprintf() took care of it
                   result[j++] = strval[i++];
                   break;
              case '9':
                   if (significant || (strval[i] != '0')) {
                     result[j++] = strval[i];
                   } else {
                     result[j++] = ' ';
                   }
                   i++;
                   break;
              case '$':
                   if (significant) {
                     result[j] = '\0';
                     j = strlen(result);
                   }
                   result[j++] = '$';
                   break;
              case 'C':  // International currency symbol (3-char ISO)
                   if (significant) {
                     result[j] = '\0';
                     j = strlen(result);
                   }
                   (void)memcpy(&(result[j]),
                                lc->int_curr_symbol,
                                (size_t)3);
                   j += 3;
                   break;
              case 'L':  // Local currency symbol
              case 'U':  // Alternate - same as local for us
                   if (significant) {
                     result[j] = '\0';
                     j = strlen(result);
                   }
                   (void)memcpy(&(result[j]), curr, (size_t)currlen);
                   j += currlen;
                   break;
              case '.':
                   // Change back to a dot if the locale set something else
                   if (strval[i] != '.') {
                     result[j++] = '.';
                   } else {
                     result[j++] = strval[i];
                   }
                   i++;
                   break;
              default: // Includes sign - copy as is
                   result[j++] = strval[i++];
                   break;
           }
           k++;
         } else {
           result[j++] = strval[i++];
         }
      } 
      result[j] = '\0';
      strncpy(strval, result, FORMAT_LEN);
   }
   return strval;
}

static char *ora_format_number(sqlite3_context *context,
                               sqlite3_value   *val,
                               const char      *fmt,
                               char            *formatted) {
    char   decimal_sep = '.';     // default 
    short  pos = 0;
    char  *f;
    char  *p;
    char  *q;
    int    precision = 0;     // Total number of characters printed
    int    scale = 0;         // Number of digits after the decimal point
    int    power = 0;         // Additional power of 10 to multiply by
    // Bunch of formatting flags
    char   leading_zero = 0;  // flag
    char   b_flag = 0;        // flag - spaces instead of zeroes
    char   ltrim = 0;         // flag - "FM" format model
    char   eeee = 0;          // flag - scientific notation
    char   tm = 0;            // flag - condensed format
    char   hex = 0;           // flag - 1 for uppercase, 2 for lowercase
    char   sign = 0;          // flag - will be 1 for leading, 2 for trailing,
                              //        3 for training minus or space
    char   pr = 0;            // flag - angle brackets if negative
    char   decimal_only_nines = 1;  // flag
    // Flags for knowing where we are in the format
    char   decimal_part = 0;  // flag
    char   v_part = 0;        // flag
    char   format_done = 0;   // flag
    // Other variables
    char   the_sign;          // Actual sign - + or -
    short  nines = 0;         // Number of nines in the format
    char   negative;          // flag
    char   num_format[FORMAT_LEN];
    char   ora_format[FORMAT_LEN];
    int    ofmt = 0;  // Oracle format length
    int    afmt = 0;  // Actual formatted length
    char   isInt = 0;         // flag
    char   buffer[50];
    int    code;
    int    i;
    double digits;
    int    typ;
    short  extrasz = 0;   // For output width
   
    if ((f = (char *)fmt) != NULL) {
      // Note that Oracle doesn't like spaces at the
      // beginning of a format
      if (strncasecmp(f, "FM", 2) == 0) {
        ltrim = 1;
        f += 2;
      }
      // Special case: Roman numbers
      // Contrarily to what some "examples" in the docs pretend,
      // Oracle doesn't accept any 9s alongside RN
      if (strcasecmp(f, "rn") == 0) {
        if (sqlite3_value_type(val) == SQLITE_INTEGER) {
          int num = sqlite3_value_int(val);

          if ((num < 1) || (num >= 4000)) {
            // Value out of range
            ksu_err_msg(context, KSU_ERR_INV_ARG_VAL_RANGE, "to_char");
            return NULL;
          }
          if (roman(num, formatted, isupper(*f), ltrim)) {
            return formatted;
          } else {
            // Conversion error - should not happen
            ksu_err_msg(context, KSU_ERR_GENERIC, "to_char");
            return NULL;
          }
        } else {
          // ERROR - should be an integer
          ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 1, "to_char");
          return NULL;
        }
      }
      pos = 0;
      num_format[0] = '\0';
      ora_format[ofmt++] = ' ';
      while (*f) {
        switch (*f) {
          case 'B':
               b_flag = 1;
               break;
          case 'D':
          case '.':
               if (decimal_part || hex) {
                 // Already seen or hexadecimal !
                 ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                 return NULL;
               }
               ora_format[ofmt++] = *f;
               decimal_sep = *f;
               decimal_part = 1;
               precision++;      // Must be counted
               break;
          case '0':
          case '9':
          case 'x':
          case 'X':
               if (format_done) {
                 ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                 return NULL;
               }
               if (*f == '9') {
                 nines++;
               } else {
                 if (precision == 0) {
                   if (*f == '0') {
                     if (nines == 0) {
                       leading_zero = 1;
                     }
                   } else if (toupper(*f) == 'X') {
                     if (nines) {
                       ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                       return NULL;
                     }
                     if (!hex) {
                       if (*f == 'X') {
                         hex = 1;
                       } else {
                         hex = 2;
                       }
                     }
                   }
                 } else {
                   if (decimal_part
                       && (*f == '0')
                       && decimal_only_nines) {
                     decimal_only_nines = 0;
                   }
                 }
               }
               precision++;
               if (decimal_part) {
                 scale++;
               }
               if (v_part) {
                 power++;
               }
               ora_format[ofmt++] = *f;
               break;
          case ',':
          case 'G': 
               if (decimal_part || hex) {
                 ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                 return NULL;
               }
               extrasz++;
               break;
          case '$':
          case 'C':
          case 'L':
          case 'U':
               if (precision && !format_done) {
                 format_done = 1;
               }
               switch (*f) {
                 case '$':
                   extrasz++;
                   break;
                 case 'C':
                   extrasz += CUR_SZ;
                   break;
                 case 'L':
                 case 'U':
                   extrasz += CURRENCY_SZ;
                   break;
               }
               break;
          case 'S':
               if ((f != fmt) && (*(f + 1) != '\0')) {
                 ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                 return NULL;
               }
               if (f == fmt) {
                 sign = 1;
                 ora_format[0] = *f;
               } else {
                 sign = 2;
                 format_done = 1;
               }
               break;
          case 'V':
               if (v_part || hex) {
                 // Already seen or hexadecimal
                 ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                 return NULL;
               }
               v_part = 1;
               break;
          default:
               code = oranum_best_match(f);
               switch (code) {
                 case ORANUM_NOT_FOUND:
                      ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                                  (char *)fmt, "to_char");
                      return NULL;
                 case ORANUM_EEEE:
                      // Oracle accepts either EEEE or eeee,
                      // but no mixed case
                      if (strncmp(f, "EEEE", 4)
                          && strncmp(f, "eeee", 4)) {
                        ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                                    (char *)fmt, "to_char");
                        return NULL;
                      }
                      eeee = 1;
                      f += 3;
                      extrasz += EEEE_SZ;
                      format_done = 1; // Cannot be followed by 9s or 0s
                      break;
                 case ORANUM_TM:
                      if ((precision == 0)
                          && ((strcasecmp(f, "TM") == 0)
                              || (strcasecmp(f, "TM9") == 0)
                              || (strcasecmp(f, "TME") == 0))) {
                         tm = 1;
                         f += strlen(f) - 1; // To stop the loop
                      } else {
                        ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                                    (char *)fmt, "to_char");
                        return NULL;
                      }
                      break;
                 case ORANUM_MI:
                      if (*(f+2) != '\0') {
                        ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                                    (char *)fmt, "to_char");
                        return NULL;
                      }
                      sign = 3;
                      f += 2;
                      break;
                 case ORANUM_PR:
                      if (*(f+2) != '\0') {
                        ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                                    (char *)fmt, "to_char");
                        return NULL;
                      }
                      pr = 1;
                      f += 2;
                      extrasz++;
                      break;
                 default :
                      ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                                  (char *)fmt, "to_char");
                      return NULL;
               }
               break;
        }
        f++;
        pos++;
      }           
      // Get rid of easy cases
      if (b_flag && (sqlite3_value_double(val) == (double)0)) {
        for (i = 0; i < ofmt; i++) {
          formatted[i] = ' ';
        }
        formatted[ofmt] = '\0';
        return formatted;
      }
      negative = (sqlite3_value_double(val) < 0);
      ora_format[ofmt] = '\0';
      typ = sqlite3_value_type(val);
      if (tm) {
        strcpy(num_format, "%g");
      } else {
        if (precision) {
          if (!eeee) {
            digits = (double)precision - (scale ? 1 + scale : 0);
            if (sqlite3_value_double(val)
                && (log10((negative ? -1 : 1)
                        * sqlite3_value_double(val)) >= digits)) {
               // Format too small
               for (i = 0; i < ofmt; i++) {
                  formatted[i] = '#';
               }
               formatted[ofmt] = '\0';
               return formatted;
            }
            // Numbers already seen
            if (scale) {
              // Float/double
              sprintf(num_format,
                      "%%+%s%d.%dlf",
                      (leading_zero ? "0" : ""),
                      precision+1, scale);
            } else {
              if (hex) {
                sprintf(num_format,
                        "%%%s%d%s",
                        (leading_zero ? "0" : ""),
                        precision+1,
                        (hex == 1? "X":"x"));
              } else {
                // Int
                sprintf(num_format,
                        "%%+%s%dlld",
                        (leading_zero ? "0" : ""),
                        precision+1);
                isInt = 1;
              }
            }
          } else {  // Scientific notation
            sprintf(num_format,
                    "%%+0%d.%de",
                    precision+1, scale);
          }
        }
      }
      //printf("C format        : [%s]\n", num_format);
      if (hex) {
        snprintf(formatted, FORMAT_LEN,
                 num_format,
                 (unsigned int)sqlite3_value_int(val));
      } else {
        if (isInt) {
          if (typ == SQLITE_INTEGER) {
            snprintf(formatted, FORMAT_LEN,
                     num_format,
                     (long long)(v_part ? 1 : pow(10, power))
                                * sqlite3_value_int64(val));
          } else {
            snprintf(formatted, FORMAT_LEN,
                     num_format,
                     (long long)(v_part ? 1 : pow(10, power))
                     * (sqlite3_value_double(val)+0.5));
          }
        } else {
          snprintf(formatted, FORMAT_LEN,
                   num_format,
                   (v_part ? 1 : pow(10, power))
                    * sqlite3_value_double(val));
        }
      }
      if (!eeee && (strlen(formatted) > ofmt)) {
        for (i = 0; i < ofmt; i++) {
           formatted[i] = '#';
        }
        formatted[ofmt] = '\0';
        return formatted;
      }
      i = 1;  // Skip sign
      // Leading zeroes
      while (isspace(formatted[i])) {
        if ((i < ofmt) && (ora_format[i] == '0')) {
          formatted[i] = '0';
        }
        i++;
      }
      while ((formatted[i] == '0')
             && ((i >= ofmt) || (ora_format[i] != '0'))) {
        formatted[i] = ' ';
        i++;
      }
      // Trailing - Trailing zeroes are suppressed ONLY if
      // the format isn't only made of 9s after the decimal
      // position.
      if (!decimal_only_nines) { 
        i = strlen(formatted) - 1;
        while (i
               && (isspace(formatted[i])
                   || formatted[i] == '0')) {
          if (isspace(formatted[i])
              && ((i < ofmt) && (ora_format[i] == '0'))) {
            formatted[i] = '0';
          } else if ((formatted[i] == '0')
                     && ((i>=ofmt) || ora_format[i] == '9')) {
            formatted[i] = '\0';
          }
          i--;
        }
      }
      // Set group separators and currencies
      //printf("Before embellish: [%s]\n", formatted);
      strcpy(buffer, embellish(formatted, fmt));
      //printf("After embellish:  [%s]\n", buffer);
      // Replace 'e' by 'E' with EEEE format
      if (eeee) {
        if ((p = strchr(formatted, 'e')) != (char *)NULL) {
          *p = 'E';
        }
      }
      // Handle sign
      afmt = strlen(formatted); // Actual formatted length
      p = formatted;
      while (isspace(*p)) {
        p++;
      }
      the_sign = *p;
      // sign flag : 1 for leading, 2 for trailing,
      //             3 for training minus or space
      if (!sign) {
         // Only show if negative
         if (*p == '+') {
           *p = ' ';
         } else {
           // Move it after spaces, if there are spaces
           q = p;
           while (isspace(*(q+1))) {
             q++;
           } 
           if (q != p) {
             *p = ' ';
             *q = the_sign;
             p = q;
           }
         }
         if (pr) {
           if (negative) {
             *p = '<';
             formatted[afmt++] = '>';
           } else {
             formatted[afmt++] = ' ';
           }
           formatted[afmt] = '\0';
         }
      } else {
         if (sign >= 2) {
           if ((sign == 3) && (*p == '+')) {
             formatted[afmt++] = ' ';
           } else {
             formatted[afmt++] = *p;
           }
           formatted[afmt] = '\0';
           p++;
           i = 0;
           do {
             formatted[i++] = *p++;
           } while (*p);
           formatted[i] = '\0';
           p = formatted;
           afmt = strlen(p);
         } else {
           // Move the sign after spaces, if there are spaces
           q = p;
           while (isspace(*(q+1))) {
             q++;
           } 
           if (q != p) {
             *p = ' ';
             *q = the_sign;
             p = q;
           }
         }
      }
      // Special case : if the value is all spaces, make it 0
      // unless the b_flag is set
      if (!b_flag) {
        while (isspace(*p)) {
          p++;
        }
        if (*p == '\0') {
          p--;
          *p = '0';
        }
      }
      if (ltrim) {
        p = formatted;
        while (isspace(*p)) {
          p++;
        }
        if (p != formatted) {
          i = 0;
          do {
            formatted[i++] = *p++;
          } while (*p);
          formatted[i] = '\0';
        }
      }
    }
    if (!ltrim && extrasz) {
      right_align(formatted, precision + extrasz);
    }
    return formatted;
}

// Spelling out of numbers (dates, which limits the range)
static char *G_units[20] = {"zero", "one", "two", "three", "four",
                            "five", "six", "seven", "eight", "nine",
                            "ten", "eleven", "twelve", "thirteen",
                            "fourteen", "fifteen", "sixteen", "seventeen",
                            "eighteen", "nineteen"};
static char *G_unitsth[20] = {"", "first", "second", "third", "fourth",
                            "fifth", "sixth", "seventh", "eighth", "ninth",
                            "tenth", "eleventh", "twelfth", "thirteenth",
                            "fourteenth", "fifteenth", "sixteenth",
                            "seventeenth", "eighteenth", "nineteenth"};
static char *G_tens[10] = {"", "ten", "twenty", "thirty", "forty",
                           "fifty", "sixty", "seventy", "eighty", "ninety"};
static char *G_tensth[10] = {"", "tenth", "twentieth", "thirtieth",
                             "fortieth", "fiftieth", "sixtieth",
                             "seventieth", "eightieth", "ninetieth"};

static void capitalize(char *from_buf, char *to_buf, char cap) {
   // Cap: 0 = lower, 1 = upper, 2 = capitalize initial
   // Note that we might have multi-byte characters
   unsigned char *f = (unsigned char *)from_buf;
   unsigned char *t = (unsigned char *)to_buf;
   unsigned char *p;
   unsigned char  utf8[5];
   unsigned char *utf;
            char  after_letter = 0;

   if (f && t) {
     while (*f) {
       utf = utf8;
       switch (cap) {
         case 0:  // lower
           if (ksu_utf8_charlower((const unsigned char *)f, utf8)) {
             _ksu_utf8_copychar(utf, t);
           }
           SQLITE_SKIP_UTF8(f);
           break;
         case 1:  // upper
           if (ksu_utf8_charupper((const unsigned char *)f, utf8)) {
             _ksu_utf8_copychar(utf, t);
           }
           SQLITE_SKIP_UTF8(f);
           break;
         default: // nothing other than "2" expected (capitalize initial)
           if (ksu_is_letter((const unsigned char *)f)) {
             if (after_letter) {
               p = ksu_utf8_charlower((const unsigned char *)f, utf8);
             } else {
               p = ksu_utf8_charupper((const unsigned char *)f, utf8);
             }
             if (p) {
               _ksu_utf8_copychar(utf, t);
             }
             SQLITE_SKIP_UTF8(f);
             after_letter = 1;
           } else {
             _ksu_utf8_copychar(f, t);
             after_letter = 0;
           }
           break;
       }
     }
     *t = '\0';
   }
}

static void spell_out_2digits(int num, char *buffer) {
    int  n1;
    int  n2;

    if (buffer && (num < 100)) {
      if (num < 20) {
        strcpy(buffer, G_units[num]);
      } else {
        // Get tens
        n1 = num / 10;
        strcpy(buffer, G_tens[n1]);
        n2 = num - n1 * 10;
        if (n2) {
          strcat(buffer, "-");
          strcat(buffer, G_units[n2]);
        }
      }
    }
}

static void spell_out_year(int year, char *buffer, char cap) {
    // Only English is supported.
    //
    // Cap: 0 = lower, 1 = upper, 2 = capitalize initial
    int  y1;
    int  y2;
    int  len;
    char tmpbuf[FORMAT_LEN];

    // From
    // http://babelhut.com/languages/english/how-to-read-years-in-english/
    //
    // Algorithm for Reading Years
    //
    //  If there there are no thousands’ or hundreds’ digits, read
    //  the number as-is. Examples:
    //     54 – “fifty-four”
    //     99 – “ninety-nine”
    //      0 – “zero”
    //      8 – “eight”
    //  If there is a thousands’ digit but the hundreds’ digit is zero,
    //  you can read the number as “n thousand and x”. If the last two
    //  digits are zero, you leave off the “and x” part. Examples:
    //   1054 – “one thousand and fifty-four”
    //   2007 – “two thousand and seven”
    //   1000 – “one thousand”
    //   2000 – “two thousand”
    //  If the hundreds’ digit is non-zero, you can read the number as
    //  “n hundred and x”. If the last two digits are zero, you leave off
    //  the “and x” part. Examples:
    //    433 – “four hundred and thirty-three”
    //   1492 – “fourteen hundred and ninety-two” (who sailed the ocean blue?)
    //   1200 – “twelve hundred”
    //    600 – “six hundred”
    //  The above rule produces some formal and old-fashioned names. Where
    //  it exists, it is acceptable to omit “hundred and”. If you do, and
    //  the tens’ digit is zero, you must read that zero as “oh”. Examples:
    //    432 – “four thirty-two”
    //   1492 – “fourteen ninety-two”
    //   1908 – “nineteen oh eight”
    //   1106 – “eleven oh six”
    //  Finally, though uncommon it is possible to read the years in rule #2
    //  using the systems for rules #3 and #4. Examples:
    //   1054 – “ten hundred and fifty-four” (if this sounds wrong to you,
    //          imagine you are watching a documentary on the history channel
    //          and the stiff narrator begins: “In the year ten hundred and
    //          fifty-four, Pope Leo IX died.”)
    //   1054 – “ten fifty-four”
    //   3026 – “thirty twenty-six”
    //   2007 – “twenty oh seven” (if this sounds wrong to you, imagine
    //          you live in 1972 and you are reading a science fiction
    //          story that starts: “In the year twenty oh seven, the world
    //          was overrun by blood-thirsty robots.”)
    //
    //  Rules adopted slightly different from above
    //
    if (year < 0) {
      year *= -1;
    }
    if (year < 100) {
      spell_out_2digits(year, tmpbuf);
      strcpy(tmpbuf, G_units[year]);
    } else {
      y1 = year / 100;
      y2 = year - y1 * 100;
      if ((y1 < 10) || (y2 < 20)) {
        if ((y1 % 10) == 0) {
          // Hundreds' digit is zero. Thousands' digits necessarily non zero.
          y1 /= 10;
          if (y1 == 1) {
            strcpy(tmpbuf, "thousand");
          } else {
            sprintf(tmpbuf, "%s thousand", G_units[y1]);
          }
        } else {
          spell_out_2digits(y1, tmpbuf);
          strcat(tmpbuf, " hundred");
        }
        if (y2 > 0) {
          strcat(tmpbuf, " and ");
          strcat(tmpbuf, G_units[y2]);
        }
      } else {
        // Just spell out y1 and y2 separately
        spell_out_2digits(y1, tmpbuf);
        strcat(tmpbuf, " ");
        len = strlen(tmpbuf);
        spell_out_2digits(y2, &(tmpbuf[len]));
      }
    }
    capitalize(tmpbuf, buffer, cap);
}

static void spell_out_day(int day, char *buffer, char cap) {
    // Only English is supported.
    char tmpbuf[FORMAT_LEN];
    int  t;
    int  u;

    if ((day > 0) && (day <= 31)) {
      if (day < 20) {
        strcpy(tmpbuf, G_units[day]);
      } else {
        t = day / 10;
        u = day - 10 * t;
        strcpy(tmpbuf, G_tens[t]);
        if (u) {
          strcat(tmpbuf, "-");
          strcat(tmpbuf, G_units[u]);
        }
      }
      capitalize(tmpbuf, buffer, cap);
    }
}

static void spell_out_day_ord(int day, char *buffer, char cap) {
    // Only English is supported.
    char tmpbuf[FORMAT_LEN];
    int  t;
    int  u;

    if ((day > 0) && (day <= 31)) {
      if (day < 20) {
        strcpy(tmpbuf, G_unitsth[day]);
      } else {
        t = day / 10;
        u = day - 10 * t;
        if (u) {
          strcpy(tmpbuf, G_tens[t]);
          strcat(tmpbuf, "-");
          strcat(tmpbuf, G_unitsth[u]);
        } else {
          strcpy(tmpbuf, G_tensth[t]);
        }
      }
      capitalize(tmpbuf, buffer, cap);
    }
}

// For indicators
#define IND_DOTS         8
#define IND_FIRST_UPPER  4
#define IND_SECOND_UPPER 2
#define IND_SET          1

// For number rendering (spelling / ordinal)
#define NUM_SPELL        1
#define NUM_ORD          2

static char *ora_format_date(sqlite3_context *context,
                             KSU_TIME_T       t,
                             const char      *fmt,
                             char            *formatted) {
    char       *f;
    char       *p;
    char       *suf;   // Suffix
    int         code; 
    int         code_suf; 
    long        val;
    short       rendering = 0;
    char        buf[FORMATTED_LEN];
    char        capbuf[FORMATTED_LEN]; // For capitalization
    int         len;
    int         what;
    int         i = 0;
    int         k;
    int         pad;
    char        fm = 0;  // flag for variable model
    char        am = 0;  // flag for am/pm
    char        bc = 0;  // flag for bc/ad
    char        cap;     // flag for capitalization
                         // 0 = lower, 1 = upper, 2 = capitalize initial
    char        processed;  // flag
    char        abbr_fmt = 0; // Abbreviated format - DS or DL
    char        ts_fmt = 0;   // Short time format
    char        non_abbr = 0; // Non-abbreviated format
    char        str;
    int         hour = 25;    // Must be memorized for am/pm indicators
    int         year = 0;    // Must be memorized for bc/ad indicators
    KSU_TM_T    tm;

    if ((f = (char *)fmt) != NULL) {
      // Note that Oracle doesn't like spaces at the
      // beginning of a format
      while (*f) {
        // Punctuation and quoted text output as is
        if (*f == '"') {
          if (abbr_fmt || ts_fmt) {
            ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
            return NULL;
          }
          f++; // Skip double quote
          while (*f && (*f != '"')) {
            formatted[i++] = *f++;
          }
          if (*f != '"') {
            ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
            return NULL;
          }
          len = 1;
        } else if (ispunct(*f) || isspace(*f)) {
          if (ispunct(*f) && (abbr_fmt || ts_fmt)) {
            ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
            return NULL;
          }
          formatted[i++] = *f;
          len = 1;
        } else {
          code = oratim_best_match(f);
          if ((code != ORATIM_AMBIGUOUS)
              && (code != ORATIM_NOT_FOUND)) {
            len = strlen(oratim_keyword(code));
          } else {
            ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
            return NULL;
          }
          str = 0;
          switch(code) {
            // Variable "what" is set to what should be extracted
            // from the date. If nothing has to be extracted,
            // 'what' is set to KSU_DATE_PART_NOT_FOUND.
            case ORATIM_YYYY:  // 4-digit year
            case ORATIM_Y_YYY: // Same as before but as Y,YYY (with a comma)
            case ORATIM_SYYYY: // S prefixes BC dates with a minus sign.
            case ORATIM_YYY:   // Last 3, 2, or 1 digit(s) of year.
            case ORATIM_YY:
            case ORATIM_Y:
            case ORATIM_RRRR:  // Same as YYYY for output
            case ORATIM_RR:    // Same as YY for output
            case ORATIM_YEAR:  // Year, spelled out;
            case ORATIM_SYEAR: // S prefixes BC dates with a minus sign (-).
                 what = KSU_DATE_PART_YEAR;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_X:
                 // Locale radix character
                 // Ignore, that's for fractional seconds
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_WW: // Week of year (1-53) where week 1 starts on
                            // the first day of the year and continues to
                            // the seventh day of the year.
                 what = KSU_DATE_PART_YEAR;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_W:  // Week of month (1-5) where week 1 starts on
                            // the first day of the month and ends on the
                            // seventh.
                 what = KSU_DATE_PART_DAY;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_TZR:
            case ORATIM_TZM:
            case ORATIM_TZH:
            case ORATIM_TZD:
                 // Just ignore
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_TS: 
                 // Returns a value in the short time format.
                 // Makes the appearance of the time components (hour, minutes,
                 // and so forth) depend on language settings. 
                 //
                 // Restriction: You can specify this format only with the
                 // DL or DS element, separated by white space.
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (non_abbr) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   ts_fmt = 1;
                 }
                 if (ksu_localtime(t, &tm) != (KSU_TM_T *)NULL) {
                   switch(ksu_territory()) {
                     case KSU_TERR_US:
                     case KSU_TERR_GB:
                     case KSU_TERR_IE:
                     case KSU_TERR_AU:
                     case KSU_TERR_IN:
                          if (abbr_fmt) {
                            sprintf(buf, "%02hd:%02hd:%02hd %s",
                                    (short)(tm.hour>12?tm.hour-12:tm.hour),
                                    tm.min,
                                    tm.sec,
                                    (tm.hour>12?"PM":"AM"));
                          } else {
                            sprintf(buf, "%hd:%02hd:%02hd %s",
                                    (short)(tm.hour>12?tm.hour-12:tm.hour),
                                    tm.min,
                                    tm.sec,
                                    (tm.hour>12?"PM":"AM"));
                          }
                          break;
                     default:
                          sprintf(buf, "%02hd:%02hd:%02hd",
                                  tm.hour,
                                  tm.min,
                                  tm.sec);
                          break;
                   }
                   (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                   i = strlen(formatted);
                 }
                 break;
            case ORATIM_SSSSS: // Seconds past midnight (0-86399).
                 what = KSU_DATE_PART_SECONDS_IN_DAY;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_SS:    // Second (0-59).
                 what = KSU_DATE_PART_SECOND;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_CC:
            case ORATIM_SCC:   // Century.
                               // If the last 2 digits of a 4-digit year are
                               // between 01 and 99 (inclusive), then
                               // the century is one greater than the first
                               // 2 digits of that year.
                               // If the last 2 digits of a 4-digit year
                               // are 00, then the century is the same as the
                               // first 2 digits of that year.
                               // For example, 2002 returns 21; 2000 returns
                               // 20.
                 what = KSU_DATE_PART_CENTURY;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_Q:     // Quarter of year (1, 2, 3, 4;
                               // January - March = 1).
                 what = KSU_DATE_PART_QUARTER;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_PM:
            case ORATIM_P_M_:
            case ORATIM_AM:
            case ORATIM_A_M_:
                 // Use a xx or xxxx place holder
                 for (k = 0; k < len; k++) {
                   formatted[i++] = 'x';
                 }
                 am = IND_SET | (len == 4 ? IND_DOTS : 0)
                              | (isupper(*f) ? IND_FIRST_UPPER : 0)
                              | (isupper(*(f + (len == 4 ? 2: 1))) ?
                                          IND_SECOND_UPPER : 0);
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_J:     // Julian day; the number of days since
                               // January 1, 4712 BC.
                 what = KSU_DATE_PART_JULIAN_NUM;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_RM:    // Roman numeral month (I-XII; January = I).
            case ORATIM_MM:    // Month (01-12; January = 01).
                 what = KSU_DATE_PART_MONTH;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_MONTH: // Name of month, padded with blanks to display
                               // width of the widest name of month in the date
                               // language used for this element.
                 what = KSU_DATE_PART_MONTHNAME;
                 str = 1;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_MON:   // Abbreviated name of month.
                 what = KSU_DATE_PART_MONTHABBREV;
                 str = 1;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_IYYY:  // Last 4, 3, 2, or 1 digit(s) of ISO year.
            case ORATIM_IYY:
            case ORATIM_IY:
            case ORATIM_I:
                 what = KSU_DATE_PART_ISOYEAR;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_IW:    // Week of year (1-52 or 1-53) based on the
                               // ISO standard.
                 what = KSU_DATE_PART_ISOWEEK;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_HH24:  // Hour of day (0-23).
            case ORATIM_HH12:  // Hour of day (1-12).
            case ORATIM_HH:    // Same as HH12
                 what = KSU_DATE_PART_HOUR;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_MI:    // Minute (0-59).
                 what = KSU_DATE_PART_MINUTE;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_FX:
                 fm = 0;
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_FM:
                 fm = 1;
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_FF:
                 // Fractional seconds - ignore.
                 // Can be followed by digits that must be skipped
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 k = len;
                 while (isdigit(f[k])) {
                   k++;
                   len++;
                 }
                 break;
            case ORATIM_EE:
            case ORATIM_E:
                 // Era - ignore
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_DY:   // Abbreviated name of day.
                 what = KSU_DATE_PART_WEEKDAYABBREV;
                 str = 1;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_DS:   // Date short format
            case ORATIM_DL:   // Date long format
                 // Can only be specified with TS
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (non_abbr) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   abbr_fmt = 1;
                 }
                 if (code == ORATIM_DS) {
                   if (ksu_short_date(t, buf)) {
                     (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                     i = strlen(formatted);
                   }
                 } else {
                   if (ksu_localtime(t, &tm) != (KSU_TM_T *)NULL) {
                     if (ksu_extract_str(t, KSU_DATE_PART_WEEKDAYNAME,
                                            buf, FORMATTED_LEN) != 1) {
                       buf[0] = '\0';
                     }
                     (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                     strcat(formatted, ", ");
                     i = strlen(formatted);
                     if (ksu_extract_str(t, KSU_DATE_PART_MONTHNAME,
                                            buf, FORMATTED_LEN) != 1) {
                       buf[0] = '\0';
                     }
                     (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                     i = strlen(formatted);
                     sprintf(buf, " %02hd, %hd", tm.mday, tm.year);
                     (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                     i = strlen(formatted);
                   } 
                 }
                 break;
            case ORATIM_DDD:  // Day of year (1-366).
                 what = KSU_DATE_PART_DAYOFYEAR;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_DD:   // Day of month (1-31).
                 what = KSU_DATE_PART_DAY;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_DAY:  // Name of day, padded with blanks to display
                              // width of the widest name of day in the date
                              // language used for this element.
                 what = KSU_DATE_PART_WEEKDAYNAME;
                 str = 1;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_D:    // Day of week (1-7), 1 = Monday
                 what = KSU_DATE_PART_ISOWEEKDAY;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            // BC/AD indicator
            case ORATIM_BC:
            case ORATIM_B_C_:
            case ORATIM_AD:
            case ORATIM_A_D_:
                 // Use a zz or zzzz place holder
                 for (k = 0; k < len; k++) {
                   formatted[i++] = 'z';
                 }
                 bc = IND_SET | (len == 4 ? IND_DOTS : 0)
                              | (isupper(*f) ? IND_FIRST_UPPER : 0)
                              | (isupper(*(f + (len == 4 ? 2: 1))) ?
                                          IND_SECOND_UPPER : 0);
                 what = KSU_DATE_PART_NOT_FOUND;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                   return NULL;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            default:
                 // REALLY not found or suffix (processed elsewhere)
                 // Generate an error
                 ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_char");
                 return NULL;
          }
          // Check whether there is a suffix
          suf = f + len;
          code_suf = oratim_best_match(suf);
          rendering = 0;
          switch (code_suf) {
            // Suffixes - ordinal and spelling out
            case ORATIM_TH:
                 rendering = NUM_ORD;
                 break;
            case ORATIM_SP:
                 rendering = NUM_SPELL;
                 break;
            case ORATIM_SPTH:
            case ORATIM_THSP:
                 rendering = NUM_SPELL | NUM_ORD;
                 break;
            default:
                 // Ignore for now
                 break;
          }
          if (what != KSU_DATE_PART_NOT_FOUND) {
            if (str) {
              if (ksu_extract_str(t, what, buf, FORMATTED_LEN) != 1) {
                buf[0] = '\0';
              }
              // String results won't be transformed, except for
              // capitalization and padding
              if (islower(*f)) {
                cap = 0;
              } else {
                if (islower(*(f+1))) {
                  cap = 2;
                } else {
                  cap = 1;
                }
              }
              capitalize(buf, capbuf, cap);
              if (!fm) {
                switch(code) {
                  case ORATIM_MONTH: 
                       pad = ksu_maxmonth();
                       break;
                  case ORATIM_MON: 
                       pad = ksu_maxmon();
                       break;
                  case ORATIM_DAY: 
                       pad = ksu_maxday();
                       break;
                  case ORATIM_DY: 
                       pad = ksu_maxdy();
                       break;
                  default :
                       pad = 0; 
                       break;
                }
              } else {
                pad = 0;
              }
              (void)strncpy(&(formatted[i]), capbuf, FORMATTED_LEN - i);
              i = strlen(formatted);
              if (pad) {
                for (k = (int)strlen(capbuf); k < pad; k++) {
                  formatted[i++] = ' ';
                }
                formatted[i] = '\0';
              }
            } else {
              if (ksu_extract_int(t, what, &val) != 1) {
                val = 0;
              }
              // Numerical results may be widely transformed
              processed = 0;
              if (rendering & NUM_SPELL) {
                if (islower(*f)) {
                  cap = 0;
                } else {
                  if (islower(*(f+1))) {
                    cap = 2;
                  } else {
                    cap = 1;
                  }
                }
                if (rendering & NUM_ORD) {
                  spell_out_day_ord((int)val, buf, cap);
                } else {
                  spell_out_day((int)val, buf, cap);
                }
                (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                processed = 1;
              }
              if (!processed) {
                switch(code) {
                  case ORATIM_YYYY:  // 4-digit year
                  case ORATIM_Y_YYY: // Same as before but as Y,YYY (with a
                                     // comma)
                  case ORATIM_SYYYY: // S prefixes BC dates with a minus sign.
                  case ORATIM_YYY:   // Last 3, 2, or 1 digit(s) of year.
                  case ORATIM_YY:
                  case ORATIM_Y:
                  case ORATIM_RRRR:  // Same as YYYY for output
                  case ORATIM_RR:    // Same as YY for output
                       year = (int)val;
                       if (val < 0) {
                         val *= -1;
                       }
                       if (code == ORATIM_SYYYY) {
                         if (!fm) {
                           sprintf(buf, "+%04d", year);
                         } else {
                           sprintf(buf, "+%d", year);
                         }
                       } else if (code == ORATIM_Y_YYY) {
                         if (!fm) {
                           sprintf(buf, "'%04d", (int)val);
                         } else {
                           sprintf(buf, "'%d", (int)val);
                         }
                       } else {
                         if (!fm) {
                           sprintf(buf, "%04d", (int)val);
                         } else {
                           sprintf(buf, "%d", (int)val);
                         }
                       }
                       k = 0;
                       if (len < 4) {
                         if (!fm) {
                           for (k = 0; k < strlen(buf) - len; k++) {
                             buf[k] = ' ';
                           }
                           k = 0;
                         } else {
                           k = strlen(buf) - len;
                         }
                       }
                       (void)strncpy(&(formatted[i]),
                                     &(buf[k]),
                                     FORMATTED_LEN - i);
                       break;
                  case ORATIM_YEAR:  // Year, spelled out;
                  case ORATIM_SYEAR: // S prefixes BC dates with a minus
                                     // sign (-).
                       year = (int)val;
                       if (val < 0) {
                         val *= -1;
                       }
                       p = f;
                       if (code == ORATIM_SYEAR) {
                         if (year < 0) {
                           formatted[i++] = '-';
                         } else {
                           formatted[i++] = '+';
                         }
                         p++;
                       }
                       if (islower(*p)) {
                         cap = 0;
                       } else {
                         if (islower(*(p+1))) {
                           cap = 2;
                         } else {
                           cap = 1;
                         } 
                       } 
                       spell_out_year(year, buf, cap);
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_WW: // Week of year (1-53)
                       if (!fm) {
                         sprintf(buf, "%02ld", val);
                       } else {
                         sprintf(buf, "%ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_W:  // Week of month (1-5)
                       sprintf(buf, "%ld", val);
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_SSSSS: // Seconds past midnight (0-86399).
                       if (!fm) {
                         sprintf(buf, "%05ld", val);
                       } else {
                         sprintf(buf, "%ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_SS:    // Second (0-59).
                       if (!fm) {
                         sprintf(buf, "%02ld", val);
                       } else {
                         sprintf(buf, "%ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_CC:
                  case ORATIM_SCC:   // Century.
                       year = (int)val*100; // In case BC/AD would be required
                       if (val < 0) {
                         val *= -1;
                       }
                       if (code == ORATIM_SCC) {
                         if (!fm) {
                           sprintf(buf, "+%02d",
                                      (int)((year < 0 ? -1 : 1) * val));
                         } else {
                           sprintf(buf, "+%d",
                                      (int)((year < 0 ? -1 : 1) * val));
                         }
                       } else {
                         if (!fm) {
                           sprintf(buf, "%02d", (int)val);
                         } else {
                           sprintf(buf, "%d", (int)val);
                         }
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_Q:     // Quarter of year (1, 2, 3, 4;
                                     // January - March = 1).
                       sprintf(buf, "%ld", val);
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_J:     // Julian day; the number of days since
                                     // January 1, 4712 BC.
                       if (!fm) {
                         sprintf(buf, "%ld", val);
                       } else {
                         sprintf(buf, "%07ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_RM:    // Roman numeral month
                       // Last parameter = ltrim
                       if (roman((int)val, buf, (char)isupper(*f), (char)1)){
                         (void)strncpy(&(formatted[i]),
                                       buf, FORMATTED_LEN - i);
                       }
                       if (!fm) {
                         i = strlen(formatted);
                         for (k = strlen(buf); k < 4; k++) {
                           formatted[i++] = ' ';
                         }
                         formatted[i] = '\0';
                       }
                       break;
                  case ORATIM_MM:    // Month (01-12; January = 01).
                       if (!fm) {
                         sprintf(buf, "%02ld", val);
                       } else {
                         sprintf(buf, "%ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_IYYY:  // Last n digit(s) of ISO year.
                  case ORATIM_IYY:
                  case ORATIM_IY:
                  case ORATIM_I:
                       year = (int)val;
                       if (val < 0) {
                         val *= -1;
                       }
                       sprintf(buf, "%04d", (int)val);
                       k = 0;
                       if (len < 4) {
                         if (!fm) {
                           for (k = 0; k < strlen(buf) - len; k++) {
                             buf[k] = ' ';
                           }
                           k = 0;
                         } else {
                           k = strlen(buf) - len;
                         }
                       }
                       (void)strncpy(&(formatted[i]),
                                     &(buf[k]),
                                     FORMATTED_LEN - i);
                       what = KSU_DATE_PART_ISOYEAR;
                       break;
                  case ORATIM_IW:    // Week of year (1-52 or 1-53)
                       if (!fm) {
                         sprintf(buf, "%02ld", val);
                       } else {
                         sprintf(buf, "%ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_HH24:  // Hour of day (0-23).
                  case ORATIM_HH12:  // Hour of day (1-12).
                  case ORATIM_HH:    // Same as HH12
                       hour = (int)val;
                       if (code != ORATIM_HH24) {
                         if (val >= 13) {
                           val -= 12;
                         }
                       }
                       if (!fm) {
                         sprintf(buf, "%02ld", val);
                       } else {
                         sprintf(buf, "%ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_MI:    // Minute (0-59).
                       if (!fm) {
                         sprintf(buf, "%02ld", val);
                       } else {
                         sprintf(buf, "%ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_DDD:  // Day of year (1-366).
                       if (!fm) {
                         sprintf(buf, "%03ld", val);
                       } else {
                         sprintf(buf, "%ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_DD:   // Day of month (1-31).
                       if (!fm) {
                         sprintf(buf, "%02ld", val);
                       } else {
                         sprintf(buf, "%ld", val);
                       }
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  case ORATIM_D:    // Day of week (1-7).
                       sprintf(buf, "%ld", val);
                       (void)strncpy(&(formatted[i]), buf, FORMATTED_LEN - i);
                       break;
                  default:
                       // REALLY not found or suffix (processed elsewhere)
                       // Generate an error
                       ksu_err_msg(context, KSU_ERR_INV_FORMAT,
                                            fmt, "to_char");
                       return NULL;
                }
                if (rendering & NUM_ORD) {
                  i = strlen(formatted);
                  if ((val <= 10) || (val >= 20)) {
                    switch (val % 10) {
                      case 1:
                          strcpy(buf, "st");
                          break;
                      case 2:
                          strcpy(buf, "nd");
                          break;
                      case 3:
                          strcpy(buf, "rd");
                          break;
                      default:
                          strcpy(buf, "th");
                          break;
                    }
                 } else {
                    strcpy(buf, "th");
                  }
                  if (islower(*suf)) {
                    cap = 0;
                  } else {
                    if (islower(*(suf+1))) {
                      cap = 2;
                    } else {
                      cap = 1;
                    }
                  }
                  capitalize(buf, capbuf, cap);
                  (void)strncpy(&(formatted[i]), capbuf, FORMATTED_LEN - i);
                }
              }
              i = strlen(formatted);
            }
          }
          if (rendering) {
            // Be ready to skip rendering info
            f += strlen(oratim_keyword(code_suf));
          }
        }
        f += len;
      }
      formatted[i] = '\0';
    }
    // Look for am/pm (xx) or bc/ad (zz) placeholders
    if (am && ((p = strstr(formatted, "xx")) != NULL)) {
      if (hour == 25) {
         // We don't have it yet!
         if (ksu_extract_int(t, KSU_DATE_PART_HOUR, &val) != 1) {
            val = 0;
         }
         hour = (int)val;
      }
      k = 1;  // Position of second letter
      if (am & IND_DOTS) {
        p[1] = '.';
        p[3] = '.';
        k = 2;
      }
      if (hour >= 12) { // pm
        p[0] = (am & IND_FIRST_UPPER ? 'P' : 'p');
        p[k] = (am & IND_SECOND_UPPER ? 'M' : 'm');
      } else { // am
        p[0] = (am & IND_FIRST_UPPER ? 'A' : 'a');
        p[k] = (am & IND_SECOND_UPPER ? 'M' : 'm');
      }
    }
    if (bc && ((p = strstr(formatted, "zz")) != NULL)) {
      if (year == 0) {
         // We don't have it yet!
         if (ksu_extract_int(t, KSU_DATE_PART_YEAR, &val) != 1) {
            val = 0;
         }
         year = (int)val;
      }
      k = 1;  // Position of second letter
      if (bc & IND_DOTS) {
        p[1] = '.';
        p[3] = '.';
        k = 2;
      }
      if (year < 0) { // bc
        p[0] = (bc & IND_FIRST_UPPER ? 'B' : 'b');
        p[k] = (bc & IND_SECOND_UPPER ? 'C' : 'c');
      } else { // ad
        p[0] = (bc & IND_FIRST_UPPER ? 'A' : 'a');
        p[k] = (bc & IND_SECOND_UPPER ? 'D' : 'd');
      }
    }
    return formatted;
}

extern void ora_to_char(sqlite3_context *context,
                        int              argc,
                        sqlite3_value  **argv) {

         char       num = 0;
         char       blob = 0;
         char       any_text = 0;
   const char      *fmt;
         char      *rslt;
         char       formatted[FORMAT_LEN];
         int        typ;
         KSU_TIME_T t;

   _ksu_check_arg_cnt(argc, 1, 2, "to_char");
   // Can be called for either numbers or dates
   typ = sqlite3_value_type(argv[0]);
   switch (typ) {
       case SQLITE_INTEGER:
       case SQLITE_FLOAT:
            num = 1;
            break;
       case SQLITE_TEXT:
            // Check that it looks like a date
            if (!ksu_is_datetime((const char *)sqlite3_value_text(argv[0]),
                                 &t, (char)0)) {
              any_text = 1; 
            }
            break;
       case SQLITE_NULL:
            sqlite3_result_null(context);
            return;
       default: // BLOB ? Return as hexa
            blob = 1;
            break;
   }
   if (argc == 1) {
      if (blob) {
        // Return as hexa
        int            len = sqlite3_value_bytes(argv[0]);
        int            k;
        unsigned char *p;
        char          *hex;
        char          *h;
        
        if ((hex = (char *)sqlite3_malloc(2*len+1)) == (char *)NULL) {
          sqlite3_result_error_nomem(context);
          return;
        }
        h = hex;
        p = (unsigned char *)sqlite3_value_blob(argv[0]);
        for (k = 0; k < len; k++) {
          sprintf(h, "%02X", p[k]);
          h += 2;
        }
        sqlite3_result_text(context, hex, 2 * len, sqlite3_free);
      } else {
        // Just return parameter as text (do nothing)
        // There is no space on the left if positive.
        sqlite3_result_value(context, argv[0]);
      }
      return;
   }
   if (blob || any_text) {
     // No format allowed
     ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME, 1, "to_char");
     return;
   }
   fmt = (const char *)sqlite3_value_text(argv[1]);
   if (num) {
     rslt = ora_format_number(context, argv[0], fmt, formatted);
   } else {
     rslt = ora_format_date(context, t, fmt, formatted);
   }
   if (!rslt) {
     return;  // Error message set in ora_format_xxxx
   }
   sqlite3_result_text(context, formatted, -1, SQLITE_TRANSIENT);
}
