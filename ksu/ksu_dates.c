#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <ksu_common.h>
#include <ksu_dates.h>
#include <langinfo.h>  // For getting month/day names

// Remarkable Julian day numbers
// ----------------------------
// It was decreed by Pope Gregory XIII that the day following 4 October 1582
// (which is 5 October 1582, in the Julian Calendar) would thenceforth be known
// as 15 October 1582. Oracle implements this gap, other DBMS products
// don't (on the ground, it was implemented by very few countries at this
//  date, see Wikipedia)
//  
// There never was any year 0, the first value
// corresponds to what would have been Jan 1st, year 0 
// and the last one what should have been Dec 31st, year 0
// Will be skipped in the same fashion as the 1582 gap. 
//
#ifdef ORA_FUNC

#define YEAR1_START        1721424
#define YEAR_MINUS_1_END   1721057

#endif

#ifdef PG_FUNC

#define YEAR1_START        1721426
#define YEAR_MINUS_1_END   1721059

#endif

#ifndef YEAR1_START

#define YEAR1_START              0
#define YEAR_MINUS_1_END         0

#endif

#define JAN_1ST_YEAR1      1721426
#define BELLE_EPOQUE_JDN   2415021   // Jan 1st, 1900
#define EPOCH_JDN          2440588   // Jan 1st, 1970

#define GREG_START         2299160   // Start of Gregorian calendar,
                                     // if the 1582 gap is taken into account

// DBMS-dependent date limits
//
// Oracle:     4712BC to 9999-12-31
#ifdef ORA_FUNC

#define MAX_JDN   5373484
#define MIN_JDN         1
#define GAP_1582        1

#endif
// MySQL:      1000-01-01 to 9999-12-31
#ifdef MY_FUNC

#define MAX_JDN   5373484
#define MIN_JDN   2086303
#define GAP_1582        0

#endif
// SQL Server: 1753-01-01 to 9999-12-31
//             (datetime2 starts at 0001-01-01)
#ifdef SS_FUNC

#define MAX_JDN   5373484
#define MIN_JDN   2361331
#define GAP_1582        0

#endif
// PostgreSQL: 4713 BC to 294276 AD (twice this maximum for the DATE type)
// Restricted to the same range as Oracle in our functions
#ifdef PG_FUNC

#define MAX_JDN   5373484
#define MIN_JDN         0
#define GAP_1582        0

#endif
#ifdef PG_FUNC
#include <ksu_pg.h>
#endif


// Values are defined as global variables
// so as to be able to override them when testing

static long long G_min_julian = MIN_JDN;
static long long G_max_julian = MAX_JDN;
static char      G_1582_gap = GAP_1582;

// The following tables are for getting names
typedef struct {
         nl_item  full;
         nl_item  abbr;
        } NL_NAME_T;

static NL_NAME_T G_month_name[12] =
       {{MON_1, ABMON_1}, {MON_2, ABMON_2}, {MON_3, ABMON_3},
        {MON_4, ABMON_4}, {MON_5, ABMON_5}, {MON_6, ABMON_6},
        {MON_7, ABMON_7}, {MON_8, ABMON_8}, {MON_9, ABMON_9},
        {MON_10, ABMON_10}, {MON_11, ABMON_11}, {MON_12, ABMON_12}};

static NL_NAME_T G_day_name[7] =
       {{DAY_1, ABDAY_1},
        {DAY_2, ABDAY_2},
        {DAY_3, ABDAY_3},
        {DAY_4, ABDAY_4},
        {DAY_5, ABDAY_5},
        {DAY_6, ABDAY_6},
        {DAY_7, ABDAY_7}};
//
// CALDATE_T is only used internally by static functions.
// KSU_TM_T (modelled after struct tm, minus DST and timezones
// and with some usage differences) is the "official" type to
// use to break a date into its components;
typedef struct {
      short y;
      short m;
      short d;
     } CALDATE_T;

// Two following functions based on algorithms at
// https://alcor.concordia.ca/~gpkatch/gdate-algorithm.html
//
// They are only used for date validation.

static long long datenum(short y, short m, short d) {
  m = (m + 9) % 12;
  y -=  m/10;
  return (long long)(365*y + y/4 - y/100 + y/400
                     + (m*306 + 5)/10 + (d - 1));
}

static CALDATE_T numdate(long long g) {
  CALDATE_T  cd;
  long       mi; 
  long       ddd; 

  cd.y = (short)((10000*g + 14780)/3652425);
  ddd = (long)(g - (365*cd.y + cd.y/4 - cd.y/100 + cd.y/400));
  if (ddd < 0) {
    (cd.y)--;
    ddd = (long)(g - (365*cd.y + cd.y/4 - cd.y/100 + cd.y/400));
  }
  mi = (100*ddd + 52)/3060;
  cd.m = (short)((mi + 2)%12 + 1);
  cd.y += (short)(mi + 2)/12;
  cd.d = (short)(ddd - (mi*306 + 5)/10 + 1);
  return cd;
}

/*
 *  Compute the maximum length of a month or day
 *  name (language dependent)
 */
static int G_max_monthname = 0;
static int G_max_dayname = 0;
static int G_max_monname = 0;
static int G_max_dyname = 0;

extern int ksu_maxmonth(void) {
  int i;
  int len;

  if (G_max_monthname == 0) {
    ksu_i18n();
    for (i = 0; i < 12; i++) {
      len = strlen(nl_langinfo(G_month_name[i].full));
      if (len > G_max_monthname) {
        G_max_monthname = len;
      }
    }
  }
  return G_max_monthname;
}

extern int ksu_maxmon(void) {
  int i;
  int len;

  if (G_max_monname == 0) {
    ksu_i18n();
    for (i = 0; i < 12; i++) {
      len = strlen(nl_langinfo(G_month_name[i].abbr));
      if (len > G_max_monname) {
        G_max_monname = len;
      }
    }
  }
  return G_max_monname;
}

extern int ksu_maxday(void) {
  int i;
  int len;

  if (G_max_dayname == 0) {
    ksu_i18n();
    for (i = 0; i < 7; i++) {
      len = strlen(nl_langinfo(G_day_name[i].full));
      if (len > G_max_dayname) {
        G_max_dayname = len;
      }
    }
  }
  return G_max_dayname;
}

extern int ksu_maxdy(void) {
  int i;
  int len;

  if (G_max_dyname == 0) {
    ksu_i18n();
    for (i = 0; i < 7; i++) {
      len = strlen(nl_langinfo(G_day_name[i].abbr));
      if (len > G_max_dyname) {
        G_max_dyname = len;
      }
    }
  }
  return G_max_dyname;
}

extern int ksu_month_by_abbr(char *abbr) {
  int ret = -1;
  int i = 0;

  if (abbr) {
    ksu_i18n();
    // 0 = January - same as in struct tm
    while((i < 12) && strcasecmp(abbr, nl_langinfo(G_month_name[i].abbr))) {
      i++;
    }
    ret = (i == 12 ? -1 : i);
  }
  return ret;
}

extern int ksu_month_by_name(char *name) {
  int ret = -1;
  int i = 0;

  if (name) {
    ksu_i18n();
    // 0 = January - same as in struct tm
    while((i < 12) && strcasecmp(name, nl_langinfo(G_month_name[i].full))) {
      i++;
    }
    ret = (i == 12 ? -1 : i);
  }
  return ret;
}

extern int ksu_day_by_abbr(char *abbr) {
  int ret = -1;
  int i = 0;

  if (abbr) {
    ksu_i18n();
    // 0 = Sunday, 6 = Saturday - same as in struct tm
    while((i < 7) && strcasecmp(abbr, nl_langinfo(G_day_name[i].abbr))) {
      i++;
    }
    ret = (i == 7 ? -1 : i);
  }
  return ret;
}

extern int ksu_day_by_name(char *name) {
  int ret = -1;
  int i = 0;

  if (name) {
    ksu_i18n();
    // 0 = Sunday, 6 = Saturday - same as in struct tm
    while((i < 7) && strcasecmp(name, nl_langinfo(G_day_name[i].full))) {
      i++;
    }
    ret = (i == 7 ? -1 : i);
  }
  return ret;
}

