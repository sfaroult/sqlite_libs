#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "pgnum.h"

static char *G_pgnum_words[] = {
    "TM",
    "PR",
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

