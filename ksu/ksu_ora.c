#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <ksu_ora.h>

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
      } else if ((mid < ORATIM_COUNT)
                 && ((comp = strcasecmp(G_oratim_words[mid+1], w)) == 0)) {
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

extern int oratim_best_match(char *w) {
  int pos = ORATIM_NOT_FOUND;
  int i = 0;
  int comp = 1;

  if (w) {
    while ((i < ORATIM_COUNT)
            && ((comp > 0)
                || (toupper(*w) == toupper(*(G_oratim_words[i]))))) {
      /*
      printf("Comparing %s to %s (len: %d)\n", w, G_oratim_words[i],
             (int)strlen(G_oratim_words[i]));
      */
      comp = strncasecmp(G_oratim_words[i],w,strlen(G_oratim_words[i]));
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

static char *G_oranum_words[] = {
    "rn",
    "TM",
    "RN",
    "PR",
    "MI",
    "FM",
    "EEEE",
    NULL};

extern int oranum_search(char *w) {
  int start = 0;
  int end = ORANUM_COUNT - 1;
  int mid;
  int pos = ORANUM_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_oranum_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
      } else if ((mid < ORANUM_COUNT)
                 && ((comp = strcasecmp(G_oranum_words[mid+1], w)) == 0)) {
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

extern int oranum_best_match(char *w) {
  int pos = ORANUM_NOT_FOUND;
  int i = 0;
  int comp = 1;

  if (w) {
    while ((i < ORANUM_COUNT) && (comp > 0)) {
      comp = strncasecmp(G_oranum_words[i],w,strlen(G_oranum_words[i]));
      i++;
    }
    if (comp == 0) {
      pos = i - 1;
    }
  }
  return pos;
}

extern int oranum_abbrev(char *w) {
  int pos = ORANUM_NOT_FOUND;
  int i = 0;
  int len;
  int comp = 1;

  if (w) {
    len = strlen(w);
    while ((i < ORANUM_COUNT) && (comp > 0)) {
      comp = strncasecmp(G_oranum_words[i],w,len);
      i++;
    }
    if (comp == 0) {
      pos = i - 1;
      if ((i < ORANUM_COUNT)
          && (strncasecmp(G_oranum_words[i],w,len)==0)) {
         pos = ORANUM_AMBIGUOUS;
      }
    }
  }
  return pos;
}

extern char *oranum_keyword(int code) {
  if ((code >= 0) && (code < ORANUM_COUNT)) {
    return G_oranum_words[code];
  } else {
    return (char *)NULL;
  }
}

