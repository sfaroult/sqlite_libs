#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#include <ksu_common.h>

// Forward declaration
static int leapYear(int year);
static int monthAndDayFromDays(int days, int leapYear, int *monthDay);

/*
 * The MAKEDATE function returns a date, given year and day-of-year values.
 * dayofyear must be greater than 0 or the result is NULL. Example: SELECT
 * MAKEDATE(2011,31), MAKEDATE(2011,32); -> '2011-01-31', '2011-02-01' SELECT
 * MAKEDATE(2011,0); -> NULL
 */
extern void  my_makedate(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {

        if (ksu_prm_ok(context, argc, argv, "makedate",
                       KSU_PRM_INT, KSU_PRM_INT)) {
          int year = sqlite3_value_int(argv[0]);
          int dayOfYear = sqlite3_value_int(argv[1]);

          if (dayOfYear < 1 || dayOfYear > 366) {
            sqlite3_result_null(context);
            return;
          }
          //Detecting the date
          int  month, day;

          month = monthAndDayFromDays(dayOfYear,
                                      leapYear(year), &day);

          //Return the result
          char *dateResult = sqlite3_malloc(sizeof(char) * 11);
          sprintf(dateResult, "%04d-%02d-%02d", year, month, day);
          sqlite3_result_text(context, dateResult, 11, sqlite3_free);
         }
}

/*
 * This function returns 1 if the year is a leap year. Return 0 otherwise.
 */
static int leapYear(int year) {
  //Leap year case
  if ((year & 3) == 0 && ((year % 25) != 0 || (year & 15) == 0)) {
     return 1;
  }
  return 0;
}

/*
 * This function finds day and month from a day of a year. LeapYear flag is
 * required to calculate a correct value.
 */
static int monthAndDayFromDays(int days, int leapYear, int *monthDay) {
  int  i;
  int  months[12] = {
                31, //January
                leapYear > 0 ? 29 : 28, //February
                31, //March
                30, //April
                31, //May
                30, //June
                31, //July
                31, //August
                30, //September
                31, //October
                30, //November
                31 // December
        };

  int  month = 0;

  for (i = 0; i < 12; ++i) {
    if (days - months[i] <= 0) {
      month = i + 1;
      break;
    }
    //Keep counting off days
    days -= months[i];
  }

  //Assign number of days
  *monthDay = days;
  return month;
}
