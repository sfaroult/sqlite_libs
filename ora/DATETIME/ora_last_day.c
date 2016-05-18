#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>

extern void     ora_last_day(sqlite3_context * context, 
                             int               argc, 
                             sqlite3_value  ** argv) {
        int             year;
        int             month;
        int             day;
        char           *input_date;
                
        if (ksu_prm_ok(context, argc, argv, "last_day", KSU_PRM_DATETIME)) {
          input_date = (char *)sqlite3_value_text(argv[0]);
          if (3 != sscanf(input_date, "%d-%d-%d", &year, &month, &day)) {
            ksu_err_msg(context, KSU_ERR_INV_DATE, input_date, "last_day");
            return;
          } 
        
          int common_year = 1;
          if (year >= 1582) {
            // Gregorian calendar
            if (year % 4 == 0) {
              if (year % 100 != 0) {
                common_year = 0;
              } else {
                if (year % 400 == 0) {
                  common_year = 0;
                } 
              } 
            } 
          } else {
            // Before 1582: Julian calendar
            if (year % 4 == 0) {
              common_year = 0;
            } 
          } 
          if (month == 1 || month == 3 || month == 5 || month == 7
                         || month == 8 || month == 10 || month == 12) {
            day = 31;
          } else if (month == 4 || month == 6 || month == 9 || month == 11) {
            day = 30;
          } else {
            if (!common_year) {
              day = 29;
            } else {
              day = 28;
            }
          } 
          char date[15];
          (void)sprintf((char *)date, "%4d-%02d-%02d", year, month, day);
          sqlite3_result_text(context, date, -1, SQLITE_TRANSIENT);
        } 
} 
