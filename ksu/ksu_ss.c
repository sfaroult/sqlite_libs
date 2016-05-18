#include <stdio.h>
#include <string.h>

#include "ksu_ss.h"

static char *G_ksu_ss_words[] = {
    "D",
    "DAY",
    "DAYOFYEAR",
    "DD",
    "DW",
    "DY",
    "HH",
    "HOUR",
    "ISOWK",
    "ISOWW",
    "ISO_WEEK",
    "M",
    "MCS",
    "MI",
    "MICROSECOND",
    "MILLISECOND",
    "MINUTE",
    "MM",
    "MONTH",
    "MS",
    "N",
    "NANOSECOND",
    "NS",
    "Q",
    "QQ",
    "QUARTER",
    "S",
    "SECOND",
    "SS",
    "W",
    "WEEK",
    "WEEKDAY",
    "WK",
    "WW",
    "Y",
    "YEAR",
    "YY",
    "YYYY",
    NULL};

extern int ksu_ss_search(char *w) {
  int start = 0;
  int end = KSU_SS_COUNT - 1;
  int mid;
  int pos = KSU_SS_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_ksu_ss_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
      } else if ((mid < KSU_SS_COUNT)
                 && ((comp = strcasecmp(G_ksu_ss_words[mid+1], w)) == 0)) {
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

extern char *ksu_ss_keyword(int code) {
  if ((code >= 0) && (code < KSU_SS_COUNT)) {
    return G_ksu_ss_words[code];
  } else {
    return (char *)NULL;
  }
}

static char *G_sstyp_words[] = {
    "bigint",
    "binary",
    "bit",
    "char",
    "date",
    "datetime",
    "datetime2",
    "datetimeoffset",
    "decimal",
    "float",
    "hierarchyid",
    "image",
    "int",
    "money",
    "nchar",
    "ntext",
    "numeric",
    "nvarchar",
    "real",
    "smalldatetime",
    "smallint",
    "smallmoney",
    "sql_variant",
    "text",
    "time",
    "tinyint",
    "uniqueidentifier",
    "varbinary",
    "varchar",
    "xml",
    NULL};

extern int sstyp_search(char *w) {
  int start = 0;
  int end = SSTYP_COUNT - 1;
  int mid;
  int pos = SSTYP_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_sstyp_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
       } else if ((mid < SSTYP_COUNT)
               && ((comp = strcasecmp(G_sstyp_words[mid+1], w)) == 0)) {
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

extern char *sstyp_keyword(int code) {
  if ((code >= 0) && (code < SSTYP_COUNT)) {
    return G_sstyp_words[code];
  } else {
    return (char *)NULL;
  }
}

