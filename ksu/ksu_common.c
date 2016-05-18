#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <langinfo.h>

#include <ksu_common.h>
#include <ksu_dates.h>

//
// Error messages. They try to be consistent with regular sqlite
// error messages rather than mimic the error messages in the
// target DBMS. Parameters (and parameter order) are dependent
// on the message.
//
static char *G_errors[KSU_ERR_COUNT] =
 {"wrong number of arguments to function %s() - %d to %d expected",     // 0
  "wrong number of arguments to function %s() - at least %d expected",
  "argument %d to function %s() must be numeric",
  "argument to function %s() must be numeric",
  "argument %d to function %s() must be positive",
  "argument to function %s() must be positive",                        // 5
  "argument %d to function %s() must be strictly positive",
  "argument to function %s() must be strictly positive",
  "argument %d to function %s() must be an integer",
  "argument to function %s() must be an integer",
  "argument %d to function %s() must be a positive integer",           // 10
  "argument to function %s() must be a positive integer",
  "argument %d to function %s() must be a strictly positive integer",
  "argument to function %s() must be a strictly positive integer",
  "argument %d to function %s() cannot be zero",
  "argument to function %s() cannot be zero",                          // 15
  "argument %d to function %s() must be a date or a timestamp",
  "argument to function %s() must be a date or a timestamp",
  "argument %d to function %s() must be a date",
  "argument to function %s() must be a date",
  "argument %d to function %s() must be a time",                       // 20
  "argument to function %s() must be a time",
  "argument %d to function %s() must be a time interval",
  "argument to function %s() must be a time interval",
  "datetime \"%s\" out of range for function %s()",
  "invalid format \"%s\" in call to function %s()",                   // 25
  "argument %d to function %s() is an invalid format",
  "invalid time unit \"%s\" in call to function function %s()",
  "argument %d to function %s() is an invalid field",
  "invalid field \"%s\" in call to function %s()",
  "unsupported format \"%s\" in call to function %s()",               // 30
  "invalid date \"%s\" in call to function %s()",
  "error converting date in function %s()",
  "invalid day \"%s\" in call to function %s()",
  "invalid month \"%s\" in call to function %s()",
  "invalid year \"%s\" in call to function %s()",                     // 35
  "invalid hour \"%s\" in call to function %s()",
  "invalid minute \"%s\" in call to function %s()",
  "invalid second \"%s\" in call to function %s()",
  "invalid UTF8 string in call to function %s()",
  "invalid timezone format ((+/-)HH:MM expected) in call to function %s()",// 40
  "invalid length \"%s\" in call to function %s()",
  "argument 1 to function log() cannot be 1",
  "argument 2 to function power() must be integer if the first one is negative",
  "argument %d to function %s() must be numeric or datetime",
  "value out of range in call to function %s()",                     // 45
  "invalid binary value in call to function %s()",
  "argument %d to function %s() is an invalid binary value",
  "invalid data type in call to function %s()",
  "argument %d to function %s() is an invalid data type",
  "invalid field for this type of column in call to function %s()",  // 50
  "invalid numeric value in call to function %s()",
  "invalid period in call to function %s()",
  "syntax error in call to function %s()",
  "syntax error near \"%s\" in call to function %s()",
  "invalid numerical interval in call to function %s()",            // 55
  "invalid interval \"%s\" in call to function %s()",
  "result of call to function %s() out of range",
  "invalid time or datetime \"%s\" in call to function %s()",
  "invalid time \"%s\" in call to function %s()",
  "input doesn\'t match format in call to function %s()",           // 60
  "conversion error in call to function %s()",
  "division by zero in call to function %s()",
  "wrong call to function %s()"                                
 };

#define ENV_LEN 50

static char *G_ksu_terr_words[] = {
    "AM", "AT", "AU", "BE", "BG", "BR", "BY", "CA", "CH", "CN", "CZ",
    "DE", "DK", "EE", "ES", "ET", "FI", "FR", "GB", "GR", "HK", "HR",
    "HU", "IE", "IL", "IN", "IS", "IT", "JP", "KR", "KZ", "LT", "NL",
    "NO", "NZ", "PL", "PT", "RO", "RU", "SE", "SI", "SK", "TR", "TW",
    "UA", "US", "YU", "ZA", NULL};

static int G_territory = KSU_TERR_NOT_FOUND;