extern int ksu_weekday(char *dayname) {
  int i = -1;

  if (dayname) {
    ksu_i18n();
    i = 0;
    while ((i < 7)
           && strncasecmp(dayname,
                          nl_langinfo(G_day_name[i].abbr),
                          strlen(nl_langinfo(G_day_name[i].abbr)))) {
      i++;
    }
  }
  return (i == 7 ? -1 : i);
}

static short isodow(long long jdn) {
    if ((jdn > G_max_julian) || (jdn < G_min_julian)) {
      return -1;
    }
    return (short)(1 + jdn % 7);
}

static short dow(long long jdn) {
    if ((jdn > G_max_julian) || (jdn < G_min_julian)) {
      return -1;
    }
    return (short)((jdn + 1) % 7);
}

// Julian date info found at various places on the web
// This is the Fliegel and van Flandern formula
static long long julian_day_num(short y, short m, short d) {
    long long jdn = 0;
    
    if (y == 0) {
      return (long long)-1;
    }
    jdn = d-32075+1461*(y+4800+(m-14)/12)/4
         +367*(m-2-(m-14)/12*12)/12
         -3*((y+4900+(m-14)/12)/100)/4;
#ifdef ORA_FUNC
    if ((y < 1582)
        || ((y == 1582) && (m < 10))
        || ((y == 1582) && (m == 10) && (d < 15))) {
      // Indicate the date as invalid if it falls in
      // the 1582 gap
      if ((y == 1582) && (m == 10) && (d < 15) && (d > 4)) {
        return (long long)-1;
      }
      // Switch to the Julian calendar
      // Correction: 
      //    1) Add 10 days
      //    2) Remove one day (make it a leap year) to
      //       every year multiple of 100 that isn't
      //       a multiple of 400 and would not be a
      //       Gregorian leap year 
      jdn += 10;
      // Source: www.tondering.dk
      // The Julian calendar has 1 leap year every 4 years:
      // Every year divisible by 4 is a leap year.
      // However, the 4-year rule was not followed in the first years after
      // the introduction of the Julian calendar in 45 BC. Due to a counting
      // error, every 3rd year was a leap year in the first years of this
      // calendar’s existence. The leap years were:
      // 45 BC, 42 BC, 39 BC, 36 BC, 33 BC, 30 BC, 27 BC, 24 BC, 21 BC,
      // 18 BC, 15 BC, 12 BC, 9 BC, AD 8, AD 12, and every 4th year from
      // then on.
      // Authorities disagree about whether 45 BC was a leap year or not.
      // This program happily ignores all this.
      int i;
      for (i = (y/100); i < 15; i++) {
        if (i % 4) {
          jdn--;
        } 
      }
      // Correction if current year is a gap year but the date is 
      // no later than Feb 28th
      if (((y % 100) == 0)
          && (y % 4)) {
        if ((m < 2) || ((m == 2) && (d <= 28))) {
          jdn++;
        }
      }
      // Additional correction: 1,000 is a common year for the Gregorian
      // calendar, a leap year for the Julian one
      if ((y < 1000)
          || ((y == 1000) && (m < 2))
          || ((y == 1000) && (m == 2) && (d <= 28))) {
        jdn--;
      }
      // Final correction: there was no year 0
      if (y < 0) {
        jdn += (YEAR1_START - YEAR_MINUS_1_END - 1);
      }
    }
#endif
#ifdef PG_FUNC
    // PostgreSQL uses a "proleptic" calendar, in other words
    // applies Gregorian correction backs in time.
    // It ignores the missing days of 1582 (which were missing
    // at this date in some countries only).
    // It handles correctly (and better than Oracle) the absence of
    // any year 0.
    if (y < 0) {
      jdn += (YEAR1_START - YEAR_MINUS_1_END - 1);
    }
#endif
    return jdn;
}

static CALDATE_T jdn_to_greg(long long jdn) {
    // Convert Julian Day number to Gregorian date.
    CALDATE_T     cd;
    long long     a;
    long long     b;
    long long     c;
    long long     d;
    long long     e;
    long long     m;

    // Formula source: www.tondering.dk
    if (!G_1582_gap || (jdn >= GREG_START)) {
      a = jdn + 32044;
      b = (4 * a + 3) / 146087;
      c = a - (146097 * b) / 4;
    } else {
      b = 0;
      c = jdn + 32082;
    }
    d = (4 * c + 3) / 1461;
    e = c - (1461 * d) / 4;
    m = (5 * e + 2) / 153;
    cd.d = (short)(e - (153 * m + 2) / 5 + 1);
    cd.m = (short)(m + 3 - 12 * (m / 10));
    cd.y = (short)(100 * b + d - 4800 + (m / 10));
    return cd;
}

extern KSU_TIME_T ksu_mktime(KSU_TM_T *tm) {
    KSU_TIME_T jd = KSU_TIME_INIT;

    if (tm && tm->year) {
      if (tm->year < 0) {
        jd.flags |= KSU_TIME_BC;
      }
      jd.jdn = julian_day_num(tm->year, 1 + tm->mon, tm->mday);
      jd.sec = (tm->hour)*3600
               + (tm->min)*60
               + tm->sec;
    }
    return jd;
}

extern KSU_TM_T  *ksu_localtime(KSU_TIME_T t, KSU_TM_T *tmptr) {
  // Populates the KSU_TM_T the address of which is passed
  // and returns the pointer in case of success, NULL in
  // case of failure.
  CALDATE_T cd;

  if (tmptr
      && !(t.flags & KSU_TIME_TIMEONLY || t.flags & KSU_TIME_XTIME)) {
    if (!(t.flags & KSU_TIME_TIMEONLY)
        && !(t.flags & KSU_TIME_XTIME)) {
      cd = jdn_to_greg(t.jdn);
      tmptr->year = cd.y;
      tmptr->mon = cd.m - 1;
      tmptr->mday = cd.d;
      tmptr->wday = dow(t.jdn);
    }
    tmptr->hour = t.sec / 3600;
    tmptr->min = (t.sec - (tmptr->hour * 3600)) / 60;
    tmptr->sec = t.sec % 60;
  }
  return tmptr;
}

extern char *ksu_datetime(KSU_TIME_T t, char *buff) {
   // buff assumed to be DATE_LEN long
   KSU_TM_T  tm;
   KSU_TM_T *tptr;

   if (buff) {
#ifdef PG_FUNC
     if (t.flags & KSU_TIME_INFINITE) {
       if (t.flags & KSU_TIME_BC) {
         strncpy(buff, "-infinite", DATE_LEN);
       } else {
         strncpy(buff, "infinite", DATE_LEN);
       }
       return buff;
     }
#endif
     if ((tptr = ksu_localtime(t, &tm)) != (KSU_TM_T *)NULL) {
       if (t.flags & KSU_TIME_TIMEONLY) {
         sprintf(buff, "%02d:%02d:%02d",
                 tptr->hour,
                 tptr->min,
                 tptr->sec);
       } else if (t.flags & KSU_TIME_XTIME) {
         sprintf(buff, "%lld %02d:%02d:%02d",
                 t.jdn,
                 tptr->hour,
                 tptr->min,
                 tptr->sec);
       } else {
         sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d",
                 tptr->year,
                 tptr->mon + 1,
                 tptr->mday,
                 tptr->hour,
                 tptr->min,
                 tptr->sec);
       }
     }
   }
   return buff;
}

extern KSU_TIME_T ksu_currenttime(char dateonly) {
  KSU_TIME_T val = KSU_TIME_INIT;
  time_t     now;
  struct tm  mytm;
  struct tm *t;

  now = time(NULL);
  if ((t = localtime_r(&now, &mytm)) != (struct tm *)NULL) {
    val.jdn = julian_day_num((short)(t->tm_year + 1900),
                  (short)(t->tm_mon + 1),
                  (short)(t->tm_mday));
    if (!dateonly) {
      val.sec = ((t->tm_hour * 3600)
                 + (t->tm_min * 60)
                 + t->tm_sec);
    }
  }
  return val; 
}

