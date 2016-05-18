#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ksu_common.h>
#include <ksu_ora.h>

#define UNIT_SZ      20
#define INTERVAL_SZ  15

extern void ora_numtoyminterval(sqlite3_context * context, 
                                int               argc, 
                                sqlite3_value  ** argv) {
   char  unit[UNIT_SZ];
   char  interval[INTERVAL_SZ];
   char *w;
   int   len;
   int   y;
   int   m;
   char  neg;

   if (ksu_prm_ok(context, argc, argv, "numtoyminterval",
                  KSU_PRM_NUMERIC, KSU_PRM_TEXT)) {
      double  val = sqlite3_value_double(argv[0]);
      neg = (val < 0);
      if (neg) {
         val = -1 * val;
      }
      strncpy(unit, (char *)sqlite3_value_text(argv[1]), UNIT_SZ);
      w = unit;
      while (isspace(*w)) {
        w++;
      }
      len = strlen(w);
      while (len && isspace(w[len-1])) {
        len--;
      }
      w[len] = '\0';
      switch(oratim_search(w)) {
         case ORATIM_YEAR :
              y = (int)(val*12 + 0.5) / 12;
              m = (int)(val*12 + 0.5) % 12;
              break;
         case ORATIM_MONTH:
              y = (int)(val + 0.5) / 12;
              m = (int)(val + 0.5) % 12;
              break;
         default :
              ksu_err_msg(context, KSU_ERR_INV_TIME_UNIT,
                          w, "numtoyminterval");         
              return;
      }
      sprintf(interval, "%c%09d-%02d", (neg?'-':'+'), y, m);
      sqlite3_result_text(context, interval, -1, SQLITE_TRANSIENT);
    } 
} 