extern int ksu_terr_search(char *w) {
  int start = 0;
  int end = KSU_TERR_COUNT - 1;
  int mid;
  int pos = KSU_TERR_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_ksu_terr_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
      } else {
        if (comp < 0) {
           start = mid + 1;
        } else {
           end = mid - 1;
        }
      }
    }
  }
  return pos;
}

extern char *ksu_terr_keyword(int code) {
  if ((code >= 0) && (code < KSU_TERR_COUNT)) {
    return G_ksu_terr_words[code];
  } else {
    return (char *)NULL;
  }
}

extern int  ksu_territory(void) {
  return G_territory;
}

extern int ksu_clock(void) {
  int h;

  // Returns 12 or 24 depending on the clock in the territory
  ksu_i18n();
  switch(G_territory) {
    case KSU_TERR_US:
    case KSU_TERR_GB:
    case KSU_TERR_IE:
    case KSU_TERR_AU:
    case KSU_TERR_IN:
      h = 12;
      break;
    default:
      h = 24;
      break;
  }
  return h;
}

extern void ksu_i18n() {
   char *p = getenv("LANG");
   if (p && (G_territory == KSU_TERR_NOT_FOUND)) {
      char *q;
      char *t = (char *)NULL;
      char  env[ENV_LEN];

      strncpy(env, p, ENV_LEN);
      // Find the territory
      if ((q = strchr(env, '_')) != NULL) {
         t = q;
         t++;
      }
      if ((q = strchr(env, '.')) != NULL) {
         *q = '\0';
         if (t) {
           G_territory = ksu_terr_search(t);
         }
      }
      strncat(env, ".UTF-8", ENV_LEN - strlen(env) - 1);
      //printf("env = %s\n", env);
      setlocale(LC_ALL, env);
   } else {
      (void)setlocale(LC_ALL, "en_US.UTF-8");
   }
}

//
// Helper functions. Some of them could be macros.
//
extern char ksu_is_num(sqlite3_value *val) {
   int t=sqlite3_value_type(val);
   return ((t==SQLITE_FLOAT)||(t==SQLITE_INTEGER));
}

extern char ksu_is_nz_num(sqlite3_value *val) {
   int t=sqlite3_value_type(val);
   if ((t==SQLITE_FLOAT)||(t==SQLITE_INTEGER)) {
      return (sqlite3_value_double(val)!=(double)0);
   } else {
     return 0;
   }
}

extern char ksu_is_num_ge(sqlite3_value *val, double min) {
   int t=sqlite3_value_type(val);
   if ((t==SQLITE_FLOAT)||(t==SQLITE_INTEGER)) {
      return (sqlite3_value_double(val)>=min);
   } else {
     return 0;
   }
}

extern char ksu_is_num_gt(sqlite3_value *val, double min) {
   int t=sqlite3_value_type(val);
   if ((t==SQLITE_FLOAT)||(t==SQLITE_INTEGER)) {
      return (sqlite3_value_double(val)>min);
   } else {
     return 0;
   }
}

extern char ksu_is_num_le(sqlite3_value *val, double max) {
   int t=sqlite3_value_type(val);
   if ((t==SQLITE_FLOAT)||(t==SQLITE_INTEGER)) {
      return (sqlite3_value_double(val)<=max);
   } else {
     return 0;
   }
}

extern char ksu_is_num_lt(sqlite3_value *val, double max) {
   int t=sqlite3_value_type(val);
   if ((t==SQLITE_FLOAT)||(t==SQLITE_INTEGER)) {
      return (sqlite3_value_double(val)<max);
   } else {
     return 0;
   }
}

extern char ksu_is_num_between(sqlite3_value *val, double min, double max) {
   int t=sqlite3_value_type(val);
   if ((t==SQLITE_FLOAT)||(t==SQLITE_INTEGER)) {
      double d = sqlite3_value_double(val);
      return ((d>=min)&&(d<=max));
   } else {
     return 0;
   }
}

extern char ksu_is_int(sqlite3_value *val) {
   int t=sqlite3_value_type(val);
   return (t==SQLITE_INTEGER);
}

extern char ksu_is_nz_int(sqlite3_value *val) {
   int t=sqlite3_value_type(val);
   if (t==SQLITE_INTEGER) {
      return (sqlite3_value_int(val)!=0);
   } else {
     return 0;
   }
}

extern char ksu_is_int_ge(sqlite3_value *val, int min) {
   int t=sqlite3_value_type(val);
   if (t==SQLITE_INTEGER) {
      return (sqlite3_value_int(val)>=min);
   } else {
     return 0;
   }
}

