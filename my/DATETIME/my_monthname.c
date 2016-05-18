#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <langinfo.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_my.h>

extern void my_monthname(sqlite3_context * context,
                         int               argc,
                         sqlite3_value  ** argv) {
    KSU_TM_T    tm;
    KSU_TM_T   *ptm;
    KSU_TIME_T  t;
    int         codes[12] = {MON_1, MON_2, MON_3,
                             MON_4, MON_5, MON_6,
                             MON_7, MON_8, MON_9,
                             MON_10, MON_11, MON_12};

    if (my_is_datetime((char *)sqlite3_value_text(argv[0]),
                       &t, (char)0)) {
      ptm = ksu_localtime(t, &tm);
      sqlite3_result_text(context,
                          nl_langinfo(codes[tm.mon]),
                          -1, NULL);
    } else {
      sqlite3_result_null(context); 
    }
} //end of my_monthname
