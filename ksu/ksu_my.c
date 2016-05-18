#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <ctype.h>
#include <ksu_my.h>
#include <ksu_common.h>
#include <ksu_dates.h>

static char my_interval(const char *str, KSU_INTV_T *intv) {
    /*
     *  Return 0 if not OK, 1 if no time component, 2 if time component
     */
    char *s = (char *)str;
    char *u;
    int   unit;
    float f;
    long  l;
    char  ok = 0;

    if (s && intv) {
      if (strncasecmp(s, "interval", 8) == 0) {
        s += 8;
        while (isspace(*s)) {
          s++;
        }
      } else {
        return (char)0;
      }
      // Look for the unit
      if (*s) {
        if ((u = strrchr(s, ' ')) != (char *)NULL) {
          u++;
          unit = mytim_search(u);
          switch(unit) {
            case MYTIM_MICROSECOND: // Ignored, unless rounding
                                    // to at least one second
                 if (sscanf(s, "%ld", &l) != 1) {
                   return (char)0;
                 }
                 intv->qty[KSU_INTV_SECOND] =
                        (short)((double)l/1000000 + 0.5);
                 ok = 2;
                 break;
            case MYTIM_SECOND:
                 if (sscanf(s, "%hd", &(intv->qty[KSU_INTV_SECOND])) != 1) {
                   return (char)0;
                 }
                 ok = 2;
                 break;
            case MYTIM_MINUTE:
                 if (sscanf(s, "%hd", &(intv->qty[KSU_INTV_MINUTE])) != 1) {
                   return (char)0;
                 }
                 ok = 2;
                 break;
            case MYTIM_HOUR:
                 if (sscanf(s, "%hd", &(intv->qty[KSU_INTV_HOUR])) != 1) {
                   return (char)0;
                 }
                 ok = 2;
                 break;
            case MYTIM_DAY:
                 if (sscanf(s, "%hd", &(intv->qty[KSU_INTV_DAY])) != 1) {
                   return (char)0;
                 }
                 ok = 1;
                 break;
            case MYTIM_WEEK:
                 if (sscanf(s, "%hd", &(intv->qty[KSU_INTV_WEEK])) != 1) {
                   return (char)0;
                 }
                 ok = 1;
                 break;
            case MYTIM_MONTH:
                 if (sscanf(s, "%hd", &(intv->qty[KSU_INTV_MONTH])) != 1) {
                   return (char)0;
                 }
                 ok = 1;
                 break;
            case MYTIM_QUARTER:
                 if (sscanf(s, "%hd", &(intv->qty[KSU_INTV_MONTH])) != 1) {
                   return (char)0;
                 }
                 intv->qty[KSU_INTV_MONTH] *= 3;
                 ok = 1;
                 break;
            case MYTIM_YEAR:
                 if (sscanf(s, "%hd", &(intv->qty[KSU_INTV_YEAR])) != 1) {
                   return (char)0;
                 }
                 ok = 1;
                 break;
            case MYTIM_SECOND_MICROSECOND:
                 if (sscanf(s, "%f", &f) != 1) {
                   return (char)0;
                 }
                 intv->qty[KSU_INTV_SECOND] = (short)(f + 0.5);
                 ok = 2;
                 break;
            case MYTIM_MINUTE_MICROSECOND:
                 if (sscanf(s, "%hd:%f",
                            &(intv->qty[KSU_INTV_MINUTE]), &f) != 2) {
                   return (char)0;
                 }
                 intv->qty[KSU_INTV_SECOND] = (short)(f + 0.5);
                 if (intv->qty[KSU_INTV_MINUTE] < 0) {
                   intv->qty[KSU_INTV_SECOND] *= -1;
                 }
                 ok = 2;
                 break;
            case MYTIM_MINUTE_SECOND:
                 if (sscanf(s, "%hd:%hd",
                           &(intv->qty[KSU_INTV_MINUTE]),
                           &(intv->qty[KSU_INTV_SECOND])) != 2) {
                   return (char)0;
                 }
                 if (intv->qty[KSU_INTV_MINUTE] < 0) {
                   intv->qty[KSU_INTV_SECOND] *= -1;
                 }
                 ok = 2;
                 break;
            case MYTIM_HOUR_MICROSECOND:
                 if (sscanf(s, "%hd:%hd:%f",
                            &(intv->qty[KSU_INTV_HOUR]),
                            &(intv->qty[KSU_INTV_MINUTE]), &f) != 3) {
                   return (char)0;
                 }
                 intv->qty[KSU_INTV_SECOND] = (short)(f + 0.5);
                 if (intv->qty[KSU_INTV_HOUR] < 0) {
                   intv->qty[KSU_INTV_MINUTE] *= -1;
                   intv->qty[KSU_INTV_SECOND] *= -1;
                 }
                 ok = 2;
                 break;
            case MYTIM_HOUR_SECOND:
                 if (sscanf(s, "%hd:%hd:%hd",
                            &(intv->qty[KSU_INTV_HOUR]),
                            &(intv->qty[KSU_INTV_MINUTE]),
                            &(intv->qty[KSU_INTV_SECOND])) != 3) {
                   return (char)0;
                 }
                 if (intv->qty[KSU_INTV_HOUR] < 0) {
                   intv->qty[KSU_INTV_MINUTE] *= -1;
                   intv->qty[KSU_INTV_SECOND] *= -1;
                 }
                 ok = 2;
                 break;
            case MYTIM_HOUR_MINUTE:
                 if (sscanf(s, "%hd:%hd",
                            &(intv->qty[KSU_INTV_HOUR]),
                            &(intv->qty[KSU_INTV_MINUTE])) != 2) {
                   return (char)0;
                 }
                 if (intv->qty[KSU_INTV_HOUR] < 0) {
                   intv->qty[KSU_INTV_MINUTE] *= -1;
                 }
                 ok = 2;
                 break;
            case MYTIM_DAY_MICROSECOND:
                 if (sscanf(s, "%hd %hd:%hd:%f",
                            &(intv->qty[KSU_INTV_DAY]),
                            &(intv->qty[KSU_INTV_HOUR]),
                            &(intv->qty[KSU_INTV_MINUTE]), &f) != 4) {
                   return (char)0;
                 }
                 intv->qty[KSU_INTV_SECOND] = (short)(f + 0.5);
                 if (intv->qty[KSU_INTV_DAY] < 0) {
                   intv->qty[KSU_INTV_HOUR] *= -1;
                   intv->qty[KSU_INTV_MINUTE] *= -1;
                   intv->qty[KSU_INTV_SECOND] *= -1;
                 }
                 ok = 2;
                 break;
            case MYTIM_DAY_SECOND:
                 if (sscanf(s, "%hd %hd:%hd:%hd",
                            &(intv->qty[KSU_INTV_DAY]),
                            &(intv->qty[KSU_INTV_HOUR]),
                            &(intv->qty[KSU_INTV_MINUTE]),
                            &(intv->qty[KSU_INTV_SECOND])) != 4) {
                   return (char)0;
                 }
                 if (intv->qty[KSU_INTV_DAY] < 0) {
                   intv->qty[KSU_INTV_HOUR] *= -1;
                   intv->qty[KSU_INTV_MINUTE] *= -1;
                   intv->qty[KSU_INTV_SECOND] *= -1;
                 }
                 ok = 2;
                 break;
            case MYTIM_DAY_MINUTE:
                 if (sscanf(s, "%hd %hd:%hd",
                            &(intv->qty[KSU_INTV_DAY]),
                            &(intv->qty[KSU_INTV_HOUR]),
                            &(intv->qty[KSU_INTV_MINUTE])) != 3) {
                   return (char)0;
                 }
                 if (intv->qty[KSU_INTV_DAY] < 0) {
                   intv->qty[KSU_INTV_HOUR] *= -1;
                   intv->qty[KSU_INTV_MINUTE] *= -1;
                 }
                 ok = 2;
                 break;
            case MYTIM_DAY_HOUR:
                 if (sscanf(s, "%hd %hd",
                            &(intv->qty[KSU_INTV_DAY]),
                            &(intv->qty[KSU_INTV_HOUR])) != 2) {
                   return (char)0;
                 }
                 if (intv->qty[KSU_INTV_DAY] < 0) {
                   intv->qty[KSU_INTV_HOUR] *= -1;
                 }
                 ok = 2;
                 break;
            case MYTIM_YEAR_MONTH:
                 if (sscanf(s, "%hd-%hd",
                            &(intv->qty[KSU_INTV_YEAR]),
                            &(intv->qty[KSU_INTV_MONTH])) != 2) {
                   return (char)0;
                 }
                 if (intv->qty[KSU_INTV_YEAR] < 0) {
                   intv->qty[KSU_INTV_MONTH] *= -1;
                 }
                 ok = 1;
                 break;
            default:
                 return (char)0; // Not a valid interval unit
          }
          return ok;
        }
      }
    }
    return (char)0;
}

