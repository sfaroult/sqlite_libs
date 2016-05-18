/*
 *
 *  Returns 00:00:00 there is no time part (date value) and
 *  NULL if the parameter isn't a date.
 *
 *  The actual MySQL function looks interestingly buggy:
 *
 *  mysql> select version();
 *  +-----------------+
 *  | version()       |
 *  +-----------------+
 *  | 5.1.73-1+deb6u1 |
 *  +-----------------+
 *  1 row in set (0.00 sec)
 *
 *  mysql> select current_time, time(current_date), current_date;
 *  +--------------+--------------------+--------------+
 *  | current_time | time(current_date) | current_date |
 *  +--------------+--------------------+--------------+
 *  | 13:36:57     | 00:20:15           | 2015-06-11   |
 *  +--------------+--------------------+--------------+
 *  1 row in set, 1 warning (0.00 sec)
 *
 *  mysql> select time('HELLO'), time('1970-01-01');
 *  +---------------+--------------------+
 *  | time('HELLO') | time('1970-01-01') |
 *  +---------------+--------------------+
 *  | 00:00:00      | NULL               |
 *  +---------------+--------------------+
 *  1 row in set, 2 warnings (0.00 sec)
 *
 *  mysql> select time('HELLO'), time(cast('1970-01-01' as date));
 *  +---------------+----------------------------------+
 *  | time('HELLO') | time(cast('1970-01-01' as date)) |
 *  +---------------+----------------------------------+
 *  | 00:00:00      | 00:00:00                         |
 *  +---------------+----------------------------------+
 *  1 row in set, 1 warning (0.00 sec)
 *
 *  mysql> 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>

extern void my_time(sqlite3_context  *context,
                    int argc,
                    sqlite3_value   **argv) {
   int             len;
   unsigned char   *str;
   char             result[20];
   int              y, m, d, h, mi, sec;

   _ksu_null_if_null_param(argc, argv);
   str = (unsigned char *)sqlite3_value_text(argv[0]);
   len = strlen((char *)str);
   switch (len) {
     case 19:
          if ((sscanf((char *)str, "%4d-%02d-%02d %02d:%02d:%02d",
                      &y, &m, &d, &h, &mi, &sec) == 6)
              && ((m > 0) && (m <= 12))
              && ((d > 0) && (d <= 31))
              && ((h >= 0) && (h < 24))
              && ((mi >= 0) && (mi < 50))
              && ((sec >= 0) && (sec < 50))) {
            sprintf(result, "%02d:%02d:%02d", h, mi, sec);
            sqlite3_result_text(context, result, -1, SQLITE_TRANSIENT);
          } else {
            sqlite3_result_null(context);
          }
          break;
     case 10:
          if ((sscanf((char *)str, "%4d-%02d-%02d", &y, &m, &d) == 3)
              && ((m > 0) && (m <= 12))
              && ((d > 0) && (d <= 31))) {
            strcpy(result, "00:00:00");
            sqlite3_result_text(context, result, -1, SQLITE_TRANSIENT);
          } else {
            sqlite3_result_null(context);
          }
          break;
     default:
          sqlite3_result_null(context);
          break;
   }
}