extern char ksu_is_int_gt(sqlite3_value *val, int min) {
   int t=sqlite3_value_type(val);
   if (t==SQLITE_INTEGER) {
      return (sqlite3_value_int(val)>min);
   } else {
     return 0;
   }
}

extern char ksu_is_int_le(sqlite3_value *val, int max) {
   int t=sqlite3_value_type(val);
   if (t==SQLITE_INTEGER) {
      return (sqlite3_value_int(val)<=max);
   } else {
     return 0;
   }
}

extern char ksu_is_int_lt(sqlite3_value *val, int max) {
   int t=sqlite3_value_type(val);
   if (t==SQLITE_INTEGER) {
      return (sqlite3_value_int(val)<max);
   } else {
     return 0;
   }
}

extern char ksu_is_int_between(sqlite3_value *val, int min, int max) {
   int t=sqlite3_value_type(val);
   if (t==SQLITE_INTEGER) {
      int i = sqlite3_value_int(val);
      return ((i>=min)&&(i<=max));
   } else {
     return 0;
   }
}

//

extern int ksu_prm_ok(sqlite3_context  *context,
                      int               argc,
                      sqlite3_value   **argv,
                      char             *funcname,
                      ...) {
    // Also checks for NULL
    // Returns 1 if OK, 0 if error or there is a null parameter
    va_list prms;
    int     ok = 1;
    int     val_type;
    int     i;
    int     prm_type;

    va_start(prms, funcname);
    i = 0;
    while (ok && (i < argc)) {
       // First if the parameter is null
       val_type = sqlite3_value_type(argv[i]);
       if (val_type == SQLITE_NULL) {
          sqlite3_result_null(context);
          ok = 0;
       } else {
          prm_type = va_arg(prms, int);
          switch (prm_type) {
             case KSU_PRM_TEXT: // No constraint at all
                  break;
             case KSU_PRM_NUMERIC:
                  if (!ksu_is_num(argv[i])) {
                     if (argc == 1) {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_NOT_NUM, funcname);
                     } else {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_N_NOT_NUM,
                                    i+1, funcname);
                     }
                     ok = 0;
                  }
                  break;
             case KSU_PRM_NUM_NOT_0:
                  if (!ksu_is_nz_num(argv[i])) {
                     if (argc == 1) {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_EQ_0,
                                    funcname);
                     } else {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_N_EQ_0,
                                    i+1, funcname);
                     }
                     ok = 0;
                  }
                  break;
             case KSU_PRM_NUM_GE_0:
                  if (!ksu_is_num_ge(argv[i], (double)0)) {
                     if (argc == 1) {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_NOT_NUM_GE_0,
                                    funcname);
                     } else {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_N_NOT_NUM_GE_0,
                                    i+1, funcname);
                     }
                     ok = 0;
                  }
                  break;
             case KSU_PRM_NUM_GT_0:
                  if (!ksu_is_num_gt(argv[i], (double)0)) {
                     if (argc == 1) {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_NOT_NUM_GT_0,
                                    funcname);
                     } else {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_N_NOT_NUM_GT_0,
                                    i+1, funcname);
                     }
                     ok = 0;
                  }
                  break;
             case KSU_PRM_INT:
                  if (!ksu_is_int(argv[i])) {
                     if (argc == 1) {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_NOT_INT,
                                    funcname);
                     } else {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_N_NOT_INT,
                                    i+1, funcname);
                     }
                     ok = 0;
                  }
                  break;
             case KSU_PRM_INT_NOT_0:
                  if (!ksu_is_nz_int(argv[i])) {
                     if (argc == 1) {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_EQ_0,
                                    funcname);
                     } else {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_N_EQ_0,
                                    i+1, funcname);
                     }
                     ok = 0;
                  }
                  break;
             case KSU_PRM_INT_GE_0:
                  if (!ksu_is_int_ge(argv[i], 0)) {
                     if (argc == 1) {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_NOT_INT_GE_0,
                                    funcname);
                     } else {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_N_NOT_INT_GE_0,
                                    i+1, funcname);
                     }
                     ok = 0;
                  }
                  break;
             case KSU_PRM_INT_GT_0:
                  if (!ksu_is_int_gt(argv[i], 0)) {
                     if (argc == 1) {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_NOT_INT_GT_0,
                                    funcname);
                     } else {
                        ksu_err_msg(context,
                                    KSU_ERR_ARG_N_NOT_INT_GT_0,
                                    i+1, funcname);
                     }
                     ok = 0;
                  }
                  break;
             case KSU_PRM_DATETIME:
                  {
                   KSU_TIME_T t;

                   if (!ksu_is_datetime((const char *)sqlite3_value_text(argv[i]),
                                         &t, 0)) {
                      if (argc == 1) {
                         ksu_err_msg(context,
                                     KSU_ERR_ARG_NOT_DATETIME,
                                     funcname);
                      } else {
                         ksu_err_msg(context,
                                     KSU_ERR_ARG_N_NOT_DATETIME,
                                     i+1, funcname);
                      }
                      ok = 0;
                   }
                  }
                  break;
             case KSU_PRM_DATE:
                  {
                   KSU_TIME_T t;

                   if (!ksu_is_date((const char *)sqlite3_value_text(argv[i]), &t)) {
                      if (argc == 1) {
                         ksu_err_msg(context,
                                     KSU_ERR_ARG_NOT_DATE,
                                     funcname);
                      } else {
                         ksu_err_msg(context,
                                     KSU_ERR_ARG_N_NOT_DATE,
                                     i+1, funcname);
                      }
                      ok = 0;
                   }
                  }
                  break;
             case KSU_PRM_TIME:
                  {
                   KSU_TIME_T t;

                   if (!ksu_is_time((const char *)sqlite3_value_text(argv[i]),
                                     &t, 1)) {
                      if (argc == 1) {
                         ksu_err_msg(context,
                                     KSU_ERR_ARG_NOT_TIME,
                                     funcname);
                      } else {
                         ksu_err_msg(context,
                                     KSU_ERR_ARG_N_NOT_TIME,
                                     i+1, funcname);
                      }
                      ok = 0;
                   }
                  }
                  break;
             case KSU_PRM_DATE_OR_TIME:
                  {
                   KSU_TIME_T t;

                   if (!ksu_is_datetime((const char *)sqlite3_value_text(argv[i]),
                                         &t, 0)) {
                     if (!ksu_is_time((const char *)sqlite3_value_text(argv[i]),
                                       &t, 1)) {
                       if (argc == 1) {
                          ksu_err_msg(context,
                                      KSU_ERR_ARG_NOT_DATETIME,
                                      funcname);
                       } else {
                          ksu_err_msg(context,
                                      KSU_ERR_ARG_N_NOT_DATETIME,
                                      i+1, funcname);
                       }
                       ok = 0;
                     }
                   }
                  }
                  break;
             default:
                break;
          }
       }
       i++;
    }
    va_end(prms);
    return ok;
}

