#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include <ksu_common.h>
#include <ksu_pg.h>
#include <ksu_dates.h>

extern void pg_extract(sqlite3_context *context,
                       int              argc,
                       sqlite3_value  **argv) {
   KSU_TIME_T t;
   KSU_INTV_T intv;
   char      *date;
   char      *piece;
   long       result = 0;
   int        pieceToExtract;
   int        what;
   int        ret;
   char       is_intv = 0;  // flag

   _ksu_null_if_null_param(argc, argv);
   date = (char *)sqlite3_value_text(argv[1]);
   if (!ksu_is_date((const char *)date, &t)) {
     if (!ksu_is_interval((const char *)date, &intv)) {
       ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                   2, "extract");
       return;
     } else {
       is_intv = 1;
    }
   }
   piece = (char *)sqlite3_value_text(argv[0]);

   //Integer representing the piece of the date
   //to be extracted
   pieceToExtract = pgtim_search(piece);
   // Switch statement to pull relevant info
   switch (pieceToExtract) {
     case PGTIM_CENTURY:
          what = KSU_DATE_PART_CENTURY;
          break;
     case PGTIM_DAY:
          what = KSU_DATE_PART_DAY;
          break;
     case PGTIM_DECADE:
          what = KSU_DATE_PART_DECADE;
          break;
     case PGTIM_DOW:
          what = KSU_DATE_PART_WEEKDAY;
          break;
     case PGTIM_DOY:
          what = KSU_DATE_PART_DAYOFYEAR;
          break;
     case PGTIM_EPOCH:
          what = KSU_DATE_PART_EPOCH;
          break;
     case PGTIM_HOUR:
          what = KSU_DATE_PART_HOUR;
          break;
     case PGTIM_ISODOW:
          what = KSU_DATE_PART_ISOWEEKDAY;
          break;
     case PGTIM_ISOYEAR:
          what = KSU_DATE_PART_ISOYEAR;
          break;
     case PGTIM_MILLENNIUM:
          what = KSU_DATE_PART_MILLENNIUM;
          break;
     case PGTIM_MINUTE:
          what = KSU_DATE_PART_MINUTE;
          break;
     case PGTIM_MONTH:
          what = KSU_DATE_PART_MONTH;
          break;
     case PGTIM_QUARTER:
          what = KSU_DATE_PART_QUARTER;
          break;
     case PGTIM_SECOND:
          what = KSU_DATE_PART_SECOND;
          break;
     case PGTIM_MICROSECONDS:
     case PGTIM_MILLISECONDS:
     case PGTIM_TIMEZONE:
     case PGTIM_TIMEZONE_HOUR:
     case PGTIM_TIMEZONE_MINUTE:
          sqlite3_result_null(context);
          return;
     case PGTIM_WEEK:
          what = KSU_DATE_PART_WEEK;
          break;
     case PGTIM_YEAR:
          what = KSU_DATE_PART_YEAR;
          break;
     default:
          ksu_err_msg(context, KSU_ERR_INV_TIME_UNIT, piece, "extract");
          return;
   }
   ret = ksu_extract_dt_int((const char *)date, what, &result);
   switch (ret) {
      case 1 : // OK
           sqlite3_result_int(context, result);
           break;  
      case -1 :
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
