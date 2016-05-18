#include <stdio.h>
#include <string.h>

#include "oranum.h"

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

