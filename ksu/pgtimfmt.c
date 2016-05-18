#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "pgtimfmt.h"

static char *G_pgtimfmt_words[] = {
    "YYYY",
    "YYY",
    "YY",
    "Y,YYY",
    "Y",
    "WW",
    "W",
    "US",
    "TZ",
    "TM",
    "TH",
    "SSSS",
    "SS",
    "SP",
    "RM",
    "Q",
    "PM",
    "P.M.",
    "OF",
    "MS",
    "MONTH",
    "MON",
    "MM",
    "MI",
    "J",
    "IYYY",
    "IYY",
    "IY",
    "IW",
    "IDDD",
    "ID",
    "I",
    "HH24",
    "HH12",
    "HH",
    "FX",
    "FM",
    "DY",
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

extern int pgtimfmt_search(char *w) {
  int start = 0;
  int end = PGTIMFMT_COUNT - 1;
  int mid;
  int pos = PGTIMFMT_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_pgtimfmt_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
       } else if ((mid < PGTIMFMT_COUNT)
               && ((comp = strcasecmp(G_pgtimfmt_words[mid+1], w)) == 0)) {
         pos = mid+1;
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

extern int pgtimfmt_best_match(char *w) {
  int pos = PGTIMFMT_NOT_FOUND;
  int i = 0;
  int comp = 1;

  if (w) {
    while ((i < PGTIMFMT_COUNT)
           && ((comp > 0)
               || (toupper(*w) == toupper(*(G_pgtimfmt_words[i]))))) {
      comp = strncasecmp(G_pgtimfmt_words[i],w,strlen(G_pgtimfmt_words[i]));
      i++;
      if (comp == 0) {
        break;
      }
    }
    if (comp == 0) {
      pos = i - 1;
    }
  }
  return pos;
}

extern int pgtimfmt_abbrev(char *w) {
  int pos = PGTIMFMT_NOT_FOUND;
  int i = 0;
  int len;
  int comp = 1;

  if (w) {
    len = strlen(w);
    while ((i < PGTIMFMT_COUNT) && (comp > 0)) {
      comp = strncasecmp(G_pgtimfmt_words[i],w,len);
      i++;
    }
    if (comp == 0) {
      pos = i - 1;
      if ((i < PGTIMFMT_COUNT)
          && (strncasecmp(G_pgtimfmt_words[i],w,len)==0)) {
         pos = PGTIMFMT_AMBIGUOUS;
      }
    }
  }
  return pos;
}

extern char *pgtimfmt_keyword(int code) {
  if ((code >= 0) && (code < PGTIMFMT_COUNT)) {
    return G_pgtimfmt_words[code];
  } else {
    return (char *)NULL;
  }
}

