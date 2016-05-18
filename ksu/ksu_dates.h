#ifndef KSU_DATES_HEADER

#define KSU_DATES_HEADER

#include <ksu_intv.h>
#include <kwintv.h>

#define KSU_DATE_PART_NOT_FOUND	-1
#define KSU_DATE_PART_CENTURY	  0
#define KSU_DATE_PART_DAY	  1
#define KSU_DATE_PART_DAY_HOUR	  2
#define KSU_DATE_PART_DAY_MINUTE	  3
#define KSU_DATE_PART_DAY_SECOND	  4
#define KSU_DATE_PART_DAYOFYEAR	  5
#define KSU_DATE_PART_DECADE	  6
#define KSU_DATE_PART_EPOCH	  7
#define KSU_DATE_PART_HOUR	  8
#define KSU_DATE_PART_HOUR_MINUTE	  9
#define KSU_DATE_PART_HOUR_SECOND	 10
#define KSU_DATE_PART_ISOWEEK	 11
#define KSU_DATE_PART_ISOWEEKDAY	 12
#define KSU_DATE_PART_ISOYEAR	 13
#define KSU_DATE_PART_JULIAN_NUM	 14
#define KSU_DATE_PART_MILLENNIUM	 15
#define KSU_DATE_PART_MINUTE	 16
#define KSU_DATE_PART_MINUTE_SECOND	 17
#define KSU_DATE_PART_MONTH	 18
#define KSU_DATE_PART_MONTHABBREV	 19
#define KSU_DATE_PART_MONTHNAME	 20
#define KSU_DATE_PART_QUARTER	 21
#define KSU_DATE_PART_SECOND	 22
#define KSU_DATE_PART_SECONDS_IN_DAY	 23
#define KSU_DATE_PART_WEEK	 24
#define KSU_DATE_PART_WEEKDAY	 25
#define KSU_DATE_PART_WEEKDAYABBREV	 26
#define KSU_DATE_PART_WEEKDAYNAME	 27
#define KSU_DATE_PART_YEAR	 28
#define KSU_DATE_PART_YEAR_MONTH	 29

#define KSU_DATE_PART_COUNT	30

// Language-dependent date component order
//  Y = year, M = month, D = day of the month

#define KSU_DATE_ORDER_YMD     0     // ISO
#define KSU_DATE_ORDER_MDY     1     // US
#define KSU_DATE_ORDER_DMY     2     // Europe, among many others

#define DATE_LEN   25

// Time flags
#define KSU_TIME_DATETIME    1
#define KSU_TIME_INFINITE    2
    // Only used with PostgreSQL
#define KSU_TIME_BC          4
    // Only used with Oracle and PostgreSQL
#define KSU_TIME_TIMEONLY    8
#define KSU_TIME_XTIME      16
    // Extended time - may contain days

typedef struct {
                short qty[KSU_INTV_COUNT];
               } KSU_INTV_T;

// Our equivalent of struct tm
typedef struct {
                short year; // ACTUAL value
                short mon;  // 0 to 11
                short mday;
                short wday; // 0 to 6, Sun = 0
                short hour;
                short min;
                short sec;
               } KSU_TM_T;

// Our equivalent of time_t - doesn't return seconds,
// but a Julian date and seconds (flags tell various things,
// BC, initally included a time component, infinite, etc)
typedef struct {
                char         flags;
                long long    jdn;
                unsigned int sec;
               }  KSU_TIME_T;

#define KSU_TIME_INIT   {0, -1, 0}

// The following macros apply to a KSU_TIME_T variable
#define _ksu_valid_date(t)  (t.jdn == -1 ? 0 : 1)
#define _ksu_pure_date(t)   (t.sec ? 0 : 1)

extern KSU_TIME_T  ksu_mktime(KSU_TM_T *tm);
extern KSU_TM_T   *ksu_localtime(KSU_TIME_T t, KSU_TM_T *tmptr);
extern char       *ksu_datetime(KSU_TIME_T t, char *buff);
extern KSU_TIME_T  ksu_currenttime(char dateonly);
extern KSU_TIME_T  ksu_currentutc(char dateonly);
extern double      ksu_difftime(KSU_TIME_T t1, KSU_TIME_T t2);
                   // Computes the number of seconds in t1 - t2

extern KSU_TIME_T  ksu_add_months(KSU_TM_T *d, int n);
extern KSU_TIME_T  ksu_add_secs(KSU_TIME_T t, int secs);

extern char       *ksu_iso_date(KSU_TIME_T t, char *isodate);
extern short       ksu_iso_week(KSU_TIME_T t);
extern char        ksu_long_iso_year(int y);
                           // Returns 1 for 53-week year, 0 for 52-week year

extern char        ksu_is_date(const char *dt, KSU_TIME_T *tptr);
extern char        ksu_is_datetime(const char *dt,
                                   KSU_TIME_T *tptr,
                                   char        strict);
extern char        ksu_is_time(const char *d,
                               KSU_TIME_T *tptr,
                               char        strict);

extern char        ksu_is_interval(const char *s, KSU_INTV_T *intv);

// Return code for the ksu_extract functions:
// -2  invalid date part
// -1  invalid date, interval or time
//  0  valid date part, irrelevant for this particular date type
//  1  OK
//
extern int         ksu_extract_int(KSU_TIME_T t,
                                   int        what,
                                   long       *result_ptr);
extern int         ksu_extract_dt_int(const char *date,
                                      int         what,
                                      long       *result_ptr);

extern int         ksu_extract_str(KSU_TIME_T  t,
                                   int         what,
                                   char       *result,
                                   int         resultsz);
extern int         ksu_extract_dt_str(const char *date,
                                      int         what,
                                      char       *result,
                                      int         resultsz);

// Short date format
// pdo is a pointer to a value that says which is the order
// (YMD, DMY, or MDY)
extern char       *ksu_short_date_fmt(char *buff, short *pdo);
// Short date
extern char       *ksu_short_date(KSU_TIME_T t, char *buff);

/*
 *   The four following functions the language-dependent
 *   maximum length of a month name and of a week day name
 *   (full and abbreviated).
 */
extern int         ksu_maxmonth(void);
extern int         ksu_maxmon(void);
extern int         ksu_maxday(void);
extern int         ksu_maxdy(void);
/*
 *   Number of the day corresponding to the name in the current
 *   language (only the abbreviation is checked, supplemental
 *   characters are ignored)
 */
extern int         ksu_weekday(char *dayname);

/*
 *    Identify month/week day by name (language dependent of course)
 */
extern int         ksu_month_by_abbr(char *abbr);
extern int         ksu_month_by_name(char *name);
extern int         ksu_day_by_abbr(char *abbr);
extern int         ksu_day_by_name(char *name);

/*
 *    For debugging
 */
extern void ksu_print_tm(KSU_TM_T *t);
extern void ksu_print_time(KSU_TIME_T *t);
extern void ksu_print_intv(KSU_INTV_T *i);

#endif
