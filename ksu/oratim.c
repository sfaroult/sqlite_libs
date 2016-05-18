#include <stdio.h>
#include <string.h>

#include "oratim.h"

static char *G_oratim_words[] = {
    "YYYY",
    "YYY",
    "YY",
    "YEAR",
    "Y,YYY",
    "Y",
    "X",
    "WW",
    "W",
    "TZR",
    "TZM",
    "TZH",
    "TZD",
    "TS",
    "THSP",
    "TH",
    "SYYYY",
    "SYEAR",
    "SSSSS",
    "SS",
    "SPTH",
    "SP",
    "SCC",
    "RRRR",
    "RR",
    "RM",
    "Q",
    "PM",
    "P.M.",
    "MONTH",
    "MON",
    "MM",
    "MI",
    "J",
    "IYYY",
    "IYY",
    "IY",
    "IW",
    "I",
    "HH24",
    "HH12",
    "HH",
    "FX",
    "FM",
    "FF",
    "EE",
    "E",
    "DY",
    "DS",
    "DL",
    "DDD",
    "DD",
    "DAY",
    "D",
    "CC",
    "BC",
    "B.C.",
    "AM",
    "AD",
    "A.M.",
    "A.D.",
    NULL};

extern int oratim_search(char *w) {
  int start = 0;
  int end = ORATIM_COUNT - 1;
  int mid;
  int pos = ORATIM_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_oratim_words[mid], w)) == 0) {
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

extern int oratim_best_match(char *w) {
  int pos = ORATIM_NOT_FOUND;
  int i = 0;
  int comp = 1;

  if (w) {
    while ((i < ORATIM_COUNT) && (comp > 0)) {
      comp = strncasecmp(G_oratim_words[i],w,strlen(G_oratim_words[i]));
      i++;
    }
    if (comp == 0) {
      pos = i - 1;
    }
  }
  return pos;
}

extern int oratim_abbrev(char *w) {
  int pos = ORATIM_NOT_FOUND;
  int i = 0;
  int len;
  int comp = 1;

  if (w) {
    len = strlen(w);
    while ((i < ORATIM_COUNT) && (comp > 0)) {
      comp = strncasecmp(G_oratim_words[i],w,len);
      i++;
    }
    if (comp == 0) {
      pos = i - 1;
      if ((i < ORATIM_COUNT)
          && (strncasecmp(G_oratim_words[i],w,len)==0)) {
         pos = ORATIM_AMBIGUOUS;
      }
    }
  }
  return pos;
}

extern char *oratim_keyword(int code) {
  if ((code >= 0) && (code < ORATIM_COUNT)) {
    return G_oratim_words[code];
  } else {
    return (char *)NULL;
  }
}