extern void  my_date_op(sqlite3_context * context,
                        char            * fname,
                        char            * date,
                        char              add,
                        char            * interval) {
  KSU_TIME_T t;
  KSU_INTV_T intv;
  KSU_TM_T   mytm;
  KSU_TM_T  *tm;
  int        i;
  long       months = 0;
  long       secs = 0;
  char       result_date[DATE_LEN];
  char       result_datetime = 0;
  char       is_intv;

  if (!ksu_is_datetime((const char *)date, &t, (char)0)) {
    sqlite3_result_null(context);
    return;
  }
  result_datetime = (strlen(date) > 12);
  for (i = 0; i < KSU_INTV_COUNT; i++) {
    intv.qty[i] = 0;
  }
  is_intv = my_interval((const char *)interval, &intv);
  result_datetime = (result_datetime || (is_intv == 2));
  if (is_intv == 0) {
    ksu_err_msg(context, KSU_ERR_ARG_NOT_INTV, fname);
    return;
  }
  result_datetime = (result_datetime || (is_intv == 2));
  months = (long)(intv.qty[KSU_INTV_CENTURY] * 1200
                 + intv.qty[KSU_INTV_YEAR] * 12
                 + intv.qty[KSU_INTV_MONTH]);
  secs = (long)(intv.qty[KSU_INTV_WEEK] * 604800 
               + intv.qty[KSU_INTV_DAY] * 86400
               + intv.qty[KSU_INTV_HOUR] * 3600
               + intv.qty[KSU_INTV_MINUTE] * 60
               + intv.qty[KSU_INTV_SECOND]);
  // printf("%c %ld mon %ld sec\n", (add ? '+' : '-'), months, secs);
  if (!add) {
    months *= -1;
    secs *= -1;
  }
  if (months) {
    if ((tm = ksu_localtime(t, &mytm)) != (KSU_TM_T *)NULL) {
      t = ksu_add_months(tm, months);
      tm = ksu_localtime(t, &mytm);
      if (result_datetime) {
        if (tm) {
          sprintf(result_date, "%04hd-%02hd-%02hd %02hd:%02hd:%02hd",
                  tm->year, (short)(1 + tm->mon), tm->mday,
                  tm->hour, tm->min, tm->sec);
        } else {
          strcpy(result_date, "0000-00-00 00:00:00");
        }
      } else {
        if (tm) {
          sprintf(result_date, "%04hd-%02hd-%02hd",
                  tm->year, (short)(1 + tm->mon), tm->mday);
        } else {
          strcpy(result_date, "0000-00-00");
        }
      }
    } else {
      strcpy(result_date, (result_datetime ?
                           "0000-00-00 00:00:00"
                           : "0000-00-00"));
    }
    sqlite3_result_text(context, result_date, -1, SQLITE_TRANSIENT);
    return;
  }
  if (secs) {
    t = ksu_add_secs(t, secs);
    tm = ksu_localtime(t, &mytm);
    if (tm) {
      sprintf(result_date, "%04hd-%02hd-%02hd %02hd:%02hd:%02hd",
              tm->year, (short)(1 + tm->mon), tm->mday,
              tm->hour, tm->min, tm->sec);
    } else {
      strcpy(result_date, "0000-00-00 00:00:00");
    }
    sqlite3_result_text(context, result_date, -1, SQLITE_TRANSIENT);
    return;
  }
  // If we reach this, nothing was changed
  tm = ksu_localtime(t, &mytm);
  if (tm) {
    if (result_datetime) {
      sprintf(result_date, "%04hd-%02hd-%02hd %02hd:%02hd:%02hd",
              tm->year, (short)(1 + tm->mon), tm->mday,
              tm->hour, tm->min, tm->sec);
    } else {
      sprintf(result_date, "%04hd-%02hd-%02hd",
              tm->year, (short)(1 + tm->mon), tm->mday);
    }
  } else {
    strcpy(result_date, (result_datetime ?
                         "0000-00-00 00:00:00"
                         : "0000-00-00"));
  }
  sqlite3_result_text(context, result_date, -1, SQLITE_TRANSIENT);
}

