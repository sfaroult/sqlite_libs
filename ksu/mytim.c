#include <stdio.h>
#include <string.h>

#include "mytim.h"

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

