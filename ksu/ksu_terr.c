#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ksu_terr.h"

static char *G_ksu_terr_words[] = {
    "AM",
    "AT",
    "AU",
    "BE",
    "BG",
    "BR",
    "BY",
    "CA",
    "CH",
    "CN",
    "CZ",
    "DE",
    "DK",
    "EE",
    "ES",
    "ET",
    "FI",
    "FR",
    "GB",
    "GR",
    "HK",
    "HR",
    "HU",
    "IE",
    "IL",
    "IN",
    "IS",
    "IT",
    "JP",
    "KR",
    "KZ",
    "LT",
    "NL",
    "NO",
    "NZ",
    "PL",
    "PT",
    "RO",
    "RU",
    "SE",
    "SI",
    "SK",
    "TR",
    "TW",
    "UA",
    "US",
    "YU",
    "ZA",
    NULL};

extern int ksu_terr_search(char *w) {
  int start = 0;
  int end = KSU_TERR_COUNT - 1;
  int mid;
  int pos = KSU_TERR_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_ksu_terr_words[mid], w)) == 0) {
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

extern char *ksu_terr_keyword(int code) {
  if ((code >= 0) && (code < KSU_TERR_COUNT)) {
    return G_ksu_terr_words[code];
  } else {
    return (char *)NULL;
  }
}

