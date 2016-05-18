#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_ora.h>

#define FORMAT_LEN     200
#define FORMATTED_LEN   64

#define INPUT_LEN   100

// Various pointer-moving macros
#define _skip_spaces(s)  {while(isspace(*s)){s++;}}
#define _skip_int(s)     {while(isdigit(*s)){s++;}}
// _skip_word() defined "negatively" because we may
// have UTF-8 characters
#define _skip_word(s)    {while(!isspace(*s) \
                                && !ispunct(*s) \
                                && !isdigit(*s)){s++;}}
#define _skip_time(s)    {while(isdigit(*s)||(*s==':')){s++;}}

extern void ora_to_date(sqlite3_context *context,
                        int              argc,
                        sqlite3_value  **argv) {
    //  1 or two arguments
    char       *str;
    char       *s;
    char       *f;
    int         code; 
    char        buf[FORMATTED_LEN];
    int         len;
    char        fm = 1;  // flag for variable model
    char        bc = 0;  // flag for bc/ad
    char        day_check = 0;
    char        has_time = 0;
    char        abbr_fmt = 0; // Abbreviated format - DS or DL
    char        ts_fmt = 0;   // Short time format
    char        non_abbr = 0; // Non-abbreviated format
    KSU_TM_T    tm;
    KSU_TIME_T  t;
    const char *fmt;
    char        result_date[DATE_LEN];
    int         pos;
    short       saved_day;

   _ksu_check_arg_cnt(argc, 1, 2, "to_date");
   _ksu_null_if_null_param(argc, argv);
   if (argc == 1) {
     if (ksu_is_datetime((const char *)sqlite3_value_text(argv[0]),
                         &t, (char)0)
         && ksu_localtime(t, &tm)) {
       // Return the date part
       (void)sprintf(result_date,
                     "%04hd-%02hd-%02hd",
                     tm.year, (short)(1 + tm.mon), tm.mday);
       sqlite3_result_text(context, result_date, -1, SQLITE_TRANSIENT);
     } else {
       ksu_err_msg(context, KSU_ERR_ARG_NOT_DATETIME, "to_date");
     }
     return;
   }
   // Clear tm
   (void)memset(&tm, 0, sizeof(KSU_TM_T));
   str = (char *)sqlite3_value_text(argv[0]);
   s = (char *)str;
   fmt = (const char *)sqlite3_value_text(argv[1]);
   f = (char *)fmt;
   // Let's try to move s and f in synch ...
   // By default the fill mode is fm, i.e. tolerant (blanks and
   // leading zeroes are ignored)
   while (*f) {
     // Punctuation and quoted text output as is in the format
     if (fm) {
       _skip_spaces(f);
       _skip_spaces(s);
     }
     if (*f) {
       if (*f == '"') {
         if (abbr_fmt || ts_fmt) {
           ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
           return;
         }
         f++; // Skip double quote
         while (*f && *s && (*f != '"')) {
           // If !fm, there must be an exact match (except for case)
           // If fm, spaces are ignored.
           if (fm) {
             _skip_spaces(f);
             _skip_spaces(s);
           }
           if (toupper(*s) == toupper(*f)) {
             s++;
             f++;
           } else {
             ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, "to_date");
             return;
           }
         }
         if (*f != '"') {
           ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, "to_date");
           return;
         }
         f++;
       } else if (isspace(*f)) {
         // Here we are in !fm mode, as spaces have otherwise
         // already been skipped at the beginning of the loop
         if (!isspace(*s)) {
           ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, "to_date");
           return;
         }
         f++;
         s++;
       } else if (ispunct(*f)) {
         if (abbr_fmt || ts_fmt) {
           ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
           return;
         }
         if (*s != *f) {
           ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, "to_date");
           return;
         }
         s++;
         f++;
       } else {
          code = oratim_best_match(f);
          if ((code != ORATIM_AMBIGUOUS)
              && (code != ORATIM_NOT_FOUND)) {
            len = strlen(oratim_keyword(code));
          } else {
            ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
            return;
          }
          switch(code) {
            case ORATIM_YYYY:  // 4-digit year
            case ORATIM_YYY:   // Last 3, 2, or 1 digit(s) of year.
            case ORATIM_YY:
            case ORATIM_Y:
            case ORATIM_RRRR:  // Same as YYYY
            case ORATIM_RR:
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 if (!sscanf(s, "%04hd", &(tm.year))) {
                   ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, "to_date");
                   return;
                 }
                 // Year provided may not be complete
                 if ((code != ORATIM_YYYY)
                     && (code != ORATIM_RRRR)) {
                   time_t     now = time(NULL);
                   struct tm  my_tm;
                   short      this_year;

                   // First get the current year
                   if ((now != (time_t)-1)
                       && localtime_r(&now, &my_tm)) {
                     this_year = my_tm.tm_year + 1900;
                   } else {
                     ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                     return;
                   }
                   switch(code) {
                     case ORATIM_YYY:   // Last 3, 2, or 1 digit(s) of year.
                        tm.year += (short)(this_year/1000)*1000;
                        break;
                     case ORATIM_YY:
                        tm.year += (short)(this_year/100)*100;
                        break;
                     case ORATIM_Y:
                        tm.year += (short)(this_year/10)*10;
                        break;
                     case ORATIM_RR:
                        if (tm.year <= 49) {
                          if ((this_year % 100) <= 49) {
                            tm.year += (short)(this_year/100)*100;
                          } else {
                            tm.year += (short)(1 + (this_year/100))*100;
                          }
                        } else {
                          if ((this_year % 100) <= 49) {
                            tm.year += (short)((this_year/100) - 1)*100;
                          } else {
                            tm.year += (short)(this_year/100)*100;
                          }
                        }
                        break;
                     default :
                        break;
                   }
                 }
                 // Move s
                 if (fm) {
                   _skip_int(s);
                 } else {
                   s += len;
                 }
                 break;
            case ORATIM_Y_YYY: // Same as before but as Y,YYY (with a comma)
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 {
                  int y1;
                  int y2;

                  // We ignore "a character" in case it would be
                  // something else than a comma
                  if (sscanf(s, "%d%*c%d", &y1, &y2) == 2) {
                    tm.year = (short)(1000 *y1 + y2);
                  } else {
                    ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                    return;
                  }
                 }
                 // Move s
                 if (fm) {
                   pos = 0;
                   while ((isdigit(*s) 
                           || ispunct(*s)
                           || isspace(*s))
                          && (pos < len)) {
                     s++;
                     pos++;
                   }
                 } else {
                   s += len;
                 }
                 break;
            case ORATIM_SYYYY: // S prefixes BC dates with a minus sign.
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 if (!sscanf(s, "+%04hd", &(tm.year))) {
                   ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                   return;
                 }
                 // Move s
                 if (fm) {
                   // Skip sign
                   s++;
                   _skip_int(s);
                 } else {
                   s += len;
                 }
                 break;
            /*
               Not valid with to_date()
            case ORATIM_X:
            case ORATIM_FF:
            case ORATIM_TZR:
            case ORATIM_TZM:
            case ORATIM_TZH:
            case ORATIM_TZD:
                 break;
            */
            case ORATIM_TS: 
                 // Returns a value in the short time format.
                 // Makes the appearance of the time components (hour, minutes,
                 // and so forth) depend on language settings. 
                 //
                 // Restriction: You can specify this format only with the
                 // DL or DS element, separated by white space.
                 if (non_abbr) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   ts_fmt = 1;
                 }
                 has_time = 1;
                 {
                  int   clock_type = ksu_clock();
                  char  ampm[3];
                  char  my_fmt[DATE_LEN];

                  if (clock_type == 12) {
                    if (fm) {
                      strcpy(my_fmt, "%hd:%hd:%hd %2s");
                    } else {
                      strcpy(my_fmt, "%02hd:%02hd:%02hd %2s");
                    }
                    if (sscanf(s, my_fmt,
                                  &tm.hour,
                                  &tm.min,
                                  &tm.sec,
                                  ampm) == 4) {
                      switch (toupper(ampm[0])) {
                        case 'A': // Nothing to do
                             break;
                        case 'P': 
                             tm.hour += (short)12;
                             break;
                        default:
                             ksu_err_msg(context, KSU_ERR_DATE_CONV,
                                         "to_date");
                             return;
                      }
                    } else {
                      ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                      return;
                    }                     
                  } else {
                     // 24 hour clock
                    if (fm) {
                      strcpy(my_fmt, "%hd:%hd:%hd");
                    } else {
                      strcpy(my_fmt, "%02hd:%02hd:%02hd");
                    }
                    if ((sscanf(s, my_fmt,
                                   &tm.hour,
                                   &tm.min,
                                   &tm.sec) != 3)
                        || (tm.hour < 0)
                        || ((clock_type == 24) && (tm.hour > 23))
                        || ((clock_type == 12) && (tm.hour > 12))
                        || (tm.min < 0)
                        || (tm.min > 59)
                        || (tm.sec < 0)
                        || (tm.sec > 59)) {
                      ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                      return;
                    }                     
                  }
                  // Move s
                  _skip_time(s);
                  if (clock_type == 12) {
                    _skip_spaces(s);
                    _skip_word(s);
                  } 
                 }
                 break;
            case ORATIM_SSSSS: // Seconds past midnight (0-86399).
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 has_time = 1;
                 {
                  int  sec;
                  char my_fmt[DATE_LEN];

                  if (fm) {
                    strcpy(my_fmt, "%d");
                  } else {
                    strcpy(my_fmt, "%05d");
                  }
                  if (sscanf(s, my_fmt, &sec)
                      && (sec >= 0)
                      && (sec < 86400)) {
                    tm.hour = (short)(sec / 3600);
                    tm.min = (short)((sec - tm.hour * 3600)/60);
                    tm.sec = (short)(sec % 60);
                  } else {
                    ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                    return;
                  }
                 }
                 _skip_int(s);
                 break;
            case ORATIM_SS:    // Second (0-59).
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 has_time = 1;
                 {
                  char my_fmt[DATE_LEN];

                  if (fm) {
                    strcpy(my_fmt, "%hd");
                  } else {
                    strcpy(my_fmt, "%02hd");
                  }
                  if (!sscanf(s, my_fmt, &(tm.sec))
                     || (tm.sec < 0)
                     || (tm.sec > 59)) {
                    ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                    return;
                  }
                 }
                 _skip_int(s);
                 break;
            case ORATIM_AM:
            case ORATIM_PM:
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 has_time = 1;
                 if (strncasecmp(s, "am", 2)
                     && strncasecmp(s, "pm", 2)) {
                   ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                   return;
                 }
                 if (strncasecmp(s, "pm", 2) == 0) {
                   tm.hour += 12;
                 }
                 s += 2; 
                 break;
            case ORATIM_A_M_:
            case ORATIM_P_M_:
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 has_time = 1;
                 if (strncasecmp(s, "a.m.", 4)
                     && strncasecmp(s, "p.m.", 4)) {
                   ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                   return;
                 }
                 if (strncasecmp(s, "p.m.", 4) == 0) {
                   tm.hour += 12;
                 }
                 s += 4; 
                 break;
            case ORATIM_J:     // Julian day
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 {
                  long j;

                  if (sscanf(s, "%ld", &j)
                      && (j > 0)) {
                    KSU_TIME_T my_t = KSU_TIME_INIT;

                    my_t.jdn = (long long)j;
                    // We are going to erase everything that might
                    // already have been set
                    // In the unlikely event we have a time:
                    my_t.sec = (unsigned int)(3600*tm.hour
                                             + 60*tm.min + tm.sec);
                    if (!ksu_localtime(my_t, &tm)) {
                      ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                      return;
                    }
                  }
                 }
                 _skip_int(s);
                 break;
            case ORATIM_RM:    // Roman numeral month (I-XII; January = I).
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 {
                  char *ok_rm = "ivx";
                  int   m;

                  pos = 0;
                  while (strchr(ok_rm, tolower(*s))) {
                    buf[pos++] = *s++;
                  }
                  buf[pos] = '\0';
                  m = ksu_compute_roman(buf);
                  if ((m >= 1) && (m <= 12)) {
                    tm.mon = (short)(m - 1);
                  } else {
                   ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, "to_date");
                   return;
                  }
                 }
                 // Nothing to skip here
                 break;
            case ORATIM_MM:    // Month (01-12; January = 01).
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 {
                  char my_fmt[DATE_LEN];

                  if (fm) {
                    strcpy(my_fmt, "%hd");
                  } else {
                    strcpy(my_fmt, "%02hd");
                  }
                  if (sscanf(s, my_fmt, &(tm.mon))
                      && (tm.mon >= 1)
                      && (tm.mon <= 12)) {
                    (tm.mon)--;
                  } else {
                   ksu_err_msg(context, KSU_ERR_INV_MONTH, s, "to_date");
                   return;
                  }
                 }
                 _skip_int(s);
                 break;
            case ORATIM_MONTH: // Name of month
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 {
                  int maxlen = ksu_maxmonth();
                  int m;

                  pos = 0;
                  while ((pos < maxlen)
                          && !isspace(*s)
                          && !ispunct(*s)
                          && !isdigit(*s)) {
                    buf[pos++] = *s++;
                  }
                  buf[pos] = '\0';
                  m = ksu_month_by_name(buf);
                  if (m >= 0) {
                    tm.mon = m;
                  } else {
                    ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, "to_date");
                    return;
                  }
                 }
                 break;
            case ORATIM_MON:   // Abbreviated name of month.
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 {
                  int maxlen = ksu_maxmon();
                  int m;

                  pos = 0;
                  while ((pos < maxlen)
                          && !isspace(*s)
                          && !ispunct(*s)
                          && !isdigit(*s)) {
                    buf[pos++] = *s++;
                  }
                  buf[pos] = '\0';
                  m = ksu_month_by_abbr(buf);
                  if (m >= 0) {
                    tm.mon = m;
                  } else {
                    ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, "to_date");
                    return;
                  }
                 }
                 break;
            case ORATIM_HH24:  // Hour of day (0-23).
            case ORATIM_HH12:  // Hour of day (1-12).
            case ORATIM_HH:    // Same as HH12
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 has_time = 1;
                 {
                  char my_fmt[DATE_LEN];

                  if (fm) {
                    strcpy(my_fmt, "%hd");
                  } else {
                    strcpy(my_fmt, "%02hd");
                  }
                  if (!sscanf(s, my_fmt, &(tm.hour))) {
                    ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                    return;
                  }
                  if ((code != ORATIM_HH24) 
                      && ((tm.hour < 0)
                         || (tm.hour > 12))) {
                    char bad_hour[5];
                    sprintf(bad_hour, "%hd", tm.hour);
                    ksu_err_msg(context, KSU_ERR_INV_HOUR, bad_hour, "to_date");
                    return;
                  }
                 }
                 _skip_int(s);
                 break;
            case ORATIM_MI:    // Minute (0-59).
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 has_time = 1;
                 {
                  char my_fmt[DATE_LEN];

                  if (fm) {
                    strcpy(my_fmt, "%hd");
                  } else {
                    strcpy(my_fmt, "%02hd");
                  }
                  if (!sscanf(s, my_fmt, &(tm.min))
                     || (tm.min < 0)
                     || (tm.min > 59)) {
                    ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                    return;
                  }
                 }
                 _skip_int(s);
                 break;
            case ORATIM_FX:
                 fm = 0;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 break;
            case ORATIM_FM:
                 fm = 1;
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 break;
           /* Era - unsupported
            case ORATIM_EE:
            case ORATIM_E:
                 break;
           */
            case ORATIM_DY:   // Abbreviated name of day.
                 // Not really sure what to do with this.
                 // Generate an error if inconsistent with other data.
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 day_check = 1;
                 {
                  int maxlen = ksu_maxdy();
                  int d;

                  pos = 0;
                  while ((pos < maxlen)
                          && !isspace(*s)
                          && !ispunct(*s)
                          && !isdigit(*s)) {
                    buf[pos++] = *s++;
                  }
                  buf[pos] = '\0';
                  d = ksu_day_by_abbr(buf);
                  if (d >= 0) {
                    tm.wday = d;
                  } else {
                    ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, "to_date");
                    return;
                  }
                 }
                 break;
            case ORATIM_DS:   // Date short format
            case ORATIM_DL:   // Date long format
                 // Can only be specified with TS
                 if (non_abbr) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   abbr_fmt = 1;
                 }
                 if (code == ORATIM_DS) {
                   // Check the format and the order of D, M and Y
                   char  dsfmt[DATE_LEN];
                   short dt_order;
                   int   scanned = 0;

                   if (ksu_short_date_fmt(dsfmt, &dt_order)) {
                     switch (dt_order) {
                       case KSU_DATE_ORDER_YMD:
                            scanned = sscanf(s, dsfmt,
                                             &tm.year, &tm.mon, &tm.mday);
                            break;
                       case KSU_DATE_ORDER_MDY:
                            scanned = sscanf(s, dsfmt,
                                             &tm.mon, &tm.mday, &tm.year);
                            break;
                       case KSU_DATE_ORDER_DMY:
                            scanned = sscanf(s, dsfmt,
                                             &tm.mday, &tm.mon, &tm.year);
                            break;
                       default :
                            break;
                     }
                     if (scanned == 3) {
                       tm.mon--;
                       while (!isspace(*s)) {
                         s++;
                       }
                     } else {
                       ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                       return;
                     }
                   } else {
                     ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                     return;
                   }
                 } else {
                   // Long format
                   char monthname[DATE_LEN];
                   char dayname[DATE_LEN];
                   int  d;
                   int  m;

                   if (sscanf(s, "%s, %s %02hd, %hd",
                                 dayname, monthname,
                                 &tm.mday, &tm.year) == 4) {
                     d = ksu_day_by_name(dayname);
                     if (d >= 0) {
                       tm.wday = (short)d;
                     } else {
                       ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, "to_date");
                       return;
                     }
                     m = ksu_month_by_name(monthname);
                     if (m >= 0) {
                       tm.mon = (short)m;
                     } else {
                       ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, "to_date");
                       return;
                     }
                     day_check = 1;
                     s += (strlen(dayname) + strlen(monthname)
                           + 7); // Brings us to the year
                     _skip_int(s);
                   } else {
                     ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                     return;
                   }
                 }
                 break;
            case ORATIM_DDD:  // Day of year (1-366).
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 {
                  char my_fmt[DATE_LEN];
                  short ddd;

                  if (fm) {
                    strcpy(my_fmt, "%hd");
                  } else {
                    strcpy(my_fmt, "%03hd");
                  }
                  if (!sscanf(s, my_fmt, &ddd)) {
                    ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                    return;
                  }
                 }
                 _skip_int(s);
                 break;
            case ORATIM_DD:   // Day of month (1-31).
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 {
                  char my_fmt[DATE_LEN];

                  if (fm) {
                    strcpy(my_fmt, "%hd");
                  } else {
                    strcpy(my_fmt, "%02hd");
                  }
                  if (!sscanf(s, my_fmt, &(tm.mday))) {
                    ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                    return;
                  }
                 }
                 _skip_int(s);
                 break;
            case ORATIM_DAY:  // Name of day
                 // Not really sure what to do with this.
                 // Generate an error if inconsistent with other data.
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 day_check = 1;
                 {
                  int maxlen = ksu_maxday();
                  int d;

                  pos = 0;
                  while ((pos < maxlen)
                          && !isspace(*s)
                          && !ispunct(*s)
                          && !isdigit(*s)) {
                    buf[pos++] = *s++;
                  }
                  buf[pos] = '\0';
                  d = ksu_day_by_name(buf);
                  if (d >= 0) {
                    tm.wday = d;
                  } else {
                    ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, "to_date");
                    return;
                  }
                 }
                 break;
            case ORATIM_D:    // Day of week (1-7), 1 = Monday
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 {
                  short wday;
                  if (isdigit(*s)) {
                    wday = (short)(*s - '0');
                    if ((wday >= 1) && (wday <= 7)) {
                      tm.wday = wday % 7;
                      day_check = 1;
                    } else {
                      char day[2];
                      day[0] = *s;
                      day[1] = '\0';
                      ksu_err_msg(context, KSU_ERR_INV_DAY, day, "to_date");
                      return;
                    }
                  } else {
                    ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                    return;
                  }
                 }
                 s++;
                 break;
            // BC/AD indicator
            case ORATIM_BC:
            case ORATIM_AD:
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 if (strncasecmp(s, "bc", 2)
                     && strncasecmp(s, "ad", 2)) {
                   ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                   return;
                 }
                 if (strncasecmp(s, "bc", 2) == 0) {
                   bc = 1;
                 }
                 s += 2; 
                 break;
            case ORATIM_B_C_:
            case ORATIM_A_D_:
                 if (abbr_fmt || ts_fmt) {
                   ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                   return;
                 } else {
                   non_abbr = 1;
                 }
                 if (strncasecmp(s, "b.c.", 4)
                     && strncasecmp(s, "a.d.", 4)) {
                   ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
                   return;
                 }
                 if (strncasecmp(s, "b.c.", 4) == 0) {
                   bc = 1;
                 }
                 s += 4; 
                 break;
            default:
                 // Other formats are only supported with to_char()
                 ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, "to_date");
                 return;
          }
          f += len;
        }
      }
    }
    // If we reach this stage we have scanned everything successfully.
    // We still have to check that it's a correct date.
    if (bc && (tm.year > 0)) {
      tm.year *= -1;
    }
    if (day_check) {
      saved_day = tm.wday;
    }
    // Try to convert to KSU_TIME_T and back
    t = ksu_mktime(&tm);
    if (!_ksu_valid_date(t)) {
      ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
      return;
    }
    // Back now
    if (ksu_localtime(t, &tm)) {
      if (day_check) {
        if (tm.wday != saved_day) { // Flop
          ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
          return;
        }
      }
      if (has_time) {
        (void)sprintf(result_date, "%hd-%02hd-%02hd %02hd:%02hd:%02hd",
                      tm.year, (short)(tm.mon + 1), tm.mday,
                      tm.hour, tm.min, tm.sec);
      } else {
        (void)sprintf(result_date, "%hd-%02hd-%02hd",
                      tm.year, (short)(tm.mon + 1), tm.mday);
      }
      sqlite3_result_text(context, result_date, -1, SQLITE_TRANSIENT);
    } else {
      ksu_err_msg(context, KSU_ERR_DATE_CONV, "to_date");
      return;
    }
}