extern KSU_TIME_T ksu_currentutc(char dateonly) {
  KSU_TIME_T val = KSU_TIME_INIT;
  time_t     now;
  struct tm  mytm;
  struct tm *t;

  now = time(NULL);
  if ((t = gmtime_r(&now, &mytm)) != (struct tm *)NULL) {
    val.jdn = julian_day_num((short)(t->tm_year + 1900),
                  (short)(t->tm_mon + 1),
                  (short)(t->tm_mday));
    if (!dateonly) {
      val.sec = ((t->tm_hour * 3600)
                 + (t->tm_min * 60)
                 + t->tm_sec);
    }
  }
  return val; 
}

static char is_caldate(short y, short m, short d) {
    CALDATE_T cd1;
    CALDATE_T cd2;
    long long dn;

    if (y == 0) {
      return (char)0;
    }
    cd1.y = y;
    cd1.m = m;
    cd1.d = d;
    dn = datenum(y,m,d);
    if (G_1582_gap && (dn > 578030) && (dn < 578041)) {
      return (char)0;
    }
    cd2 = numdate(dn);
    return (char)(memcmp(&cd1, &cd2, sizeof(CALDATE_T)) == 0);
}

static KSU_TIME_T mkksutime(const char *dt) {
  KSU_TIME_T val = KSU_TIME_INIT;
  int        pieces;

  if (dt) {
     KSU_TM_T  tm;
     char     *s = (char *)dt;

#ifdef PG_FUNC
     switch(pgspecial_search((char *)dt)) {
       case PGSPECIAL_PLUSINFINITY:
       case PGSPECIAL_INFINITY:
            val.flags |= KSU_TIME_INFINITE;
            return val;
       case PGSPECIAL_MINUSINFINITY:
            val.flags |= (KSU_TIME_INFINITE | KSU_TIME_BC);
            return val;
       case PGSPECIAL_EPOCH:
            val.jdn = EPOCH_JDN;
            return val;
       case PGSPECIAL_NOW:
            val = ksu_currenttime(0);
            val.flags |= KSU_TIME_DATETIME;
            return val;
       case PGSPECIAL_TODAY:
            val = ksu_currenttime(1);
            return val;
       case PGSPECIAL_TOMORROW:
            val = ksu_currenttime(1);
            val.jdn++;
            return val;
       case PGSPECIAL_YESTERDAY:
            val = ksu_currenttime(1);
            val.jdn--;
            return val;
       default:
            break;
     }
#endif
#ifdef SS_FUNC
     // SQL Server interprets 0 as Jan 1st, 1900
     if (strcmp(s, "0") == 0) {
       val.jdn = BELLE_EPOQUE_JDN;
       return val;
     }
#endif
     tm.hour = 0;
     tm.min = 0;
     tm.sec = 0;
     if (*s == '-') {
       s++;
     }
     if ((pieces = sscanf(s,
                          "%4hd-%02hd-%02hd %02hd:%02hd:%02hd",
                          &tm.year, &tm.mon, &tm.mday,
                          &tm.hour, &tm.min, &tm.sec)) >= 3) {

       if (is_caldate(tm.year, tm.mon, tm.mday)
           && ((tm.hour>=0) && (tm.hour<24))
           && ((tm.min>=0) && (tm.min<60))
           && ((tm.sec>=0) && (tm.sec<60))) {
         tm.mon--;
         if (s != dt) {
           val.flags |= KSU_TIME_BC;
           tm.year *= -1;
         }
         val = ksu_mktime(&tm);
         if ((val.jdn != -1) && (pieces == 6)) {
           val.flags |= KSU_TIME_DATETIME;
         }
       }
     }
  }
  return val;
}

extern KSU_TIME_T ksu_add_secs(KSU_TIME_T t, int secs) {
    char      sub = 0;
    long long start_jdn;

    if ((t.flags & KSU_TIME_INFINITE) || (t.jdn == -1)) {
      return t;
    }
    start_jdn = t.jdn;
    if (secs > 0) {
      t.jdn += (long long)((int)t.sec + secs)/86400; 
      t.sec = (unsigned long)((int)t.sec + secs)%86400; 
    } else {
      sub = 1;
      secs *= -1;
      if (secs > t.sec) {
        t.jdn -= ((long long)(secs - (int)t.sec)/86400 + 1);
        if (t.sec) {
          t.sec = (86400 - (secs - t.sec)%86400); 
        } else {
          if (secs % 86400) {
            t.sec = 86400 - secs%86400; 
          }
        }
      } else {
        t.sec -= secs;
      }
    }
    // Check that it doesn't take us outside valid ranges
    if ((t.jdn < G_min_julian) 
         || (t.jdn > G_max_julian)) {
      t.jdn = -1;
    }
    return t;
}

//-- ISO week number
// Very loosely based on info found at
// http://www.boyet.com/articles/publishedarticles/calculatingtheisoweeknumb.html
// Plus Wikipedia.
static long long isoweekone(short year) {
   long long jdn = 0;
   short     mon_offset; // Offset to Monday

   // Find the Julian Day Number for Jan 4th.
   jdn = julian_day_num(year, (short)1, (short)4);
   mon_offset = jdn % 7;
   return (jdn - mon_offset); 
}

extern char *ksu_iso_date(KSU_TIME_T t, char *isodate) {
   char      *d = isodate;
   KSU_TM_T  *tm;
   KSU_TM_T   mytm;
   long long  week1_jdn;
   short      iso_year;
   short      iso_week = 0;

   if (d) {
     if ((tm = ksu_localtime(t, &mytm)) != (KSU_TM_T *)NULL) {
       iso_year = tm->year;
       // The difficult period is between Dec 29th
       // and Jan 3rd included: the iso year may be
       // different from the official year.
       if ((tm->mon == 11) && (tm->mday >= 29)) {
         week1_jdn = isoweekone((iso_year == -1) ? 1 : iso_year + 1);
         if (t.jdn >= week1_jdn) {
           iso_year += ((iso_year == -1) ? 2 : 1);
           iso_week = 1;
         }
       } else if ((tm->mon == 0) && (tm->mday < 4)) {
         week1_jdn = isoweekone(iso_year);
         if (t.jdn < week1_jdn) {
           iso_year -= ((iso_year == 1) ? 2 : 1);
           iso_week = 53;
         } else {
           iso_week = 1;
         }
       }
       if (!iso_week) {
         week1_jdn = isoweekone(iso_year);
         iso_week = (t.jdn - week1_jdn) / 7 + 1;
       }
       sprintf(isodate, "%hd-W%02hd-%d",
                       iso_year,
                       iso_week,
                       isodow(t.jdn));
     } else {
       return (char *)NULL;
     }
   }
   return d;
}

extern char ksu_long_iso_year(int y) {
  char        is_long = 0;
  KSU_TM_T    tm;
  KSU_TM_T   *ptm;
  KSU_TIME_T  t = KSU_TIME_INIT;

  // An ISO long year (53 weeks instead of 52) is either
  // any year starting on Thursday or any leap year starting
  // on Wednesday
  // Note that we ignore all the Julian/Gregorian/Year 0
  // subtleties here.
  (void)memset(&tm, 0, sizeof(KSU_TM_T));
  tm.year = y;
  tm.mon = 0;
  tm.mday = 1;
  t = ksu_mktime(&tm);
  if ((ptm = ksu_localtime(t, &tm)) != (KSU_TM_T *)NULL) {
    if ((y & 3) == 0 && ((y % 25) != 0 || (y & 15) == 0)) {
      /* leap year */
      if (ptm->wday == 3) {
        is_long = 1;
      }
    }
    if (!is_long) {
      if (ptm->wday == 4) {
        is_long = 1;
      }
    }
  }
  return is_long;
}

extern short ksu_iso_week(KSU_TIME_T t) {
  char   isodate[DATE_LEN];
  char  *p;
  short  w;

  if (t.flags & KSU_TIME_INFINITE) {
    return 0;
  }
  if (ksu_iso_date(t, isodate)) {
    // First character skipped in case of negative year
    if (((p = strchr(&(isodate[1]), '-')) != NULL)
        && (*(p+1) == 'W')) {
      p += 2;
      if (sscanf(p, "%hd", &w)) {
        return w;
      }
    }
  }
  return (short)0;
}