extern void my_time_op(sqlite3_context * context,
                       char            * fname,
                       char              add,
                       char            * expr1,
                       char            * expr2) {
  // expr1 is a time or datetime expression,
  // and expr2 is a time expression. 
  KSU_TIME_T t1;
  KSU_TIME_T t2;
  KSU_TIME_T t;
  char       result_date[DATE_LEN];
  char      *d;

  t.jdn = 0;
  t.sec = 0;
  if (!ksu_is_datetime((const char *)expr1, &t1, 1)
      && !ksu_is_time((const char *)expr1, &t1, 0)) {
    ksu_err_msg(context, KSU_ERR_INV_TIME_DATETIME, expr1, fname);
    return;
  }
  t.flags = t1.flags;
  if (!ksu_is_time((const char *)expr2, &t2, 0)) {
    ksu_err_msg(context, KSU_ERR_INV_TIME, expr2, fname);
    return;
  }
  if (add) {
    t.sec = t1.sec + t2.sec;
    if (t.sec >= 86400) {
      t.jdn = (t.sec / 86400);
      t.sec %= 86400;
    }
    t.jdn += (t1.jdn + t2.jdn);
  } else {
    t.sec = t1.sec - t2.sec;
    if (t.sec <= -86400) {
      t.jdn = (t.sec / 86400);
      t.sec = -1 * ((-1 % t.sec) % 86400);
    }
    t.jdn += (t1.jdn - t2.jdn);
  }
  if (t.jdn && (t.flags & KSU_TIME_TIMEONLY)) {
    t.flags &= ~KSU_TIME_TIMEONLY;
    t.flags |= KSU_TIME_XTIME;
  }
  if ((d = ksu_datetime(t, result_date)) != (char *)NULL) {
    sqlite3_result_text(context, result_date, -1, SQLITE_TRANSIENT);
  } else {
    sqlite3_result_null(context);
  }
}

