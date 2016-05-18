#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_pg.h>

extern void pg_date_part(sqlite3_context  *context,
                         int               argc,
                         sqlite3_value   **argv) {
   int   ret;
   int   part_code;
   int   ksu_code;
   char *arg1;
   char *arg2;
   long  result;

   _ksu_null_if_null_param(argc, argv);
   arg1 = (char *)sqlite3_value_text(argv[0]);
   arg2 = (char *)sqlite3_value_text(argv[1]);
   part_code = pgtim_search(arg1);
   // Convert Postgres-specific codes to our generic codes
   switch (part_code) {
     case PGTIM_NOT_FOUND:
          ksu_err_msg(context, KSU_ERR_INV_TIME_UNIT,
                      arg1, "date_part");
          return;
     case PGTIM_C:
     case PGTIM_CENTURY:
     case PGTIM_CENTURIES:
          ksu_code = KSU_DATE_PART_CENTURY;
          break;
     case PGTIM_D:
     case PGTIM_DAY:
     case PGTIM_DAYS:
          ksu_code = KSU_DATE_PART_DAY;
          break;
     case PGTIM_DECADE:
     case PGTIM_DECADES:
          ksu_code = KSU_DATE_PART_DECADE;
          break;
     case PGTIM_DOW:
          ksu_code = KSU_DATE_PART_WEEKDAY;
          break;
     case PGTIM_DOY:
          ksu_code = KSU_DATE_PART_DAYOFYEAR;
          break;
     case PGTIM_EPOCH:
          ksu_code = KSU_DATE_PART_EPOCH;
          break;
     case PGTIM_H:
     case PGTIM_HOUR:
     case PGTIM_HOURS:
          ksu_code = KSU_DATE_PART_HOUR;
          break;
     case PGTIM_ISODOW:
          ksu_code = KSU_DATE_PART_ISOWEEKDAY;
          break;
     case PGTIM_ISOYEAR:
          ksu_code = KSU_DATE_PART_ISOYEAR;
          break;
     case PGTIM_MILLENNIUM:
          ksu_code = KSU_DATE_PART_MILLENNIUM;
          break;
     case PGTIM_M:
     case PGTIM_MIN:
     case PGTIM_MINS:
     case PGTIM_MINUTE:
     case PGTIM_MINUTES:
          ksu_code = KSU_DATE_PART_MINUTE;
          break;
     case PGTIM_MON:
     case PGTIM_MONS:
     case PGTIM_MONTH:
     case PGTIM_MONTHS:
          ksu_code = KSU_DATE_PART_MONTH;
          break;
     case PGTIM_QUARTER:
          ksu_code = KSU_DATE_PART_QUARTER;
          break;
     case PGTIM_S:
     case PGTIM_SEC:
     case PGTIM_SECS:
     case PGTIM_SECOND:
     case PGTIM_SECONDS:
          ksu_code = KSU_DATE_PART_SECOND;
          break;
     case PGTIM_W:
     case PGTIM_WEEK:
     case PGTIM_WEEKS:
          ksu_code = KSU_DATE_PART_ISOWEEK;
          break;
     case PGTIM_Y:
     case PGTIM_YEAR:
     case PGTIM_YEARS:
          ksu_code = KSU_DATE_PART_YEAR;
          break;
     default:
          // Legit PostgreSQL date part that isn't
          // supported (fraction of a second, timezone ...)
          sqlite3_result_null(context);
          return;
   }
   ret = ksu_extract_dt_int((const char *)arg2,
                            ksu_code, &result);
   switch (ret) {
     case -2: // Invalid date part (should already have been
              // screened out)
          ksu_err_msg(context, KSU_ERR_INV_TIME_UNIT, 
                      arg1, "date_part");
          break;
     case -1: // Invalid date, interval or time
          ksu_err_msg(context,
                      KSU_ERR_N_INV_DATATYPE , 
                      2, "date_part");
          break;
     case 0:  // Irrelevant date part for the other arg
          ksu_err_msg(context, KSU_ERR_INV_TIME_UNIT, 
                      arg1, "date_part");
          break;
     case 1:  // OK
          sqlite3_result_int64(context, result);
          break;
     default: // Should not happen
          sqlite3_result_null(context);
          break;
   }
}

