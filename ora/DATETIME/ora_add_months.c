#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>
#include <ksu_dates.h>

extern void     ora_add_months(sqlite3_context * context, 
                             int               argc, 
                             sqlite3_value  ** argv) {
        KSU_TM_T     tm;
        KSU_TM_T    *ptm;
        KSU_TIME_T   t;
        char        *input_date;
        int          len;
        char         buff[DATE_LEN];
        int          n;
                
        if (ksu_prm_ok(context, argc, argv, "add_months",
                       KSU_PRM_DATETIME, KSU_PRM_INT)) {
          input_date = (char *)sqlite3_value_text(argv[0]);
          if (!ksu_is_datetime((const char *)input_date, 
                               &t, (char)0)
              || !ksu_localtime(t, &tm)) {
            ksu_err_msg(context, KSU_ERR_INV_DATE, input_date, "add_months");
            return;
          } 
          len = strlen(input_date);
          n = sqlite3_value_int(argv[1]);
          t = ksu_add_months(&tm, n);
          if ((ptm = ksu_localtime(t, &tm)) != (KSU_TM_T *)NULL) {
            if (len > 15) {
              sprintf(buff, "%4hd-%02hd-%02hd %02hd:%02hd:%02hd",
                            ptm->year, (short)(ptm->mon + 1), ptm->mday,
                            ptm->hour, ptm->min, ptm->sec);
            } else {
              sprintf(buff, "%4hd-%02hd-%02hd",
                            ptm->year, (short)(ptm->mon + 1), ptm->mday);
            }
            sqlite3_result_text(context, buff, -1, SQLITE_TRANSIENT);
          } else {
            ksu_err_msg(context, KSU_ERR_INV_DATE, input_date, "add_months");
          }
        } 
} 
