#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <langinfo.h>

#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_my.h>

extern void my_dayname(sqlite3_context * context,
                       int               argc,
                       sqlite3_value  ** argv) {
    KSU_TM_T    tm;
    KSU_TM_T   *ptm;
    KSU_TIME_T  t;
    int         codes[12] = {DAY_1, DAY_2, DAY_3,
                             DAY_4, DAY_5, DAY_6, DAY_7};

    if (my_is_datetime((char *)sqlite3_value_text(argv[0]),
                       &t, (char)0)) {
      ptm = ksu_localtime(t, &tm);
      sqlite3_result_text(context,
                          nl_langinfo(codes[tm.wday]),
                          -1, NULL);
    } else {
      sqlite3_result_null(context); 
    }
} //end of my_dayname