// MySQL is pretty lax when an integer parameter is expected.
// Float values are rounded to the nearest integer, and char
// values sometimes magically transmogrify into an integer
// value. The following functions do it (def is what to return
// when val isn't numerical)
extern int  my_value_int(sqlite3_value *val, int def) {
  int typ;

  if (val) {
    typ = sqlite3_value_type(val);
    if (typ == SQLITE_INTEGER) {
      return sqlite3_value_int(val);
    } else if (typ == SQLITE_FLOAT) {
      return (int)(0.5 + sqlite3_value_double(val));
    }
  }
  return def;
}

extern sqlite3_int64 my_value_int64(sqlite3_value *val, int def) {
  int typ;

  if (val) {
    typ = sqlite3_value_type(val);
    if (typ == SQLITE_INTEGER) {
      return sqlite3_value_int64(val);
    } else if (typ == SQLITE_FLOAT) {
      return (sqlite3_int64)(0.5 + sqlite3_value_double(val));
    }
  }
  return def;
}

extern double my_value_double(sqlite3_value *val, double def) {
  int typ;

  if (val) {
    typ = sqlite3_value_type(val);
    if ((typ == SQLITE_INTEGER) || (typ == SQLITE_FLOAT)) {
      return sqlite3_value_double(val);
    }
  }
  return def;
}

