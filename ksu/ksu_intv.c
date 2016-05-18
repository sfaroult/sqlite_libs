#include <stdio.h>
#include <string.h>

#include "ksu_intv.h"

static char *G_ksu_intv_words[] = {
    "century",
    "day",
    "hour",
    "minute",
    "month",
    "second",
    "week",
    "year",
    NULL};

extern int ksu_intv_search(char *w) {
  int start = 0;
  int end = KSU_INTV_COUNT - 1;
  int mid;
  int pos = KSU_INTV_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_ksu_intv_words[mid], w)) == 0) {
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

extern char *ksu_intv_keyword(int code) {
  if ((code >= 0) && (code < KSU_INTV_COUNT)) {
    return G_ksu_intv_words[code];
  } else {
    return (char *)NULL;
  }
}