extern KSU_TIME_T ksu_add_months(KSU_TM_T *d, int n) {
  // The general idea is that when you add n months you expect
  // the result month to be the current month plus n, modulo
  // 12, and the number of the day in the month to be the same
  // as the current one. However, it won't necessarily work for
  // a day after the 28th, depending on the month. So days are
  // adjusted to indeed land on the target month but on a valid
  // date;
  KSU_TIME_T val = KSU_TIME_INIT;
  KSU_TM_T   target;
  char       sub = 0;

  if (d) {
    (void)memcpy(&target, d, sizeof(KSU_TM_T));
    if (n < 0) {
      n *= -1;
      sub = 1;
    }
    // Loops aren't the most efficient way to perform
    // computations but we generally don't expect people
    // to add or subtract decades and it makes easier handling
    // year 0 (granted, unlikely to be a frequent occurrence)
    if (!sub) {
      while ((d->mon + n) > 11) {
        target.year += (target.year == -1 ? 2 : 1);
        n -= 12;
      }
      target.mon = d->mon + n;
    } else {
      while ((d->mon - n) < 0) {
        target.year -= (target.year == 1 ? 2 : 1);
        n -= 12;
      }
      target.mon = d->mon - n;
    }
    if (G_1582_gap
        && (target.year == 1582)
        && (target.mon == 9)
        && ((target.mday > 4) && (target.mday < 15))) {
      // Aim for the closest interval boundary
      if (target.mday < 10) {
        target.mday = 4;
      } else {
        target.mday = 15;
      }
    }
    val = ksu_mktime(&target);
    while (val.jdn == -1) {
      target.mday--;
      val = ksu_mktime(&target);
    }
  }
  return val;
}

extern char ksu_is_date(const char *dt, KSU_TIME_T *tptr) {
    char  ret = 0;

    if (dt && tptr) {
      *tptr =  mkksutime(dt);
      ret = (tptr->jdn != -1); 
    }
    return ret;
}

extern char ksu_is_time(const char *dt, KSU_TIME_T *tptr, char strict) {
    // If not strict, hours can be greater than 23 or a number
    // of days can precede the time
    char ret = 0;
    int  len;
    int  d;
    int  h;
    int  mi;
    int  s;

    if (dt && tptr) {
#ifdef PG_FUNC
      switch(pgspecial_search((char *)dt)) {
        //case PGSPECIAL_NOW:
        case PGSPECIAL_ALLBALLS:
             tptr->jdn = 0;
             tptr->sec = 0;
             tptr->flags = KSU_TIME_TIMEONLY;
             return (char)1;
        default:
             break;
      }
#endif
      len = strlen((char *)dt);
      if (strict) {
        if ((len == 8)
            && (sscanf((char *)dt, "%02d:%02d:%02d", &h, &mi, &s) == 3)
            && ((h>=0) && (h<24))
            && ((mi>=0) && (mi<60))
            && ((s>=0) && (s<60))) {
          tptr->jdn = 0;
          tptr->sec = (h*3600) + (mi*60) + s;
          tptr->flags = KSU_TIME_TIMEONLY;
          ret = 1;
        }
      } else {
        char *p;

        // If there is a space then we can have a number of
        // days coming first
        if (((p = strchr((char *)dt, ' ')) != (char *)NULL)
           || ((p = strchr((char *)dt, '\n')) != (char *)NULL)) {
          if (((p = strchr((char *)dt, ':')) != (char *)NULL)
              && (strlen(p) == 6)
              && (sscanf((char *)dt, "%d %02d:%02d:%02d",
                                     &d, &h, &mi, &s) == 4)
              && ((h>=0) && (h<24))
              && ((mi>=0) && (mi<60))
              && ((s>=0) && (s<60))) {
            tptr->jdn = (long long)d;
            tptr->sec = (h*3600) + (mi*60) + s;
            tptr->flags = KSU_TIME_XTIME;
            ret = 1;
          }
        } else {
          // We accept a number of hours greater than 23
          if (((p = strchr((char *)dt, ':')) != (char *)NULL)
              && (strlen(p) == 6)
              && (sscanf((char *)dt, "%d:%02d:%02d", &h, &mi, &s) == 3)
              && ((mi>=0) && (mi<60))
              && ((s>=0) && (s<60))) {
            tptr->jdn = h / 24;
            tptr->sec = ((h - (h/24) * 24) *3600) + (mi*60) + s;
            tptr->flags = KSU_TIME_XTIME;
            ret = 1;
          }
        }
      }
    }
    return ret;
}

extern char ksu_is_datetime(const char *dt,
                            KSU_TIME_T *tptr,
                            char        strict) {
    char       ret = 0;
    int        len;

    if (dt && tptr) {
      len = strlen((char *)dt);
#ifdef PG_FUNC
      switch(pgspecial_search((char *)dt)) {
        case PGSPECIAL_PLUSINFINITY:
        case PGSPECIAL_INFINITY:
             tptr->flags = KSU_TIME_INFINITE;
             return 1;
        case PGSPECIAL_MINUSINFINITY:
             tptr->flags = (KSU_TIME_INFINITE | KSU_TIME_BC);
             return 1;
        case PGSPECIAL_EPOCH:
             tptr->jdn = EPOCH_JDN;
             return 1;
        case PGSPECIAL_NOW:
             *tptr = ksu_currenttime(0);
             tptr->flags |= KSU_TIME_DATETIME;
             return 1;
        case PGSPECIAL_TODAY:
             *tptr = ksu_currenttime(1);
             return 1;
        case PGSPECIAL_TOMORROW:
             *tptr = ksu_currenttime(1);
             (tptr->jdn)++;
             return 1;
        case PGSPECIAL_YESTERDAY:
             *tptr = ksu_currenttime(1);
             (tptr->jdn)--;
             return 1;
        default:
             break;
      }
#endif
      if (strict) {
        if ((len != 19)
            && !((len == 20) && (G_min_julian < JAN_1ST_YEAR1))) {
          return (char)0;
        }
        tptr->flags |= KSU_TIME_DATETIME;
      }
      *tptr =  mkksutime(dt);
      ret = (tptr->jdn != -1); 
    }
    return ret;
}

static char iso_intv_read(int         what,
                          char       *num,
                          char       *dot,
                          int         what_next,
                          short       coef,
                          KSU_INTV_T *intv,
                          KSU_INTV_T *add) {
    char neg = 0;

    if (num && intv && add) {
      if (intv->qty[what]) {
        // Already seen
        return (char)0;
      }
      if (sscanf(num, "%hd", &(intv->qty[what])) != 1) {
        return (char)0;
      }
      if (num < 0) {
         neg = 1;
      }
      if (coef && dot) {
        float frac;

        if (sscanf(dot, "%f", &frac) != 1) {
          return (char)0;
        }
        add->qty[what_next] += (short)(coef * (neg ? -1 : 1) * frac);
      }
      return (char)1;
    }
    return (char)0;
}

#define  WEEK_SEEN       1
#define  OTHER_THAN_WEEK 2