extern void ksu_err_msg(sqlite3_context  *context,
                        int               code, ...) {
            va_list ap;
    static  char    err_msg[KSU_MAX_ERRMSG];

    va_start(ap, code);
    (void)vsprintf(err_msg, G_errors[code], ap);
    va_end(ap);
    sqlite3_result_error(context, (const char *)err_msg, -1);
    return;
}

/*
 *   Computation of roman numbers. Some products (Oracle) use this ...
 */

#define RM_ARRAY_SIZE    8

typedef struct {
                char  code; 
                short val;
               } ROM_T;

static ROM_T G_vals[RM_ARRAY_SIZE] = {{'M', 1000},
                                      {'D', 500},
                                      {'C', 100},
                                      {'L', 50},
                                      {'X', 10},
                                      {'V', 5},
                                      {'I', 1},
                                      {'*', 0}};

static short get_romval(char code) {
    short i = 0;

    G_vals[RM_ARRAY_SIZE - 1].code = code;
    while (G_vals[i].code != toupper(code)) {
       i++;
    }
    if (i == RM_ARRAY_SIZE - 1) {
       return 0;
    } else {
       return G_vals[i].val;
    } 
}

extern int ksu_compute_roman(char *str) {
    char *p;
    int   val = 0;
    int   next_val;
    int   remainder;

    if (str && *str) {
       p = str;
       p++;
       if (*p == '\0') {
          val = get_romval(*str);
       } else {
          remainder = ksu_compute_roman(p);
          val = get_romval(*str);
          next_val = get_romval(*p);
          if (val < next_val) {
             val = remainder - val;
          } else {
             val = remainder + val;
          }
       }
    }
    return val;
}