static void long_to_datetime(long long numval, KSU_TM_T *tm) {
    (void)memset(tm, 0, sizeof(KSU_TM_T));
    tm->sec = (short)(numval % 100);
    tm->min = (short)((numval / 100) % 100);
    tm->hour = (short)((numval / 10000) % 100);
    tm->mday = (short)((numval / 1000000) % 100);
    tm->mon = (short)((numval / 100000000) % 100 - 1);
    tm->year = (short)((numval / 10000000000));
    if (tm->year == 0) {
      tm->year = 2000;  // Seems to be the default for MySQL
    }
    if ((tm->year > 0) && (tm->year < 100)) {
      if (tm->year >= 70) {
        tm->year += 1900;
      } else {
        tm->year += 2000;
      }
    }
}

static void long_to_date(long long numval, KSU_TM_T *tm) {
    (void)memset(tm, 0, sizeof(KSU_TM_T));
    tm->mday = (short)(numval % 100);
    tm->mon = (short)(((numval / 100) % 100) - 1);
    tm->year = (short)(numval / 10000);
    if (tm->year == 0) {
      tm->year = 2000;  // Seems to be the default for MySQL
    }
    if ((tm->year > 0) && (tm->year < 100)) {
      if (tm->year >= 70) {
        tm->year += 1900;
      } else {
        tm->year += 2000;
      }
    }
}

extern char my_int_datetime(sqlite3_value *val, KSU_TM_T *tm) {
    sqlite3_int64  numval;
    char           date_ok = 1;
    int            bytes;

    if (!val || !tm) {
      return (char)0;
    }
    if ((sqlite3_value_type(val) != SQLITE_INTEGER)
       && (sqlite3_value_type(val) != SQLITE_FLOAT)) {
      numval = (sqlite3_int64)0;
    } else {
      numval = sqlite3_value_int64(val);
      bytes = sqlite3_value_bytes(val);
      if (bytes <= 8) {
        numval *= 1000000;
      } 
    }
    long_to_datetime((long long)numval, tm);
    if ((tm->sec > 59)
        || (tm->min > 59)
        || (tm->hour > 23)
        || (tm->mday > 31)
        || (tm->mday < 1)
        || (tm->mon > 11)
        || (tm->mon < 0)
        || (tm->year < 1)
        || (tm->year > 9999)) {
      date_ok = 0;
    }
    return date_ok;
}

extern char my_int_date(sqlite3_value *val, KSU_TM_T *tm) {
    sqlite3_int64  numval;
    char           date_ok = 1;

    if (!val || !tm) {
      return (char)0;
    }
    if ((sqlite3_value_type(val) != SQLITE_INTEGER)
       && (sqlite3_value_type(val) != SQLITE_FLOAT)) {
      numval = (sqlite3_int64)0;
    } else {
      numval = sqlite3_value_int64(val);
    }
    long_to_date((long long)numval, tm);
    if ((tm->mday > 31)
        || (tm->mday < 1)
        || (tm->mon > 11)
        || (tm->mon < 0)
        || (tm->year < 1)
        || (tm->year > 9999)) {
      date_ok = 0;
    }
    return date_ok;
}

