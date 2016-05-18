#ifndef KSU_COMMON_H
#define KSU_COMMON_H

#include <sqlite3.h>

#include <ksu_utf8.h>

// ---  ERROR MESSAGES ---
#define KSU_MAX_ERRMSG    250

#define KSU_ERR_COUNT      64

#define KSU_ERR_INV_ARG_RANGE         0
#define KSU_ERR_TOO_FEW_ARGS          1
#define KSU_ERR_ARG_N_NOT_NUM         2
#define KSU_ERR_ARG_NOT_NUM           3
#define KSU_ERR_ARG_N_NOT_NUM_GE_0    4
#define KSU_ERR_ARG_NOT_NUM_GE_0      5
#define KSU_ERR_ARG_N_NOT_NUM_GT_0    6
#define KSU_ERR_ARG_NOT_NUM_GT_0      7
#define KSU_ERR_ARG_N_NOT_INT         8
#define KSU_ERR_ARG_NOT_INT           9
#define KSU_ERR_ARG_N_NOT_INT_GE_0   10
#define KSU_ERR_ARG_NOT_INT_GE_0     11
#define KSU_ERR_ARG_N_NOT_INT_GT_0   12
#define KSU_ERR_ARG_NOT_INT_GT_0     13
#define KSU_ERR_ARG_N_EQ_0           14
#define KSU_ERR_ARG_EQ_0             15
#define KSU_ERR_ARG_N_NOT_DATETIME   16
#define KSU_ERR_ARG_NOT_DATETIME     17
#define KSU_ERR_ARG_N_NOT_DATE       18
#define KSU_ERR_ARG_NOT_DATE         19
#define KSU_ERR_ARG_N_NOT_TIME       20
#define KSU_ERR_ARG_NOT_TIME         21
#define KSU_ERR_ARG_N_NOT_INTV       22
#define KSU_ERR_ARG_NOT_INTV         23
#define KSU_ERR_INV_DATE_RANGE       24
#define KSU_ERR_INV_FORMAT           25
#define KSU_ERR_N_INV_FORMAT         26
#define KSU_ERR_INV_TIME_UNIT        27
#define KSU_ERR_N_INV_FIELD          28
#define KSU_ERR_INV_FIELD            29
#define KSU_ERR_UNSUP_FORMAT         30
#define KSU_ERR_INV_DATE             31
#define KSU_ERR_DATE_CONV            32
#define KSU_ERR_INV_DAY              33
#define KSU_ERR_INV_MONTH            34
#define KSU_ERR_INV_YEAR             35
#define KSU_ERR_INV_HOUR             36
#define KSU_ERR_INV_MIN              37
#define KSU_ERR_INV_SEC              38
#define KSU_ERR_INV_UTF8             39
#define KSU_ERR_INV_TZ_FORMAT        40
#define KSU_ERR_INV_LENGTH           41
#define KSU_ERR_INV_LOG_BASE         42
#define KSU_ERR_INV_POWER            43
#define KSU_ERR_ARG_N_NOT_NUM_OR_DAT 44
#define KSU_ERR_INV_ARG_VAL_RANGE    45
#define KSU_ERR_ARG_BINARY           46
#define KSU_ERR_ARG_N_BINARY         47
#define KSU_ERR_INV_DATATYPE         48
#define KSU_ERR_N_INV_DATATYPE       49
#define KSU_ERR_INV_XTRACT_FOR_SRC   50
#define KSU_ERR_INV_NUMERIC          51
#define KSU_ERR_INV_PERIOD           52
#define KSU_ERR_SYNTAX_ERROR         53
#define KSU_ERR_SYNTAX_ERROR_NEAR    54
#define KSU_ERR_INV_INTV_NUM         55
#define KSU_ERR_INV_INTV             56
#define KSU_ERR_RESULT_OUT_OF_RANGE  57
#define KSU_ERR_INV_TIME_DATETIME    58
#define KSU_ERR_INV_TIME             59
#define KSU_ERR_FORMAT_MISMATCH      60
#define KSU_ERR_CONV                 61
#define KSU_ERR_DIV_ZERO             61
#define KSU_ERR_GENERIC              63

extern void ksu_err_msg(sqlite3_context  *context,
                        int code, ...);

// ---  PARAMETER CONSTRAINTS ---
// "Text" is no constraint at all
#define KSU_PRM_TEXT          0
// Numeric, any but zero, positive, strictly positive
#define KSU_PRM_NUMERIC       1
#define KSU_PRM_NUM_NOT_0     2
#define KSU_PRM_NUM_GE_0      3
#define KSU_PRM_NUM_GT_0      4
// Integer
#define KSU_PRM_INT           5
#define KSU_PRM_INT_NOT_0     6
#define KSU_PRM_INT_GE_0      7
#define KSU_PRM_INT_GT_0      8
// Date types. Date means date only, time means time only
#define KSU_PRM_DATETIME      9
#define KSU_PRM_DATE         10
#define KSU_PRM_TIME         11
#define KSU_PRM_DATE_OR_TIME 12

extern int ksu_prm_ok(sqlite3_context  *context,
                      int               argc,
                      sqlite3_value   **argv,
                      char             *funcname,
                      ...);

