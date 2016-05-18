/*
 * Title: ora_extract.c
 * History:
 *              March 25, 2015: Date Created
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include <ksu_common.h>
#include <ksu_dates.h>

#define EXTRACT_NOT_FOUND       -1
#define EXTRACT_DAY       0
#define EXTRACT_HOUR      1
#define EXTRACT_MINUTE    2
#define EXTRACT_MONTH     3
#define EXTRACT_SECOND    4
#define EXTRACT_TIMEZONE_ABBR     5
#define EXTRACT_TIMEZONE_HOUR     6
#define EXTRACT_TIMEZONE_MINUTE   7
#define EXTRACT_TIMEZONE_REGION   8
#define EXTRACT_YEAR      9

#define EXTRACT_COUNT   10

static char *G_extract_words[] = {
   "DAY",
   "HOUR",
   "MINUTE",
   "MONTH",
   "SECOND",
   "TIMEZONE_ABBR",
   "TIMEZONE_HOUR",
   "TIMEZONE_MINUTE",
   "TIMEZONE_REGION",
   "YEAR",
   NULL
};

static int extract_search(char *w) {
   int start = 0;
   int end = EXTRACT_COUNT - 1;
   int mid;
   int pos = EXTRACT_NOT_FOUND;
   int comp;
   while (start <= end) {
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_extract_words[mid], w)) == 0) {
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
   return pos;
}

extern void ora_extract(sqlite3_context *context,
                        int              argc,
                        sqlite3_value  **argv) {
   KSU_TIME_T t;
   char      *date;
   char      *piece;
   long       result = 0;
   int        pieceToExtract;
   int        what;
   int        ret;

   _ksu_null_if_null_param(argc, argv);
   date = (char *)sqlite3_value_text(argv[1]);
   if (!ksu_is_date((const char *)date, &t)) {
      ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                  2, "extract");
      return;
   }
   piece = (char *)sqlite3_value_text(argv[0]);

   //Integer representing the piece of the date
   //to be extracted
   pieceToExtract = extract_search(piece);
   // Switch statement to pull relevant info
   switch (pieceToExtract) {
     case EXTRACT_DAY:
          what = KSU_DATE_PART_DAY;
          break;
     case EXTRACT_HOUR:
          what = KSU_DATE_PART_HOUR;
          break;
     case EXTRACT_MINUTE:
          what = KSU_DATE_PART_MINUTE;
          break;
     case EXTRACT_MONTH:
          what = KSU_DATE_PART_MONTH;
          break;
     case EXTRACT_SECOND:
          what = KSU_DATE_PART_SECOND;
          break;
     case EXTRACT_YEAR:
          what = KSU_DATE_PART_YEAR;
          break;
     case EXTRACT_TIMEZONE_ABBR:
     case EXTRACT_TIMEZONE_HOUR:
     case EXTRACT_TIMEZONE_MINUTE:
     case EXTRACT_TIMEZONE_REGION:
          sqlite3_result_null(context);
          return;
     default:
          ksu_err_msg(context, KSU_ERR_INV_TIME_UNIT, piece, "extract");
          return;
   }
   ret = ksu_extract_int(t, what, &result);
   switch (ret) {
      case 1 : // OK
           sqlite3_result_int(context, result);
           break;  
      case -1 : // Should not occur
           ksu_err_msg(context, KSU_ERR_INV_DATE, date, "extract");
           break;  
      case -2 : // Should not occur
           ksu_err_msg(context, KSU_ERR_INV_TIME_UNIT, piece, "extract");
           break;  
      default : // Can only be 0, valid date part but irrelevant for
                // the date supplied (extracting hour from a pure date,
                // for instance)
           ksu_err_msg(context, KSU_ERR_INV_XTRACT_FOR_SRC, "extract");
           break;  
   }
}
