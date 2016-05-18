#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ksu_common.h>
#include <ksu_dates.h>

extern void my_curtime(sqlite3_context  *context,
                       int argc,
                       sqlite3_value   **argv) {
    KSU_TIME_T   t;
    KSU_TM_T     tm;
    KSU_TM_T    *ptm;
    char         date[DATE_LEN];

    t = ksu_currenttime((char)0);
    if ((ptm = ksu_localtime(t, &tm)) != (KSU_TM_T *)NULL) {
      sprintf(date, "%02hd:%02hd:%02hd",
                    ptm->hour,
                    ptm->min,
                    ptm->sec);
      sqlite3_result_text(context, date, -1, SQLITE_TRANSIENT);
    } else {
       sqlite3_result_null(context);
    }
}
