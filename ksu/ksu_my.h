#ifndef KSU_MY_HEADER

#define KSU_MY_HEADER

#include <ksu_common.h>
#include <ksu_dates.h>

// Time/Interval units
#define MYTIM_NOT_FOUND      	 -1
#define MYTIM_DAY          	      0
#define MYTIM_DAY_HOUR	          1
#define MYTIM_DAY_MICROSECOND	  2
#define MYTIM_DAY_MINUTE	      3
#define MYTIM_DAY_SECOND	      4
#define MYTIM_HOUR	              5
#define MYTIM_HOUR_MICROSECOND	  6
#define MYTIM_HOUR_MINUTE	      7
#define MYTIM_HOUR_SECOND	      8
#define MYTIM_MICROSECOND	      9
#define MYTIM_MINUTE	         10
#define MYTIM_MINUTE_MICROSECOND 11
#define MYTIM_MINUTE_SECOND	     12
#define MYTIM_MONTH	             13
#define MYTIM_QUARTER	         14
#define MYTIM_SECOND	         15
#define MYTIM_SECOND_MICROSECOND 16
#define MYTIM_WEEK	             17
#define MYTIM_YEAR	             18
#define MYTIM_YEAR_MONTH	     19

#define MYTIM_COUNT	             20

extern int   mytim_search(char *w);
extern char *mytim_keyword(int code);

extern void  my_date_op(sqlite3_context * context,
                        char            * fname,
                        char            * date,
                        char              add,
                        char            * interval);
extern void  my_time_op(sqlite3_context * context,
                        char            * fname,
                        char              add,
                        char            * expr1,
                        char            * expr2);
// MySQL is pretty lax when an integer parameter is expected.
// Float values are rounded to the nearest integer, and char
// values sometimes magically transmogrify into an integer
// value. The following functions do it (def is what to return
// when val isn't numerical)
extern int           my_value_int(sqlite3_value *val, int def);
extern sqlite3_int64 my_value_int64(sqlite3_value *val, int def);
extern double        my_value_double(sqlite3_value *val, double def);

// MySQL attempts to transform anything into a date.
extern char          my_is_date(const char *dt, KSU_TIME_T *tptr);
extern char          my_is_datetime(const char *dt,
                                    KSU_TIME_T *tptr,
                                    char        strict);

extern char          my_is_interval(const char *s, KSU_INTV_T *intv);

extern char          my_int_date(sqlite3_value *val, KSU_TM_T *tm);
extern char          my_int_datetime(sqlite3_value *val, KSU_TM_T *tm);

#endif
