#include <stdio.h>
#include <string.h>

#include "ksu_date_part.h"

static char *G_ksu_date_part_words[] = {
    "century",
    "day",
    "day_hour",
    "day_minute",
    "day_second",
    "dayofyear",
    "decade",
    "epoch",
    "hour",
    "hour_minute",
    "hour_second",
    "isoweek",
    "isoweekday",
    "isoyear",
    "julian_num",
    "millennium",
    "minute",
    "minute_second",
    "month",
    "monthabbrev",
    "monthname",
    "quarter",
    "second",
    "seconds_in_day",
    "week",
    "weekday",
    "weekdayabbrev",
    "weekdayname",
    "year",
    "year_month",
    NULL};

extern int ksu_date_part_search(char *w) {
  int start = 0;
  int end = KSU_DATE_PART_COUNT - 1;
  int mid;
  int pos = KSU_DATE_PART_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_ksu_date_part_words[mid], w)) == 0) {
         pos = mid;
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

extern char *ksu_date_part_keyword(int code) {
  if ((code >= 0) && (code < KSU_DATE_PART_COUNT)) {
    return G_ksu_date_part_words[code];
  } else {
    return (char *)NULL;
  }
}