static char ksu_iso_interval(const char *s, KSU_INTV_T *intv) {
    char        ret = 0;
    char       *p;
    char        time_part = 0;
    char       *num = NULL;
    char       *dot = NULL;
    int         i;
    char        combination = 0;
    char        neg = 0;
    char        alt = 0;
    KSU_INTV_T  add;  // Additional fractional parts

    if (s && intv) {
      p = (char *)s;
      for (i = 0; i < KSU_INTV_COUNT; i++) {
        add.qty[i] = 0;
      }
      if (*p == '-') {
        neg = 1;
        p++;
      } else if (*p == '+') {
        p++;
      }
      if (*p == 'P') {
        p++;
        if (strchr(p, '-')) {
          // P followed by YYYY-MM-DD
          alt = 2;
          if (sscanf(p, "%4hd-%02hd-%02hd",
                        &(intv->qty[KSU_INTV_YEAR]),
                        &(intv->qty[KSU_INTV_MONTH]),
                        &(intv->qty[KSU_INTV_DAY])) != 3) {
            return (char)0;
          }
          if ((intv->qty[KSU_INTV_MONTH]<0)
              ||(intv->qty[KSU_INTV_MONTH]>12)
              ||(intv->qty[KSU_INTV_DAY]<0)
              ||(intv->qty[KSU_INTV_DAY]>31)) {
            return (char)0;
          }
          p += 10;
        }
        while (*p) {
          if (time_part && (alt != 1) && strchr(p, ':')) {
            if (sscanf(p, "%02hd:%02hd:%02hd",
                          &(intv->qty[KSU_INTV_HOUR]),
                          &(intv->qty[KSU_INTV_MINUTE]),
                          &(intv->qty[KSU_INTV_SECOND])) != 3) {
              return (char)0;
            }
            if ((intv->qty[KSU_INTV_HOUR]>=0)
                &&(intv->qty[KSU_INTV_HOUR]<24)
                &&(intv->qty[KSU_INTV_MINUTE]>=0)
                &&(intv->qty[KSU_INTV_MINUTE]<60)
                &&(intv->qty[KSU_INTV_SECOND]>=0)
                &&(intv->qty[KSU_INTV_SECOND]<60)) {
               ret = 1;
            } else {
              return (char)0;
            }
            p += 8;
            if (*p) {
              // Not expecting anything there
              return (char)0;
            }
            break;
          }
          if (isdigit(*p)) {
            if (num == NULL) {
              num = p;
            }
          }
          switch (*p) {
            case '.':
            case ',':
                 if (dot) {
                   return (char)0;
                 }
                 dot = p;
                 break;
            case 'Y':
                 if (combination & WEEK_SEEN) {
                    return (char)0;
                 }
                 combination |= OTHER_THAN_WEEK;
                 if (!iso_intv_read(KSU_INTV_YEAR, num, dot,
                                    KSU_INTV_MONTH, 12, intv, &add)) {
                   return (char)0;
                 }
                 num = NULL;
                 dot = NULL;
                 break;
            case 'M':
                 if (combination & WEEK_SEEN) {
                    return (char)0;
                 }
                 combination |= OTHER_THAN_WEEK;
                 if (!time_part) {
                   if (!iso_intv_read(KSU_INTV_MONTH, num, dot,
                                      KSU_INTV_DAY, 30, intv, &add)) {
                     return (char)0;
                   }
                 } else {
                   if (!iso_intv_read(KSU_INTV_MINUTE, num, dot,
                                      KSU_INTV_SECOND, 60, intv, &add)) {
                     return (char)0;
                   }
                 }
                 num = NULL;
                 dot = NULL;
                 break;
            case 'W':
                 if (combination & OTHER_THAN_WEEK) {
                    return (char)0;
                 }
                 combination |= WEEK_SEEN;
                 if (!iso_intv_read(KSU_INTV_WEEK, num, dot,
                                    KSU_INTV_DAY, 7, intv, &add)) {
                   return (char)0;
                 }
                 num = NULL;
                 dot = NULL;
                 break;
            case 'D':
                 if (combination & WEEK_SEEN) {
                    return (char)0;
                 }
                 combination |= OTHER_THAN_WEEK;
                 if (!iso_intv_read(KSU_INTV_DAY, num, dot,
                                    KSU_INTV_HOUR, 24, intv, &add)) {
                   return (char)0;
                 }
                 num = NULL;
                 dot = NULL;
                 break;
            case 'H':
                 if (combination & WEEK_SEEN) {
                    return (char)0;
                 }
                 combination |= OTHER_THAN_WEEK;
                 if (!iso_intv_read(KSU_INTV_HOUR, num, dot,
                                    KSU_INTV_MINUTE, 60, intv, &add)) {
                   return (char)0;
                 }
                 num = NULL;
                 dot = NULL;
                 break;
            case 'S':
                 if (combination & WEEK_SEEN) {
                    return (char)0;
                 }
                 combination |= OTHER_THAN_WEEK;
                 if (!iso_intv_read(KSU_INTV_SECOND, num, dot,
                                    KSU_INTV_NOT_FOUND, 0, intv, &add)) {
                   return (char)0;
                 }
                 num = NULL;
                 dot = NULL;
                 break;
            case 'T':
                 time_part = 1;
                 break;
            default:
                 if (!isdigit(*p)) {
                   return (char)0;
                 }
                 break;
          }
          p++;
        }
        for (i = 0; i < KSU_INTV_COUNT; i++) {
          intv->qty[i] += add.qty[i];
          if (neg) {
            intv->qty[i] *= -1;
          }
        }
        // Change weeks to days
        if (intv->qty[KSU_INTV_WEEK]) {
          intv->qty[KSU_INTV_DAY] += 7 * intv->qty[KSU_INTV_WEEK];
          intv->qty[KSU_INTV_WEEK] = 0;
        }
        ret = 1;
      }
    }
    return ret;
}

static char ksu_sql_interval(const char *s, KSU_INTV_T *intv) {
   char  ret = 0;
   char *p;

   // "year-month"
   // or "days hh:mi:ss" (extended time)
   if (strchr((char *)s, '-')) {
     // Check that we have a pattern (digits)+-(digits)+
     p = (char *)s;
     while (isspace(*p)) {
       p++;
     }
     if (isdigit(*p)) {
       while (isdigit(*p)) {
         p++;
       }
       if (*p == '-') {
         p++;
         if (isdigit(*p)) {
           while (isdigit(*p)) {
             p++;
           }
           if (*p == '\0') {
             if (sscanf((char *)s, "%hd-%hd",
                        &(intv->qty[KSU_INTV_YEAR]),
                        &(intv->qty[KSU_INTV_MONTH])) == 2) {
               ret = 1;
             }
           }
         }
       }
     } 
   } else { 
     // Look for an extended time
     KSU_TIME_T  t = KSU_TIME_INIT;
     KSU_TM_T    mytm;
     KSU_TM_T   *tm;

     if (ksu_is_time(s, &t, 0)
         && ((tm = ksu_localtime(t, &mytm)) != (KSU_TM_T *)NULL)) {
       intv->qty[KSU_INTV_YEAR] = tm->year;
       intv->qty[KSU_INTV_MONTH] = tm->mon + 1;
       intv->qty[KSU_INTV_DAY] = tm->mday;
       intv->qty[KSU_INTV_HOUR] = tm->hour;
       intv->qty[KSU_INTV_MINUTE] = tm->min;
       intv->qty[KSU_INTV_SECOND] = tm->sec;
       ret = 1;
     }
   }
   return ret;
}

#define UNIT_LEN   20
#define _min(a, b) (a < b ? a : b)

#define P_MILLENNIUM 0
#define P_CENTURY   1
#define P_DECADE    2
#define P_YEAR      3
#define P_QUARTER   4
#define P_MONTH     5
#define P_WEEK      6
#define P_DAY       7
#define P_HOUR      8
#define P_MINUTE    9
#define P_SECOND   10

#define P_COUNT    11

