#include <stdio.h>
#include <string.h>

#include "pgspecial.h"

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

