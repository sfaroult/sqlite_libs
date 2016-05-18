#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <ksu_pg.h>

static char *G_pgspecial_words[] = {
    "+infinity",
    "-infinity",
    "allballs",
    "epoch",
    "infinity",
    "now",
    "today",
    "tomorrow",
    "yesterday",
    NULL};

extern int pgspecial_search(char *w) {
  int start = 0;
  int end = PGSPECIAL_COUNT - 1;
  int mid;
  int pos = PGSPECIAL_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_pgspecial_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
      } else if ((mid < PGSPECIAL_COUNT)
                 && ((comp = strcasecmp(G_pgspecial_words[mid+1], w)) == 0)) {
         pos = mid+1;
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

extern char *pgspecial_keyword(int code) {
  if ((code >= 0) && (code < PGSPECIAL_COUNT)) {
    return G_pgspecial_words[code];
  } else {
    return (char *)NULL;
  }
}

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
      } else if ((mid < PGTIM_COUNT)
                 && ((comp = strcasecmp(G_pgtim_words[mid+1], w)) == 0)) {
         pos = mid+1;
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

static char *G_pgnum_words[] = {
    "TM",
    "TH",
    "SG",
    "S",
    "PR",
    "PL",
    "MI",
    "FM",
    "EEEE",
    NULL};

extern int pgnum_search(char *w) {
  int start = 0;
  int end = PGNUM_COUNT - 1;
  int mid;
  int pos = PGNUM_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_pgnum_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
       } else if ((mid < PGNUM_COUNT)
               && ((comp = strcasecmp(G_pgnum_words[mid+1], w)) == 0)) {
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

extern int pgnum_best_match(char *w) {
  int pos = PGNUM_NOT_FOUND;
  int i = 0;
  int comp = 1;

  if (w) {
    while ((i < PGNUM_COUNT)
           && ((comp > 0)
               || (toupper(*w) == toupper(*(G_pgnum_words[i]))))) {
      comp = strncasecmp(G_pgnum_words[i],w,strlen(G_pgnum_words[i]));
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

extern int pgnum_abbrev(char *w) {
  int pos = PGNUM_NOT_FOUND;
  int i = 0;
  int len;
  int comp = 1;

  if (w) {
    len = strlen(w);
    while ((i < PGNUM_COUNT) && (comp > 0)) {
      comp = strncasecmp(G_pgnum_words[i],w,len);
      i++;
    }
    if (comp == 0) {
      pos = i - 1;
      if ((i < PGNUM_COUNT)
          && (strncasecmp(G_pgnum_words[i],w,len)==0)) {
         pos = PGNUM_AMBIGUOUS;
      }
    }
  }
  return pos;
}

extern char *pgnum_keyword(int code) {
  if ((code >= 0) && (code < PGNUM_COUNT)) {
    return G_pgnum_words[code];
  } else {
    return (char *)NULL;
  }
}

