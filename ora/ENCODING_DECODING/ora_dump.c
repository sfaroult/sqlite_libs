#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <ksu_common.h>
#include <ksu_dates.h>

#define  TEMP_SZ    10

extern void ora_dump(sqlite3_context * context,
                     int               argc,
                     sqlite3_value  ** argv) {
   int            sz;
   int            charlen;
   unsigned char *p;
   int            typ;
   int            fmt = 10;
   char           charset = 0;
   int            start = 1;
   int            len = -1;
   int            bufsz;
   char          *rslt;
   char           tmp[TEMP_SZ];
   int            i;
   int            j;
   int            k;
   int            n;
   KSU_TIME_T     t;

   _ksu_check_arg_cnt(argc, 1, 4, "dump");
   _ksu_null_if_null_param(argc, argv);
   sz = sqlite3_value_bytes(argv[0]);
   p = (unsigned char *)sqlite3_value_text(argv[0]);
   charlen = ksu_charlen(p);
   if (argc > 1) {
     typ = sqlite3_value_type(argv[1]);
     if (typ != SQLITE_INTEGER) {
       ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 2, "dump");
       return;
     }
     fmt = sqlite3_value_int(argv[1]);
     if (fmt > 1000) {
       charset = 1;
       fmt -= 1000;
     }
     // Quickly check if the format is valid
     switch (fmt) {
       case 8:
       case 10:
       case 16:
       case 17:
            break;
       default:
            ksu_err_msg(context, KSU_ERR_INV_FORMAT, 2, "dump");
            return;
     }
     if (argc > 2) {
       typ = sqlite3_value_type(argv[2]);
       if (typ != SQLITE_INTEGER) {
         ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 3, "dump");
         return;
       }
       start = sqlite3_value_int(argv[2]);
       if ((start < 1) || (start > charlen)) {
         ksu_err_msg(context, KSU_ERR_INV_ARG_VAL_RANGE, "dump");
         return;
       }
       if (argc > 3) {
         typ = sqlite3_value_type(argv[3]);
         if (typ != SQLITE_INTEGER) {
           ksu_err_msg(context, KSU_ERR_ARG_N_NOT_INT, 4, "dump");
           return;
         }
         len = sqlite3_value_int(argv[3]);
         if (len < 1) {
           ksu_err_msg(context, KSU_ERR_INV_ARG_VAL_RANGE, "dump");
           return;
         }
       }
     }
   }
   // Quickly compute a buffer size
   bufsz = 35 + (charset ? 20 : 0) + ((1+charlen-start)*4<sz?
             ((1+charlen-start)*4):sz) * (1 + (fmt == 16 ? 2 : 3));
   if ((rslt = (char *)sqlite3_malloc(bufsz)) == (char *)NULL) {
      sqlite3_result_error_nomem(context);
      return;
   }
   typ = sqlite3_value_type(argv[0]);
   switch(typ) {
      case SQLITE_INTEGER:
      case SQLITE_FLOAT:
           typ = 2;
           break;
      case SQLITE_TEXT:
           if (ksu_is_datetime((const char *)p, &t, 0)) {
             typ = 12;
           } else {
             typ = 96;
           }
           break;
      default:
           typ = 113;
           break;
    }
   if (charset) {
     sprintf(rslt, "Typ=%d Len=%d CharacterSet=UTF8: ", typ, sz);
   } else {
     sprintf(rslt, "Typ=%d Len=%d: ", typ, sz);
   }
   for (i = 1; i < start; i++) {
     SQLITE_SKIP_UTF8(p);
   }
   k = 0;
   while (i <= charlen) {
     if (i > start) {
       strcat(rslt, ",");
     }
     n = ksu_is_utf8((const unsigned char *)p);
     switch(fmt) {
       case 8:
            for (j = 0; j < n; j++) {
              if (j) {
                strcat(rslt, ",");
              }
              sprintf(tmp, "%o", *p);
              strcat(rslt, tmp);
              p++;
            }
            break;
       case 10:
            for (j = 0; j < n; j++) {
              if (j) {
                strcat(rslt, ",");
              }
              sprintf(tmp, "%d", *p);
              strcat(rslt, tmp);
              p++;
            }
            break;
       case 16:
            for (j = 0; j < n; j++) {
              if (j) {
                strcat(rslt, ",");
              }
              sprintf(tmp, "%02X", *p);
              strcat(rslt, tmp);
              p++;
            }
            break;
       case 17:
            if ((n == 1) && isprint(*p)) {
              sprintf(tmp, "%c", *p);
              strcat(rslt, tmp);
              p++;
            } else {
              for (j = 0; j < n; j++) {
                if (j) {
                  strcat(rslt, ",");
                }
                sprintf(tmp, "%02X", *p);
                strcat(rslt, tmp);
                p++;
              }
            }
            break;
       default:
            break;
     }
     i++;
     k++;
     if (k == len) {
       break;
     }
   }
   sqlite3_result_text(context, rslt, -1, sqlite3_free);
}
