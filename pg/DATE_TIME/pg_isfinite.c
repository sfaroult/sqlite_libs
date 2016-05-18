#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ksu_common.h>
#include <ksu_dates.h>
#include <ksu_pg.h>

//Determines if the date is finite(not + /-infinity) 
extern void pg_isfinite(sqlite3_context * context,
                        int               argc,
                        sqlite3_value  ** argv) {
  char      *s;
  int        ret;
  KSU_TIME_T t;
  KSU_INTV_T intv;
        
  _ksu_null_if_null_param(argc, argv);
  s = (char *)sqlite3_value_text(argv[0]);
  if (ksu_is_datetime((const char *)s, &t, 0)) {
    ret = !(t.flags & KSU_TIME_INFINITE);
  } else if (ksu_is_interval((const char *)s, &intv)) {
    ret = 1;
  } else {
    ksu_err_msg(context, KSU_ERR_INV_DATATYPE, "isfinite");
    return;
  }
  sqlite3_result_int(context, ret);
} 