static char ksu_pair_interval(const char *s, KSU_INTV_T *intv) {
    char  ret = 0;
    char *p;
    char *q;  // Quantity pointer
    char *u;  // Unit pointer
    char  unit[UNIT_LEN];
    int   unit_code;
    int   intv_code;
    int   intv_coef = 1;
    char  periods[P_COUNT] = {0,0,0,0,0,0,0,0,0,0,0};
    char  neg = 0;

    if (s && intv) {
      // Quantity/unit pairs
      // Look for a number first
      p = (char *)s;
      while (isspace(*p)) {
        p++;
      }
      if (*p == '\0') {
        return (char)2;
      }
      // Can be signed
      switch (*p) {
         case '-':
              neg = 1;
         case '+':
              p++;
              break;
         default:
              break;
      }
      if (!isdigit(*p)) {
        return (char)0;
      }
      q = p;
      while (isdigit(*p)) {
        p++;
      }
      while (isspace(*p)) {
        p++;
      }
      if (!isalpha(*p)) {
        return (char)0;
      }
      u = p;
      while (isalpha(*p)) {
        p++;
      }
      (void)memcpy(unit, u, _min(UNIT_LEN, p - u));
      unit[_min(UNIT_LEN, p - u)] = '\0';
      unit_code = kwintv_search(unit);
      intv_code = KSU_INTV_NOT_FOUND;
      switch (unit_code) {
        case KWINTV_C:
        case KWINTV_CENTURIES:
        case KWINTV_CENTURY:
             if (periods[P_CENTURY]) {
                return (char)0;
             }
             periods[P_CENTURY] = 1;
             intv_code = KSU_INTV_YEAR;
             intv_coef = 100;
             break;
        case KWINTV_D:
        case KWINTV_DAY:
        case KWINTV_DAYS:
             if (periods[P_DAY]) {
                return (char)0;
             }
             periods[P_DAY] = 1;
             intv_code = KSU_INTV_DAY;
             break;
        case KWINTV_DECADE:
        case KWINTV_DECADES:
             if (periods[P_DECADE]) {
                return (char)0;
             }
             periods[P_DECADE] = 1;
             intv_code = KSU_INTV_YEAR;
             intv_coef = 10;
             break;
        case KWINTV_H:
        case KWINTV_HOUR:
        case KWINTV_HOURS:
             if (periods[P_HOUR]) {
                return (char)0;
             }
             periods[P_HOUR] = 1;
             intv_code = KSU_INTV_HOUR;
             break;
        case KWINTV_M:
        case KWINTV_MIN:
        case KWINTV_MINS:
        case KWINTV_MINUTE:
        case KWINTV_MINUTES:
             if (periods[P_MINUTE]) {
                return (char)0;
             }
             periods[P_MINUTE] = 1;
             intv_code = KSU_INTV_MINUTE;
             break;
        case KWINTV_MON:
        case KWINTV_MONS:
        case KWINTV_MONTH:
        case KWINTV_MONTHS:
             if (periods[P_MONTH]) {
                return (char)0;
             }
             periods[P_MONTH] = 1;
             intv_code = KSU_INTV_MONTH;
             break;
        case KWINTV_S:
        case KWINTV_SEC:
        case KWINTV_SECOND:
        case KWINTV_SECONDS:
        case KWINTV_SECS:
             if (periods[P_SECOND]) {
                return (char)0;
             }
             periods[P_SECOND] = 1;
             intv_code = KSU_INTV_SECOND;
             break;
        case KWINTV_W:
        case KWINTV_WEEK:
        case KWINTV_WEEKS:
             if (periods[P_WEEK]) {
                return (char)0;
             }
             periods[P_WEEK] = 1;
             intv_code = KSU_INTV_DAY;
             intv_coef = 7;
             break;
        case KWINTV_Y:
        case KWINTV_YEAR:
        case KWINTV_YEARS:
             if (periods[P_YEAR]) {
                return (char)0;
             }
             periods[P_YEAR] = 1;
             intv_code = KSU_INTV_YEAR;
             break;
        default:
             return (char)0;;
      }
      if ((intv_code != KSU_INTV_NOT_FOUND)
         && (sscanf(q, "%hd", &(intv->qty[intv_code])) == 1)) {
        intv->qty[intv_code] *= (intv_coef * (neg ? -1 : 1));
        while (isspace(*p)) {
          p++;
        }
        ret = (ksu_pair_interval((const char *)p, intv) > 0);
      }
    }
    return ret;
}

static void intv_normalize(KSU_INTV_T *intv) {
   int  i;
   int  j;
   int  k;
   char neg;
   static struct {
           int code;
           int max;
          } limits[KSU_INTV_COUNT] = {{KSU_INTV_SECOND, 60},
                                      {KSU_INTV_MINUTE, 60},
                                      {KSU_INTV_HOUR, 24},
                                      {KSU_INTV_DAY, 0},
                                      {KSU_INTV_WEEK, 0},
                                      {KSU_INTV_MONTH, 12},
                                      {KSU_INTV_YEAR, 0}};

   if (intv) {
      for (i = 0; i < KSU_INTV_COUNT - 1; i++) {
        if (limits[i].max) {
          j = limits[i].code;
          neg = 0;
          if (intv->qty[j] < 0) {
            neg = 1;
            intv->qty[j] *= -1;
          }
          if (intv->qty[j] >= limits[i].max) {
            k = limits[i+1].code;
            intv->qty[k] += (neg ? -1 : 1) * (intv->qty[j] / limits[i].max);
            intv->qty[j] = (neg ? -1 : 1) * (intv->qty[j] % limits[i].max);
          } else {
            // Restore the sign
            if (neg) {
              intv->qty[j] *= -1;
            }
          }
        }
        if ((intv->qty[j] < 0) && limits[i].max) {
          k = limits[i+1].code;
          if (intv->qty[k] > 0) {
             intv->qty[k] -= 1;
             intv->qty[j] += limits[i].max;
          } 
        }
      }
   }
}

extern double ksu_difftime(KSU_TIME_T t1, KSU_TIME_T t2) { 
   // Computes the number of seconds in t1 - t2
   double    result = -1;
   long long days;

   if ((t1.jdn != -1) && (t2.jdn != -1)) {
     days = t1.jdn - t2.jdn;
     if (G_1582_gap) {
       if ((t1.jdn<GREG_START) && (t2.jdn>GREG_START)) {
         days += 10;
       } else if ((t2.jdn<GREG_START) && (t1.jdn>GREG_START)) {
         days -= 10;
       }
     }
     if ((t1.jdn<YEAR_MINUS_1_END) && (t2.jdn>YEAR1_START)) {
       days += (YEAR1_START - YEAR_MINUS_1_END);
     } else if ((t2.jdn<YEAR_MINUS_1_END) && (t1.jdn>YEAR1_START)) {
       days -= (YEAR1_START - YEAR_MINUS_1_END);
     }
     result = (double)days * 86400.0;
     result += ((double)t1.sec - (double)t2.sec);
   }
   return result;
}

// Sets the interval structure when valid
extern char ksu_is_interval(const char *s, KSU_INTV_T *intv) {
    char ret = 0;
    int  i;

    if (s && intv) {
      for (i = 0; i < KSU_INTV_COUNT; i++) {
        intv->qty[i] = 0;
      }
      if ((ret = ksu_iso_interval(s, intv)) == 0) {
        if ((ret = ksu_sql_interval(s, intv)) == 0) {
          if ((ret = ksu_pair_interval(s, intv)) != 0) {
            intv_normalize(intv);
          }
        } else {
          intv_normalize(intv);
        }
      } else {
        intv_normalize(intv);
      }
    }
    return ret;
}

// Return code for the ksu_extract functions:
// -2  invalid date part (at least for the returned datatype)
// -1  invalid date, interval or time
//  0  valid date part, irrelevant for this particular date type
//  1  OK
//
extern int ksu_extract_int(KSU_TIME_T t,
                           int        what,
                           long       *result_ptr) {
   if (result_ptr) {
     KSU_TM_T   tm;
     KSU_TM_T  *tmpt;
     char       isodate[DATE_LEN];
     long long  jdn;

     switch (what) {
       case KSU_DATE_PART_CENTURY:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            if (tmpt->year > 0) {
               *result_ptr = (long)(1 + (tmpt->year - 1) / 100);
            } else {
               *result_ptr = (long)((tmpt->year + 1) / 100 - 1);
            }
            break;
       case KSU_DATE_PART_DAY:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            *result_ptr = (long)tmpt->mday;
            break;
       case KSU_DATE_PART_JULIAN_NUM:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            // Pray for no overflow
            *result_ptr = (long)t.jdn;
            break;
       case KSU_DATE_PART_DAYOFYEAR:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            jdn = julian_day_num(tmpt->year, (short)1, (short)1);
            if (jdn != -1) {
              *result_ptr = (long)(1 + t.jdn - jdn);
            } else {
              return -1;
            }
            break;
       case KSU_DATE_PART_DECADE:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            *result_ptr = (long)(tmpt->year/10);
            break;
       case KSU_DATE_PART_ISOWEEK:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            *result_ptr = (long)ksu_iso_week(t);
            break;
       case KSU_DATE_PART_ISOWEEKDAY:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            *result_ptr = (long)isodow(t.jdn);
            break;
       case KSU_DATE_PART_ISOYEAR:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((ksu_iso_date(t, isodate) == (char *)NULL)
               || !sscanf(isodate, "%ld", result_ptr)) {
              return -1;
            }
            break;
       case KSU_DATE_PART_MILLENNIUM:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            if (tmpt->year > 0) {
              *result_ptr = (long)((tmpt->year - 1) / 1000 + 1);
            } else {
              *result_ptr = (long)((tmpt->year + 1) / 1000 - 1);
            }
            break;
       case KSU_DATE_PART_MONTH:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            *result_ptr = (long)(1 + tmpt->mon);
            break;
       case KSU_DATE_PART_QUARTER:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            *result_ptr = (long)(1 + (tmpt->mon/3));
            break;
       case KSU_DATE_PART_WEEK:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            // Week 1 is the week that contains Jan 1st.
            // Different from ISO
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            jdn = julian_day_num(tmpt->year, (short)1, (short)1);
            if (jdn != -1) {
              *result_ptr = (long)(1 + (t.jdn - jdn)/7);
            } else {
              return -1;
            }
            break;
       case KSU_DATE_PART_WEEKDAY:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            *result_ptr = (long)tmpt->wday;
            break;
       case KSU_DATE_PART_YEAR:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
              return -1;
            }
            *result_ptr = (long)tmpt->year;
            break;
       case KSU_DATE_PART_EPOCH:
            if (t.flags & KSU_TIME_TIMEONLY) {
              *result_ptr = (long)(t.sec);
            } else {
              *result_ptr = (long)((t.jdn - EPOCH_JDN)*86400 + t.sec);
            }
            break;
       case KSU_DATE_PART_SECONDS_IN_DAY:
            if (!(t.flags & (KSU_TIME_DATETIME | KSU_TIME_TIMEONLY))) {
              return 0;
            }
            *result_ptr = (long)t.sec;
            break;
       case KSU_DATE_PART_HOUR:
            if (!(t.flags & (KSU_TIME_DATETIME | KSU_TIME_TIMEONLY))) {
              return 0;
            }
            *result_ptr = (long)(t.sec / 3600);
            break;
       case KSU_DATE_PART_MINUTE:
            if (!(t.flags & (KSU_TIME_DATETIME | KSU_TIME_TIMEONLY))) {
              return 0;
            }
            *result_ptr = (long)(t.sec - (t.sec / 3600) * 3600) / 60;
            break;
       case KSU_DATE_PART_SECOND:
            if (!(t.flags & (KSU_TIME_DATETIME | KSU_TIME_TIMEONLY))) {
              return 0;
            }
            *result_ptr = (long)(t.sec % 60);
            break;
       default:
            return -2;
     }
     return 1;
   } else {
      return -1;
   }
}

