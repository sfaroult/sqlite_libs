#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include <ksu_common.h>

#define INTERVAL_SZ  15

// [-] P  [ years Y  ] [months M ] [days D ] [ T  [hours H ] [minutes M ] [seconds [. frac_secs] S  ] ]
static int iso_extract(char *iso, int *yptr, int *mptr) {

    char *p = iso;
    char *q = iso;
    int   assigned = -1;
    char  neg = 0;
    char  markers[] = "PYMDTHM.S";
    char  digits = 0;
    short mk = 0;

    if (p) {
      if (*p == '-') {
        neg = 1;
        p++;
      }
      if (*p == 'P') {
        mk = 1;
        p++;
        q = p;
        while (*p) {
          if (isdigit(*p)) {
            digits = 1;
          } else {
            if (digits
                && ((*p == markers[mk])
                   || ((markers[mk] == '.') && (*p == 'S')))) {
              digits = 0;
              mk++;
            } else {
              // Invalid format
              return -1;
            }
          }
          p++;
        }
        assigned = sscanf(q, "%dY%d", yptr, mptr);
        if (neg) {
          *yptr *= -1; 
          *mptr *= -1; 
        }
      }
    }
    return assigned;
}

extern void ora_to_yminterval(sqlite3_context * context,
                              int               argc,
                              sqlite3_value  ** argv) {
        int   years;
        int   months;
        char *v;
        int   m;
        char  interval[INTERVAL_SZ];
        char  neg = 0;

        _ksu_null_if_null_param(argc, argv);
        v = (char *)sqlite3_value_text(argv[0]);
        if ((*v == 'P') || (strncmp(v, "-P", 2) == 0)) {
          // Iso interval
          if (2 == iso_extract(v, &years, &months)) {
            // Number of months may be > 11 in the iso format
            m = (months + (12 * years));
            if (m < 0) {
              neg = 1;
              m *= -1;
            }
            years = m / 12;
            months = m - years * 12;
          } else {
            // Invalid
            ksu_err_msg(context, KSU_ERR_ARG_NOT_INTV, "to_yminterval");
            return;
          }
        } else {
          // SQL interval
          if (*v == '-') {
             v++;
             neg = 1;
          }
          if (!isdigit(*v)) {
            ksu_err_msg(context, KSU_ERR_ARG_NOT_INTV, "to_yminterval");
            return;
          }
          if (sscanf(v, "%d-%d", &years, &months) == 2) {
            if ((months < 0) || (months > 11)) {
              // Invalid
              ksu_err_msg(context, KSU_ERR_ARG_NOT_INTV, "to_yminterval");
              return;
            }
          } else {
            ksu_err_msg(context, KSU_ERR_ARG_NOT_INTV, "to_yminterval");
            return;
          }
        }
        sprintf(interval, "%c%09d-%02d", (neg?'-':'+'), years, months);
        sqlite3_result_text(context, interval, -1, SQLITE_TRANSIENT);
}
