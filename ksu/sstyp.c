#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "sstyp.h"

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