/*
 *  Laxer functions than the ksu_ ones
 */
extern char my_is_date(const char *dt, KSU_TIME_T *tptr) {
   char      ok = 0;
   char     *d = (char *)dt;
   long long numval;
   KSU_TM_T  tm;

   if (dt && tptr) {
     ok = ksu_is_date(dt, tptr);
     if ((ok == 0) && isdigit(*d) && (strlen(d) >= 3)) {
       // Check we only have digits
       while (isdigit(*d)) {
         d++;
       }
       if (*d) {
          return (char)0;
       }
       // Try harder
       // Wants at least a month and a day.
       // If no year, 2000 is assumed
       // If two-digit year, 21st century assumed up to 69 included,
       // 20th century from 70 up.
       (void)sscanf((char *)dt, "%lld", &numval);
       long_to_date((long long)numval, &tm);
       if ((tm.mday > 31)
           || (tm.mday < 1)
           || (tm.mon > 11)
           || (tm.mon < 0)
           || (tm.year < 1)
           || (tm.year > 9999)) {
         ok = 0;
       } else {
         ok = 1;
         *tptr = ksu_mktime(&tm);
       }
     }
   }
   return ok;
}

extern char my_is_datetime(const char *dt,
                           KSU_TIME_T *tptr,
                           char        strict) {
   char       ok = 0;
   long long  numval;
   char      *d = (char *)dt;
   KSU_TM_T   tm;

   ok = ksu_is_datetime(dt, tptr, strict);
   if (ok == 0) {
     // Try harder
     // Check we only have digits
     while (isdigit(*d)) {
       d++;
     }
     if (*d) {
       return (char)0;
     }
     // If two-digit year, 21st century assumed up to 69 included,
     // 20th century from 70 up.
     (void)sscanf((char *)dt, "%lld", &numval);
     if (strlen(d) <= 8) {
       numval *= 1000000;
     }
     long_to_datetime((long long)numval, &tm);
     if ((tm.sec > 59)
         || (tm.min > 59)
         || (tm.hour > 23)
         || (tm.mday > 31)
         || (tm.mday < 1)
         || (tm.mon > 11)
         || (tm.mon < 0)
         || (tm.year < 1)
         || (tm.year > 9999)) {
       ok = 0;
     } else {
       ok = 1;
       *tptr = ksu_mktime(&tm);
     }
   }
   return ok;
}

extern char my_is_interval(const char *s, KSU_INTV_T *intv) {
    return (my_interval(s, intv) ? (char)1 : (char)0);
}


static char *G_mytim_words[] = {
    "DAY",
    "DAY_HOUR",
    "DAY_MICROSECOND",
    "DAY_MINUTE",
    "DAY_SECOND",
    "HOUR",
    "HOUR_MICROSECOND",
    "HOUR_MINUTE",
    "HOUR_SECOND",
    "MICROSECOND",
    "MINUTE",
    "MINUTE_MICROSECOND",
    "MINUTE_SECOND",
    "MONTH",
    "QUARTER",
    "SECOND",
    "SECOND_MICROSECOND",
    "WEEK",
    "YEAR",
    "YEAR_MONTH",
    NULL};

extern int mytim_search(char *w) {
  int start = 0;
  int end = MYTIM_COUNT - 1;
  int mid;
  int pos = MYTIM_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_mytim_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
      } else if ((mid < MYTIM_COUNT)
                 && ((comp = strcasecmp(G_mytim_words[mid+1], w)) == 0)) {
         pos = mid+1;
         start = end + 1;
      } else {
        if (comp < 0) {
           start = mid + 1;
        } else {
           end = mid - 1;
        }
      }
    }
  }
  return pos;
}

extern char *mytim_keyword(int code) {
  if ((code >= 0) && (code < MYTIM_COUNT)) {
    return G_mytim_words[code];
  } else {
    return (char *)NULL;
  }
}

