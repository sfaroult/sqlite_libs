#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

static double date_to_double(char *dat, int *ok_ptr) {
    double  ret = -1;
    time_t  t;
    int     y;
    int     m;
    int     d;
    int     h = 0;
    int     mi = 0;
    int     s = 0;
    char    conv = 0;

    *ok_ptr = 0;
    if (dat) { 
       int len = strlen(dat);
       if ((len == 19) || (len == 10)) {
          if (len == 19) {
             if (sscanf(dat, "%4d-%02d-%02d %02d:%02d:%02d",
                           &y, &m, &d, &h, &mi, &s) == 6) {
                conv = 1;
             }
          } else {
             if (sscanf(dat, "%4d-%02d-%02d", &y, &m, &d) == 3) {
                conv = 1;
             }
          }
          if (conv) {
             struct tm stm;

             (void)memset(&stm, 0, sizeof(struct tm));
             stm.tm_year = y - 1900;
             stm.tm_mon = m - 1;
             stm.tm_mday = d;
             stm.tm_hour = h;
             stm.tm_min = mi;
             stm.tm_sec = s;
             t = mktime(&stm);
             if (t != (time_t)-1) {
                *ok_ptr = 1;
                ret = (double)t;
             }
          }
       }
    }
    return ret;
}

extern void     ora_width_bucket(sqlite3_context * context,
                                 int               argc,
                                 sqlite3_value  ** argv) {
        int             typ;
        double          exp;
        double          min;
        double          max;
        double          count;
        double          width;
        int             bc = 1;
        double          i;

        _ksu_null_if_null_param(argc, argv);
        // exp can be numeric or a datetime,
        // min and max must be of the same type as exp,
        // count must be a positive integer.
        typ = sqlite3_value_type(argv[3]);
        if (typ != SQLITE_INTEGER) {
           ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT_GT_0,
                                4, "width_bucket");
           return;
        }
        count = sqlite3_value_int(argv[3]);
        if (count <= 0) {
           ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT_GT_0,
                                4, "width_bucket");
           return;
        }
        typ = sqlite3_value_type(argv[0]);
        if ((typ == SQLITE_INTEGER)
            || (typ == SQLITE_FLOAT)) {
           exp = sqlite3_value_double(argv[0]);
           typ = sqlite3_value_type(argv[1]);
           if ((typ != SQLITE_INTEGER)
               && (typ != SQLITE_FLOAT)) {
              ksu_err_msg(context, KSU_ERR_ARG_N_NOT_NUM,
                                   2, "width_bucket");
              return;
           }
           min = sqlite3_value_double(argv[1]);
           typ = sqlite3_value_type(argv[2]);
           if ((typ != SQLITE_INTEGER)
               && (typ != SQLITE_FLOAT)) {
              ksu_err_msg(context, KSU_ERR_ARG_N_NOT_NUM,
                                   3, "width_bucket");
              return;
           }
           max = sqlite3_value_double(argv[2]);
        } else {
           // Check if we have dates. If that is the case,
           // convert them to double values (actually Unix timestamps)
           int ok = 0;
           exp = date_to_double((char *)sqlite3_value_text(argv[0]), &ok);
           if (ok) {
              min = date_to_double((char *)sqlite3_value_text(argv[1]), &ok);
              if (ok) {
                 max = date_to_double((char *)sqlite3_value_text(argv[2]), &ok);
                 if (!ok) {
                    ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                                3, "width_bucket");
                    return;
                 }
              } else {
                 ksu_err_msg(context, KSU_ERR_ARG_N_NOT_DATETIME,
                             2, "width_bucket");
                 return;
              }
           } else {
              ksu_err_msg(context, KSU_ERR_ARG_N_NOT_NUM_OR_DAT,
                          1, "width_bucket");
              return;
           }
        }
        width = (max - min) / count;

        if (exp < min) {
           sqlite3_result_int(context, 0);
        } else if (exp > max) {
           sqlite3_result_int(context, count + 1);
        } else {
           i = min;
           while (i < max) {
              if (exp >= i && exp < i + width) {
                 sqlite3_result_int(context, bc);
                 break;
              }
              i += width;
              bc++;
           }
        }
}