extern int ksu_extract_dt_int(const char *date,
                              int         what,
                              long       *result_ptr) {
   KSU_INTV_T  intv;
   KSU_TIME_T  t;
   char        is_intv;

   if (ksu_is_datetime(date, &t, 0)) {
     if (t.flags & KSU_TIME_INFINITE) {
       return -1;
     }
     is_intv = 0;
   } else if (ksu_is_interval(date, &intv)) {
     is_intv = 1;
   } else if (ksu_is_time(date, &t, 1)) {
     if (t.flags & KSU_TIME_INFINITE) {
       return -1;
     }
     is_intv = 0;
   } else {
     return -1; 
   }
   if (is_intv) {
     // Extraction from an interval doesn't necessarily match
     // how the interval was specified.
     switch (what) {
       case KSU_DATE_PART_CENTURY:
            *result_ptr = (long)intv.qty[KSU_INTV_YEAR] / 100;
            break;
       case KSU_DATE_PART_DAY:
            *result_ptr = (long)intv.qty[KSU_INTV_DAY];
            break;
       case KSU_DATE_PART_DECADE:
            *result_ptr = (long)intv.qty[KSU_INTV_YEAR] / 10;
            break;
       case KSU_DATE_PART_EPOCH:
            // Pray for no overflow
            *result_ptr = (long)intv.qty[KSU_INTV_CENTURY] * 3155760000
                        + (long)intv.qty[KSU_INTV_YEAR] * 31557600
                        + (long)intv.qty[KSU_INTV_MONTH] * 2592000
                        + (long)intv.qty[KSU_INTV_WEEK] * 604800
                        + (long)intv.qty[KSU_INTV_DAY] * 86400
                        + (long)intv.qty[KSU_INTV_HOUR] * 3600
                        + (long)intv.qty[KSU_INTV_MINUTE] * 60
                        + (long)intv.qty[KSU_INTV_SECOND];
            break;
       case KSU_DATE_PART_HOUR:
            *result_ptr = (long)intv.qty[KSU_INTV_HOUR];
            break;
       case KSU_DATE_PART_MILLENNIUM:
            *result_ptr = (long)intv.qty[KSU_INTV_YEAR] / 1000;
            break;
       case KSU_DATE_PART_MINUTE:
            *result_ptr = (long)intv.qty[KSU_INTV_MINUTE];
            break;
       case KSU_DATE_PART_MONTH:
            *result_ptr = (long)intv.qty[KSU_INTV_MONTH];
            break;
       case KSU_DATE_PART_QUARTER:
            *result_ptr = (long)intv.qty[KSU_INTV_MONTH] / 3;
            break;
       case KSU_DATE_PART_SECOND:
            *result_ptr = (long)intv.qty[KSU_INTV_SECOND];
            break;
       case KSU_DATE_PART_WEEK:
            *result_ptr = (long)intv.qty[KSU_INTV_DAY] / 7;
            break;
       case KSU_DATE_PART_YEAR:
            *result_ptr = (long)intv.qty[KSU_INTV_YEAR];
            break;
       default:
            return -2;
     }
   } else {
     return ksu_extract_int(t, what, result_ptr);
   }
   return -1;
}

extern int ksu_extract_str(KSU_TIME_T  t,
                           int         what,
                           char       *result,
                           int         resultsz) {
   if (result && (resultsz > 0)) {
     KSU_TM_T   tm;
     KSU_TM_T  *tmpt;

     ksu_i18n();  // Just in case
     if ((what != KSU_DATE_PART_WEEKDAYNAME)
         && (what != KSU_DATE_PART_WEEKDAYABBREV)) {
       if ((tmpt = ksu_localtime(t, &tm)) == (KSU_TM_T *)NULL) {
         return -1;
       }
     }
     switch (what) {
       case KSU_DATE_PART_DAY_HOUR:
            if (!(t.flags & KSU_TIME_DATETIME)) {
              return 0;
            }
            snprintf(result, resultsz, "%hd %02hd",
                     tmpt->mday,
                     tmpt->hour);
            break;
       case KSU_DATE_PART_DAY_MINUTE:
            if (!(t.flags & KSU_TIME_DATETIME)) {
              return 0;
            }
            snprintf(result, resultsz, "%hd %02hd:%02hd",
                     tmpt->mday,
                     tmpt->hour,
                     tmpt->min);
            break;
       case KSU_DATE_PART_DAY_SECOND:
            if (!(t.flags & KSU_TIME_DATETIME)) {
              return 0;
            }
            snprintf(result, resultsz, "%hd %02hd:%02hd:%02hd",
                     tmpt->mday,
                     tmpt->hour,
                     tmpt->min,
                     tmpt->sec);
            break;
       case KSU_DATE_PART_HOUR_MINUTE:
            if (!(t.flags & (KSU_TIME_DATETIME | KSU_TIME_TIMEONLY))) {
              return 0;
            }
            snprintf(result, resultsz, "%02hd:%02hd",
                     tmpt->hour,
                     tmpt->min);
            break;
       case KSU_DATE_PART_HOUR_SECOND:
            if (!(t.flags & (KSU_TIME_DATETIME | KSU_TIME_TIMEONLY))) {
              return 0;
            }
            snprintf(result, resultsz, "%02hd:%02hd:%02hd",
                     tmpt->hour,
                     tmpt->min,
                     tmpt->sec);
            break;
       case KSU_DATE_PART_MINUTE_SECOND:
            if (!(t.flags & (KSU_TIME_DATETIME | KSU_TIME_TIMEONLY))) {
              return 0;
            }
            snprintf(result, resultsz, "%02hd:%02hd",
                     tmpt->min,
                     tmpt->sec);
            break;
       case KSU_DATE_PART_MONTHABBREV:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            snprintf(result, resultsz, "%s",
                     nl_langinfo(G_month_name[tmpt->mon].abbr));
            break;
       case KSU_DATE_PART_MONTHNAME:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            snprintf(result, resultsz, "%s",
                     nl_langinfo(G_month_name[tmpt->mon].full));
            break;
       case KSU_DATE_PART_WEEKDAYNAME:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            snprintf(result, resultsz, "%s",
                     nl_langinfo(G_day_name[dow(t.jdn)].full));
            break;
       case KSU_DATE_PART_WEEKDAYABBREV:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            snprintf(result, resultsz, "%s",
                     nl_langinfo(G_day_name[dow(t.jdn)].abbr));
            break;
       case KSU_DATE_PART_YEAR_MONTH:
            if (t.flags & KSU_TIME_TIMEONLY) {
              return 0;
            }
            snprintf(result, resultsz, "%d-%02d",
                     tmpt->year,
                     1 + tmpt->mon);
            break;
       default:
            return -2;
     }
     return 1;
   }
   return -1;
}