// ---  INTERNATIONALIZATION ---
//
// Support of multiple languages
// for error messages ?
#define KSU_LANG_UNKNOWN  	-1
#define KSU_LANG_NOT_FOUND	-1
#define KSU_LANG_EN	  0
#define KSU_LANG_FR	  1

#define KSU_LANG_COUNT	2

#ifdef __APPLE__
extern char * gettext(const char * msgid);
extern char * dgettext(const char * domainname, const char * msgid);
extern char * dcgettext(const char * domainname, const char * msgid,
                        int category);
#else
#include <libintl.h>
#endif

#define _(msg)    gettext(msg)

// Territories (for date formats)

#define KSU_TERR_NOT_FOUND	-1
#define KSU_TERR_AM	  0
#define KSU_TERR_AT	  1
#define KSU_TERR_AU	  2
#define KSU_TERR_BE	  3
#define KSU_TERR_BG	  4
#define KSU_TERR_BR	  5
#define KSU_TERR_BY	  6
#define KSU_TERR_CA	  7
#define KSU_TERR_CH	  8
#define KSU_TERR_CN	  9
#define KSU_TERR_CZ	 10
#define KSU_TERR_DE	 11
#define KSU_TERR_DK	 12
#define KSU_TERR_EE	 13
#define KSU_TERR_ES	 14
#define KSU_TERR_ET	 15
#define KSU_TERR_FI	 16
#define KSU_TERR_FR	 17
#define KSU_TERR_GB	 18
#define KSU_TERR_GR	 19
#define KSU_TERR_HK	 20
#define KSU_TERR_HR	 21
#define KSU_TERR_HU	 22
#define KSU_TERR_IE	 23
#define KSU_TERR_IL	 24
#define KSU_TERR_IN	 25
#define KSU_TERR_IS	 26
#define KSU_TERR_IT	 27
#define KSU_TERR_JP	 28
#define KSU_TERR_KR	 29
#define KSU_TERR_KZ	 30
#define KSU_TERR_LT	 31
#define KSU_TERR_NL	 32
#define KSU_TERR_NO	 33
#define KSU_TERR_NZ	 34
#define KSU_TERR_PL	 35
#define KSU_TERR_PT	 36
#define KSU_TERR_RO	 37
#define KSU_TERR_RU	 38
#define KSU_TERR_SE	 39
#define KSU_TERR_SI	 40
#define KSU_TERR_SK	 41
#define KSU_TERR_TR	 42
#define KSU_TERR_TW	 43
#define KSU_TERR_UA	 44
#define KSU_TERR_US	 45
#define KSU_TERR_YU	 46
#define KSU_TERR_ZA	 47

#define KSU_TERR_COUNT	48

extern int   ksu_terr_search(char *w);
extern char *ksu_terr_keyword(int code);

// The following function sets internationalization
// (language for messages and locale for number/date
// formatting) if not already done
extern void ksu_i18n();
// Territory (date formats)
extern int  ksu_territory(void);
// 12 or 24 hour clock (returns 12 or 24)
extern int  ksu_clock(void); 

// ---  Roman numbers ---
extern int ksu_compute_roman(char *str);

// --- Parameter handling  ---
//
//  Returns null as soon as one of the parameters is null
//
//  Special case for Oracle for which an empty string is the
//  same as null.
//
#ifdef ORA_FUNC

#define _ksu_null_if_null_param(argc, argv) { \
     int            i; \
     unsigned char *p; \
     for (i=0; i<argc; i++) { \
       if ((sqlite3_value_type(argv[i]) == SQLITE_NULL) \
           || ((p=(unsigned char *)sqlite3_value_text(argv[i]))!=NULL \
           && (*p=='\0'))) { \
         sqlite3_result_null(context); \
         return; \
       } \
     } \
   }

#else

// Other than Oracle
#define _ksu_null_if_null_param(argc, argv) { \
                int i; \
                for (i=0; i<argc; i++) { \
                  if (sqlite3_value_type(argv[i]) == SQLITE_NULL) { \
                    sqlite3_result_null(context); \
                    return; \
                  } \
                } \
             }

#endif

#define _ksu_check_arg_cnt(argc, mincnt, maxcnt, funcname) { \
               if (argc<mincnt || (maxcnt!=-1 && argc>maxcnt)) { \
                  if (maxcnt>0) { \
                     ksu_err_msg(context,KSU_ERR_INV_ARG_RANGE,funcname,\
                             mincnt,maxcnt); \
                  } else { \
                     ksu_err_msg(context,KSU_ERR_TOO_FEW_ARGS,\
                             funcname,mincnt); \
                  } \
                  return; \
               } \
             }

extern char ksu_is_num_le(sqlite3_value *val, double max);
extern char ksu_is_num_lt(sqlite3_value *val, double max);
extern char ksu_is_num_between(sqlite3_value *val, double min, double max);
extern char ksu_is_int_le(sqlite3_value *val, int max);
extern char ksu_is_int_lt(sqlite3_value *val, int max);
extern char ksu_is_int_between(sqlite3_value *val, int min, int max);

#endif
