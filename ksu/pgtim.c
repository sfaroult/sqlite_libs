#include <stdio.h>
#include <string.h>

#include "pgtim.h"

static char *G_pgtim_words[] = {
    "c",
    "centuries",
    "century",
    "d",
    "day",
    "days",
    "decade",
    "decades",
    "dow",
    "doy",
    "epoch",
    "h",
    "hour",
    "hours",
    "isodow",
    "isoyear",
    "m",
    "microsecond",
    "microseconds",
    "millennium",
    "millisecond",
    "milliseconds",
    "min",
    "mins",
    "minute",
    "minutes",
    "mon",
    "mons",
    "month",
    "months",
    "quarter",
    "s",
    "sec",
    "second",
    "seconds",
    "secs",
    "timezone",
    "timezone_hour",
    "timezone_minute",
    "w",
    "week",
    "weeks",
    "y",
    "year",
    "years",
    NULL};

extern int pgtim_search(char *w) {
  int start = 0;
  int end = PGTIM_COUNT - 1;
  int mid;
  int pos = PGTIM_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_pgtim_words[mid], w)) == 0) {
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

extern char *pgtim_keyword(int code) {
  if ((code >= 0) && (code < PGTIM_COUNT)) {
    return G_pgtim_words[code];
  } else {
    return (char *)NULL;
  }
}