extern int ksu_extract_dt_str(const char *date,
                              int         what,
                              char       *result,
                              int         resultsz) {
   KSU_INTV_T  intv;
   KSU_TIME_T  t;
   char        is_intv;

   if (ksu_is_datetime(date, &t, 0)) {
     if (t.flags & KSU_TIME_INFINITE) {
       return -1;
     }
     is_intv = 0;
   } else if (ksu_is_interval(date, &intv)) {
     is_intv = 1;
   } else if (ksu_is_time(date, &t, 1)) {
     if (t.flags & KSU_TIME_INFINITE) {
       return -1;
     }
     is_intv = 0;
   } else {
     return -1; 
   }
   if (is_intv) {
     switch (what) {
       case KSU_DATE_PART_DAY_HOUR:
            snprintf(result, resultsz, "%hd day%s %02hd hour%s",
                     intv.qty[KSU_INTV_DAY],
                     (intv.qty[KSU_INTV_DAY]>1? "s":""),
                     intv.qty[KSU_INTV_HOUR],
                     (intv.qty[KSU_INTV_HOUR]>1? "s":""));
            break;
       case KSU_DATE_PART_DAY_MINUTE:
            snprintf(result, resultsz, "%hd day%s %02hd:%02hd",
                     intv.qty[KSU_INTV_DAY],
                     (intv.qty[KSU_INTV_DAY]>1? "s":""),
                     intv.qty[KSU_INTV_HOUR],
                     intv.qty[KSU_INTV_MINUTE]);
            break;
       case KSU_DATE_PART_DAY_SECOND:
            snprintf(result, resultsz, "%hd day%s %02hd:%02hd:%02hd",
                     intv.qty[KSU_INTV_DAY],
                     (intv.qty[KSU_INTV_DAY]>1? "s":""),
                     intv.qty[KSU_INTV_HOUR],
                     intv.qty[KSU_INTV_MINUTE],
                     intv.qty[KSU_INTV_SECOND]);
            break;
       case KSU_DATE_PART_HOUR_MINUTE:
            snprintf(result, resultsz, "%02hd:%02hd",
                     intv.qty[KSU_INTV_HOUR],
                     intv.qty[KSU_INTV_MINUTE]);
            break;
       case KSU_DATE_PART_HOUR_SECOND:
            snprintf(result, resultsz, "%02hd:%02hd:%02hd",
                     intv.qty[KSU_INTV_HOUR],
                     intv.qty[KSU_INTV_MINUTE],
                     intv.qty[KSU_INTV_SECOND]);
            break;
       case KSU_DATE_PART_MINUTE_SECOND:
            snprintf(result, resultsz, "%02hd:%02hd",
                     intv.qty[KSU_INTV_MINUTE],
                     intv.qty[KSU_INTV_SECOND]);
            break;
       case KSU_DATE_PART_YEAR_MONTH:
            snprintf(result, resultsz, "%02hd-%02hd",
                     intv.qty[KSU_INTV_YEAR],
                     intv.qty[KSU_INTV_MONTH]);
            break;
       case KSU_DATE_PART_MONTHABBREV:
       case KSU_DATE_PART_MONTHNAME:
       case KSU_DATE_PART_WEEKDAYABBREV:
       case KSU_DATE_PART_WEEKDAYNAME:
            return 0;
       default:
            return -2;
     }
   } else {
     return ksu_extract_str(t, what, result, resultsz);
   }
   return 0;
}

extern char *ksu_short_date_fmt(char *buff, short *pdo) {
    if (buff && pdo) {
      ksu_i18n();
      switch (ksu_territory()) {
        case KSU_TERR_BE:
        case KSU_TERR_IE:
        case KSU_TERR_IN:
        case KSU_TERR_NL:
        case KSU_TERR_DK:
             strcpy(buff, "%02hd-%02hd-%hd");
             *pdo = KSU_DATE_ORDER_DMY; 
             break;
        case KSU_TERR_CZ:
        case KSU_TERR_CH:
        case KSU_TERR_BY:
        case KSU_TERR_DE:
        case KSU_TERR_AT:
        case KSU_TERR_BG:
        case KSU_TERR_EE:
        case KSU_TERR_FI:
        case KSU_TERR_HR:
        case KSU_TERR_IS:
        case KSU_TERR_RO:
        case KSU_TERR_RU:
        case KSU_TERR_SK:
        case KSU_TERR_NO:
        case KSU_TERR_PL:
        case KSU_TERR_TR:
        case KSU_TERR_UA:
             strcpy(buff, "%02hd.%02hd.%hd");
             *pdo = KSU_DATE_ORDER_DMY; 
             break;
        case KSU_TERR_AU:
        case KSU_TERR_FR:
        case KSU_TERR_GB:
        case KSU_TERR_BR:
        case KSU_TERR_ES:
        case KSU_TERR_GR:
        case KSU_TERR_HK:
        case KSU_TERR_IL:
        case KSU_TERR_IT:
        case KSU_TERR_PT:
             strcpy(buff, "%02hd/%02hd/%hd");
             *pdo = KSU_DATE_ORDER_DMY; 
             break;
        case KSU_TERR_HU:
        case KSU_TERR_KZ:
             strcpy(buff, "%hd.%02hd.%02hd");
             *pdo = KSU_DATE_ORDER_YMD; 
             break;
        case KSU_TERR_ZA:
             strcpy(buff, "%hd/%02hd/%02hd");
             *pdo = KSU_DATE_ORDER_YMD; 
             break;
        case KSU_TERR_US:
             strcpy(buff, "%hd/%hd/%hd");
             *pdo = KSU_DATE_ORDER_MDY; 
             break;
        default:
             strcpy(buff, "%hd-%02hd-%02hd");
             *pdo = KSU_DATE_ORDER_YMD; 
             break;
      }
    }
    return buff;
}

extern char *ksu_short_date(KSU_TIME_T t, char *buff) {
    KSU_TM_T tm;
    short    date_order;
    char     format[DATE_LEN];

    if (buff) {
      if (ksu_localtime(t, &tm)
          && ksu_short_date_fmt(format, &date_order)) {
        switch (date_order) {
          case KSU_DATE_ORDER_DMY:
               sprintf(buff, format,
                       tm.mday,
                       (short)(1 + tm.mon),
                       tm.year); 
               break;
          case KSU_DATE_ORDER_MDY:
               sprintf(buff, format,
                       (short)(1 + tm.mon),
                       tm.mday,
                       tm.year); 
               break;
          default: //  KSU_DATE_ORDER_YMD
               sprintf(buff, format,
                       tm.year,
                       (short)(1 + tm.mon),
                       tm.mday); 
               break;
        }
      } else {
        return (char *)NULL;
      }
    }
    return buff;
}

// Debugging functions

extern void ksu_print_tm(KSU_TM_T *t) {
  // For debugging
  if (t) {
    printf("year : %hd\n", t->year); 
    printf("mon  : %hd\n", t->mon); 
    printf("mday : %hd\n", t->mday); 
    printf("wday : %hd\n", t->wday); 
    printf("hour : %hd\n", t->hour); 
    printf("min  : %hd\n", t->min); 
    printf("sec  : %hd\n", t->sec); 
  }
}

extern void ksu_print_time(KSU_TIME_T *t) {
  // For debugging
  if (t) {
    printf("flags : %02x\n", (unsigned char)t->flags); 
    printf("jdn   : %lld\n", t->jdn); 
    printf("sec   : %u\n", t->sec); 
  }
}

extern void ksu_print_intv(KSU_INTV_T *i) {
  // For debugging
  int k;

  if (i) {
     for (k = 0; k < KSU_INTV_COUNT; k++) {
        printf("%s\t%hd\n", ksu_intv_keyword(k), i->qty[k]);
     }
  }
}
