#include <stdio.h>
#include <string.h>

#include "kwintv.h"

static char *G_kwintv_words[] = {
    "years",
    "year",
    "y",
    "weeks",
    "week",
    "w",
    "secs",
    "seconds",
    "second",
    "sec",
    "s",
    "months",
    "month",
    "mons",
    "mon",
    "minutes",
    "minute",
    "mins",
    "min",
    "m",
    "hours",
    "hour",
    "h",
    "decades",
    "decade",
    "days",
    "day",
    "d",
    "century",
    "centuries",
    "c",
    NULL};

extern int kwintv_search(char *w) {
  int start = 0;
  int end = KWINTV_COUNT - 1;
  int mid;
  int pos = KWINTV_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_kwintv_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
      } else {
        if (comp > 0) {
           start = mid + 1;
        } else {
           end = mid - 1;
        }
      }
    }
  }
  return pos;
}

extern int kwintv_best_match(char *w) {
  int pos = KWINTV_NOT_FOUND;
  int i = 0;
  int comp = 1;

  if (w) {
    while ((i < KWINTV_COUNT) && (comp > 0)) {
      comp = strncasecmp(G_kwintv_words[i],w,strlen(G_kwintv_words[i]));
      i++;
    }
    if (comp == 0) {
      pos = i - 1;
    }
  }
  return pos;
}

extern int kwintv_abbrev(char *w) {
  int pos = KWINTV_NOT_FOUND;
  int i = 0;
  int len;
  int comp = 1;

  if (w) {
    len = strlen(w);
    while ((i < KWINTV_COUNT) && (comp > 0)) {
      comp = strncasecmp(G_kwintv_words[i],w,len);
      i++;
    }
    if (comp == 0) {
      pos = i - 1;
      if ((i < KWINTV_COUNT)
          && (strncasecmp(G_kwintv_words[i],w,len)==0)) {
         pos = KWINTV_AMBIGUOUS;
      }
    }
  }
  return pos;
}

extern char *kwintv_keyword(int code) {
  if ((code >= 0) && (code < KWINTV_COUNT)) {
    return G_kwintv_words[code];
  } else {
    return (char *)NULL;
  }
}

