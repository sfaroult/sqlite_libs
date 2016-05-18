#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_pg.h>

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


// Called by both pg_to_date and pg_to_timestamp
extern void pg_to_date_timestamp(sqlite3_context *context,
                                 int              argc,
                                 sqlite3_value  **argv,
                                 char            *funcname) {
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
    KSU_TM_T    tm;
    KSU_TIME_T  t;
    const char *fmt;
    char        result_date[DATE_LEN];
    int         pos;
    short       saved_day;

    if (ksu_prm_ok(context, argc, argv, funcname,
                   KSU_PRM_TEXT, KSU_PRM_TEXT)) {
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
                ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, funcname);
                return;
              }
            }
            if (*f != '"') {
              ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, funcname);
              return;
            }
            f++;
          } else if (isspace(*f)) {
            // Here we are in !fm mode, as spaces have otherwise
            // already been skipped at the beginning of the loop
            if (!isspace(*s)) {
              ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, funcname);
              return;
            }
            f++;
            s++;
          } else if (ispunct(*f)) {
            if (*s != *f) {
              ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, funcname);
              return;
            }
            s++;
            f++;
          } else {
             code = pgtimfmt_best_match(f);
             if ((code != PGTIMFMT_AMBIGUOUS)
                 && (code != PGTIMFMT_NOT_FOUND)) {
               len = strlen(pgtimfmt_keyword(code));
             } else {
               ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, funcname);
               return;
             }
             switch(code) {
               case PGTIMFMT_YYYY:  // 4-digit year
               case PGTIMFMT_YYY:   // Last 3, 2, or 1 digit(s) of year.
               case PGTIMFMT_YY:
               case PGTIMFMT_Y:
                    if (!sscanf(s, "%04hd", &(tm.year))) {
                      ksu_err_msg(context, KSU_ERR_FORMAT_MISMATCH, funcname);
                      return;
                    }
                    // Year provided may not be complete
                    if (code != PGTIMFMT_YYYY) {
                      time_t     now = time(NULL);
                      struct tm  my_tm;
                      short      this_year;
   
                      // First get the current year
                      if ((now != (time_t)-1)
                          && localtime_r(&now, &my_tm)) {
                        this_year = my_tm.tm_year + 1900;
                      } else {
                        ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                        return;
                      }
                      switch(code) {
                        case PGTIMFMT_YYY:   // Last 3, 2, or 1 digit(s) of year.
                           tm.year += (short)(this_year/1000)*1000;
                           break;
                        case PGTIMFMT_YY:
                           tm.year += (short)(this_year/100)*100;
                           break;
                        case PGTIMFMT_Y:
                           tm.year += (short)(this_year/10)*10;
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
               case PGTIMFMT_Y_YYY:
                    // Same as before but as Y,YYY (with a comma)
                    {
                     int y1;
                     int y2;
   
                     // We ignore "a character" in case it would be
                     // something else than a comma
                     if (sscanf(s, "%d%*c%d", &y1, &y2) == 2) {
                       tm.year = (short)(1000 *y1 + y2);
                     } else {
                       ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
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
               case PGTIMFMT_SSSS: // Seconds past midnight (0-86399).
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
                       ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                       return;
                     }
                    }
                    _skip_int(s);
                    break;
               case PGTIMFMT_SS:    // Second (0-59).
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
                       ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                       return;
                     }
                    }
                    _skip_int(s);
                    break;
               case PGTIMFMT_AM:
               case PGTIMFMT_PM:
                    has_time = 1;
                    if (strncasecmp(s, "am", 2)
                        && strncasecmp(s, "pm", 2)) {
                      ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                      return;
                    }
                    if (strncasecmp(s, "pm", 2) == 0) {
                      tm.hour += 12;
                    }
                    s += 2; 
                    break;
               case PGTIMFMT_A_M_:
               case PGTIMFMT_P_M_:
                    has_time = 1;
                    if (strncasecmp(s, "a.m.", 4)
                        && strncasecmp(s, "p.m.", 4)) {
                      ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                      return;
                    }
                    if (strncasecmp(s, "p.m.", 4) == 0) {
                      tm.hour += 12;
                    }
                    s += 4; 
                    break;
               case PGTIMFMT_J:     // Julian day
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
                         ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                         return;
                       }
                     }
                    }
                    _skip_int(s);
                    break;
               case PGTIMFMT_RM:    // Roman numeral month (I-XII; January = I).
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
                      ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, funcname);
                      return;
                     }
                    }
                    // Nothing to skip here
                    break;
               case PGTIMFMT_MM:    // Month (01-12; January = 01).
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
                      ksu_err_msg(context, KSU_ERR_INV_MONTH, s, funcname);
                      return;
                     }
                    }
                    _skip_int(s);
                    break;
               case PGTIMFMT_MONTH: // Name of month
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
                       ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, funcname);
                       return;
                     }
                    }
                    break;
               case PGTIMFMT_MON:   // Abbreviated name of month.
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
                       ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, funcname);
                       return;
                     }
                    }
                    break;
               case PGTIMFMT_HH24:  // Hour of day (0-23).
               case PGTIMFMT_HH12:  // Hour of day (1-12).
               case PGTIMFMT_HH:    // Same as HH12
                    has_time = 1;
                    {
                     char my_fmt[DATE_LEN];
   
                     if (fm) {
                       strcpy(my_fmt, "%hd");
                     } else {
                       strcpy(my_fmt, "%02hd");
                     }
                     if (!sscanf(s, my_fmt, &(tm.hour))) {
                       ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                       return;
                     }
                     if ((code != PGTIMFMT_HH24) 
                         && ((tm.hour < 0)
                            || (tm.hour > 12))) {
                       char bad_hour[5];
                       sprintf(bad_hour, "%hd", tm.hour);
                       ksu_err_msg(context, KSU_ERR_INV_HOUR,
                                   bad_hour, funcname);
                       return;
                     }
                    }
                    _skip_int(s);
                    break;
               case PGTIMFMT_MI:    // Minute (0-59).
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
                       ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                       return;
                     }
                    }
                    _skip_int(s);
                    break;
               case PGTIMFMT_FX:
                    fm = 0;
                    break;
               case PGTIMFMT_FM:
                    fm = 1;
                    break;
               case PGTIMFMT_DY:   // Abbreviated name of day.
                    // Not really sure what to do with this.
                    // Generate an error if inconsistent with other data.
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
                       ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, funcname);
                       return;
                     }
                    }
                    break;
               case PGTIMFMT_DDD:  // Day of year (1-366).
                    {
                     char my_fmt[DATE_LEN];
                     short ddd;
   
                     if (fm) {
                       strcpy(my_fmt, "%hd");
                     } else {
                       strcpy(my_fmt, "%03hd");
                     }
                     if (!sscanf(s, my_fmt, &ddd)) {
                       ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                       return;
                     }
                    }
                    _skip_int(s);
                    break;
               case PGTIMFMT_DD:   // Day of month (1-31).
                    {
                     char my_fmt[DATE_LEN];
   
                     if (fm) {
                       strcpy(my_fmt, "%hd");
                     } else {
                       strcpy(my_fmt, "%02hd");
                     }
                     if (!sscanf(s, my_fmt, &(tm.mday))) {
                       ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                       return;
                     }
                    }
                    _skip_int(s);
                    break;
               case PGTIMFMT_DAY:  // Name of day
                    // Not really sure what to do with this.
                    // Generate an error if inconsistent with other data.
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
                       ksu_err_msg(context, KSU_ERR_INV_MONTH, buf, funcname);
                       return;
                     }
                    }
                    break;
               case PGTIMFMT_D:    // Day of week (1-7), 1 = Monday
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
                         ksu_err_msg(context, KSU_ERR_INV_DAY, day, funcname);
                         return;
                       }
                     } else {
                       ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                       return;
                     }
                    }
                    s++;
                    break;
               // BC/AD indicator
               case PGTIMFMT_BC:
               case PGTIMFMT_AD:
                    if (strncasecmp(s, "bc", 2)
                        && strncasecmp(s, "ad", 2)) {
                      ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                      return;
                    }
                    if (strncasecmp(s, "bc", 2) == 0) {
                      bc = 1;
                    }
                    s += 2; 
                    break;
               case PGTIMFMT_B_C_:
               case PGTIMFMT_A_D_:
                    if (strncasecmp(s, "b.c.", 4)
                        && strncasecmp(s, "a.d.", 4)) {
                      ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
                      return;
                    }
                    if (strncasecmp(s, "b.c.", 4) == 0) {
                      bc = 1;
                    }
                    s += 4; 
                    break;
               /*
                *   Ignored / Unsupported
                */
               case PGTIMFMT_MS:
               case PGTIMFMT_US:
                    _skip_int(s);
                    break;
               case PGTIMFMT_TZ:
               case PGTIMFMT_OF:
                    ksu_err_msg(context, KSU_ERR_UNSUP_FORMAT, fmt, funcname);
                    return;
               default:
                    // Other formats are only supported with to_char()
                    ksu_err_msg(context, KSU_ERR_INV_FORMAT, fmt, funcname);
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
         ksu_print_tm(&tm);
         ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
         return;
       }
       // Back now
       if (ksu_localtime(t, &tm)) {
         if (day_check) {
           if (tm.wday != saved_day) { // Flop
             ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
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
         ksu_err_msg(context, KSU_ERR_DATE_CONV, funcname);
         return;
       }
    }
}

extern void pg_to_date(sqlite3_context *context,
                       int              argc,
                       sqlite3_value  **argv) {
    pg_to_date_timestamp(context, argc, argv, "to_date");
}
