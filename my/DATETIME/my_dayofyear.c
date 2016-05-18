/*
 * An implementation of mysql's day of year function.
 * 
 * https://dev.mysql.com/doc/refman/5.7/en/date-and-time-functions.html
 * http://www.cplusplus.com/reference/ctime/tm/
 * 
 * It expects to be given a date in the format "YYYY-MM-DD". It will return the
 * day of year of that date as an integer.
 */
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include <ksu_common.h>

static int dateStringToDayOfYear(const char *dateString) {
    struct tm       dateToParse;
    struct tm       mydate;
    struct tm      *tmptr;
    int             year, month, day;
    time_t          secondsConversion = (time_t) 0;

    if (3 != sscanf(dateString, "%d-%d-%d", &year, &month, &day))
       return -1;
    dateToParse.tm_year = year - 1900;
    //stored as years sinced 1900
    dateToParse.tm_mon = month - 1;
    //stored as 0 = Jan,[...], 11 = Dec
    dateToParse.tm_mday = day;
    //stored as expected:1 = first day
    dateToParse.tm_hour = 0;
    dateToParse.tm_min = 0;
    dateToParse.tm_sec = 0;
    if ((secondsConversion = mktime(&dateToParse)) == (time_t)-1)
       return -1;
    tmptr = localtime_r(&secondsConversion, &mydate);
    if (tmptr) {
      return (tmptr->tm_yday + 1);
    }
    return -1;
}

extern void  my_dayofyear(sqlite3_context * context,
                          int               argc,
                          sqlite3_value  ** argv) {
        const char     *dateString;
        int             doy;

        if (ksu_prm_ok(context, argc, argv, "dayofyear", KSU_PRM_DATETIME)) {
           dateString = (const char *)sqlite3_value_text(argv[0]);
           if ((doy = dateStringToDayOfYear(dateString)) == -1) {
             ksu_err_msg(context, KSU_ERR_INV_DATE,
                         (char *)dateString, "dayofyear");
           } else {
             sqlite3_result_int(context, doy);
           }
        }
}
