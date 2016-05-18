#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#include <ksu_common.h>
#include <ksu_dates.h>

#define ENV_LEN      256

#define ORAENV_NOT_FOUND	-1
#define ORAENV_ACTION	  0
#define ORAENV_AUDITED_CURSORID	  1
#define ORAENV_AUTHENTICATED_IDENTITY	  2
#define ORAENV_AUTHENTICATION_DATA	  3
#define ORAENV_AUTHENTICATION_METHOD	  4
#define ORAENV_BG_JOB_ID	  5
#define ORAENV_CDB_NAME	  6
#define ORAENV_CLIENT_IDENTIFIER	  7
#define ORAENV_CLIENT_INFO	  8
#define ORAENV_CLIENT_PROGRAM_NAME	  9
#define ORAENV_CON_ID	 10
#define ORAENV_CON_NAME	 11
#define ORAENV_CURRENT_BIND	 12
#define ORAENV_CURRENT_EDITION_ID	 13
#define ORAENV_CURRENT_EDITION_NAME	 14
#define ORAENV_CURRENT_SCHEMA	 15
#define ORAENV_CURRENT_SCHEMAID	 16
#define ORAENV_CURRENT_SQL	 17
#define ORAENV_CURRENT_SQL1	 18
#define ORAENV_CURRENT_SQL2	 19
#define ORAENV_CURRENT_SQL3	 20
#define ORAENV_CURRENT_SQL4	 21
#define ORAENV_CURRENT_SQL5	 22
#define ORAENV_CURRENT_SQL6	 23
#define ORAENV_CURRENT_SQL7	 24
#define ORAENV_CURRENT_SQL_LENGTH	 25
#define ORAENV_CURRENT_USER	 26
#define ORAENV_CURRENT_USERID	 27
#define ORAENV_DATABASE_ROLE	 28
#define ORAENV_DBLINK_INFO	 29
#define ORAENV_DB_DOMAIN	 30
#define ORAENV_DB_NAME	 31
#define ORAENV_DB_SUPPLEMENTAL_LOG_LEVEL	 32
#define ORAENV_DB_UNIQUE_NAME	 33
#define ORAENV_ENTERPRISE_IDENTITY	 34
#define ORAENV_ENTRYID	 35
#define ORAENV_FG_JOB_ID	 36
#define ORAENV_GLOBAL_CONTEXT_MEMORY	 37
#define ORAENV_GLOBAL_UID	 38
#define ORAENV_HOST	 39
#define ORAENV_IDENTIFICATION_TYPE	 40
#define ORAENV_INSTANCE	 41
#define ORAENV_INSTANCE_NAME	 42
#define ORAENV_IP_ADDRESS	 43
#define ORAENV_ISDBA	 44
#define ORAENV_IS_APPLY_SERVER	 45
#define ORAENV_IS_DG_ROLLING_UPGRADE	 46
#define ORAENV_LANG	 47
#define ORAENV_LANGUAGE	 48
#define ORAENV_MODULE	 49
#define ORAENV_NETWORK_PROTOCOL	 50
#define ORAENV_NLS_CALENDAR	 51
#define ORAENV_NLS_CURRENCY	 52
#define ORAENV_NLS_DATE_FORMAT	 53
#define ORAENV_NLS_DATE_LANGUAGE	 54
#define ORAENV_NLS_SORT	 55
#define ORAENV_NLS_TERRITORY	 56
#define ORAENV_ORACLE_HOME	 57
#define ORAENV_OS_USER	 58
#define ORAENV_PLATFORM_SLASH	 59
#define ORAENV_POLICY_INVOKER	 60
#define ORAENV_PROXY_ENTERPRISE_IDENTITY	 61
#define ORAENV_PROXY_USER	 62
#define ORAENV_PROXY_USERID	 63
#define ORAENV_SCHEDULER_JOB	 64
#define ORAENV_SERVER_HOST	 65
#define ORAENV_SERVICE_NAME	 66
#define ORAENV_SESSIONID	 67
#define ORAENV_SESSION_EDITION_ID	 68
#define ORAENV_SESSION_EDITION_NAME	 69
#define ORAENV_SESSION_USER	 70
#define ORAENV_SESSION_USERID	 71
#define ORAENV_SID	 72
#define ORAENV_STATEMENTID	 73
#define ORAENV_TERMINAL	 74
#define ORAENV_UNIFIED_AUDIT_SESSIONID	 75

#define ORAENV_COUNT	76

static char *G_oraenv_words[] = {
    "ACTION", "AUDITED_CURSORID", "AUTHENTICATED_IDENTITY",
    "AUTHENTICATION_DATA", "AUTHENTICATION_METHOD", "BG_JOB_ID",
    "CDB_NAME", "CLIENT_IDENTIFIER", "CLIENT_INFO", "CLIENT_PROGRAM_NAME",
    "CON_ID", "CON_NAME", "CURRENT_BIND", "CURRENT_EDITION_ID",
    "CURRENT_EDITION_NAME", "CURRENT_SCHEMA", "CURRENT_SCHEMAID",
    "CURRENT_SQL", "CURRENT_SQL1", "CURRENT_SQL2", "CURRENT_SQL3",
    "CURRENT_SQL4", "CURRENT_SQL5", "CURRENT_SQL6", "CURRENT_SQL7",
    "CURRENT_SQL_LENGTH", "CURRENT_USER", "CURRENT_USERID", "DATABASE_ROLE",
    "DBLINK_INFO", "DB_DOMAIN", "DB_NAME", "DB_SUPPLEMENTAL_LOG_LEVEL",
    "DB_UNIQUE_NAME", "ENTERPRISE_IDENTITY", "ENTRYID", "FG_JOB_ID",
    "GLOBAL_CONTEXT_MEMORY", "GLOBAL_UID", "HOST", "IDENTIFICATION_TYPE",
    "INSTANCE", "INSTANCE_NAME", "IP_ADDRESS", "ISDBA", "IS_APPLY_SERVER",
    "IS_DG_ROLLING_UPGRADE", "LANG", "LANGUAGE", "MODULE", "NETWORK_PROTOCOL",
    "NLS_CALENDAR", "NLS_CURRENCY", "NLS_DATE_FORMAT", "NLS_DATE_LANGUAGE",
    "NLS_SORT", "NLS_TERRITORY", "ORACLE_HOME", "OS_USER", "PLATFORM_SLASH",
    "POLICY_INVOKER", "PROXY_ENTERPRISE_IDENTITY", "PROXY_USER",
    "PROXY_USERID", "SCHEDULER_JOB", "SERVER_HOST", "SERVICE_NAME",
    "SESSIONID", "SESSION_EDITION_ID", "SESSION_EDITION_NAME", "SESSION_USER",
    "SESSION_USERID", "SID", "STATEMENTID", "TERMINAL",
    "UNIFIED_AUDIT_SESSIONID", NULL};

static int oraenv_search(char *w) {
  int start = 0;
  int end = ORAENV_COUNT - 1;
  int mid;
  int pos = ORAENV_NOT_FOUND;
  int comp;

  if (w) {
    while(start<=end){
      mid = (start + end) / 2;
      if ((comp = strcasecmp(G_oraenv_words[mid], w)) == 0) {
         pos = mid;
         start = end + 1;
      } else if ((mid < ORAENV_COUNT)
                 && ((comp = strcasecmp(G_oraenv_words[mid+1], w)) == 0)) {
         pos = mid+1;
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

extern void ora_env(sqlite3_context *context, char *what,
                    int len, char legacy) {
   // "legacy" is set to 1 if called from the userenv() function
   // "len" is ignored.
   int           code;
   int           terr;
   int           i;
   char         *p;
   char         *v;
   char          env[ENV_LEN];
   char          buff[ENV_LEN];
   short         date_order;
   sqlite3      *thisdb;
   struct lconv *lc;

   if (what) {
     ksu_i18n();
     code = oraenv_search(what);
     switch (code) {
       // Only ones also supported by the legacy function
       case ORAENV_CLIENT_INFO:
            sqlite3_result_text(context, "KSU SQLite library",
                                -1, SQLITE_STATIC);
            break;
       case ORAENV_ENTRYID:
            // Return 0, always
            sqlite3_result_int(context, 0);
            break;
       case ORAENV_ISDBA:
            sqlite3_result_text(context, "TRUE", -1, SQLITE_STATIC);
            break;
       case ORAENV_LANG:
       case ORAENV_LANGUAGE:
            p = getenv("LANG");
            if (p == NULL) {
              if (code == ORAENV_LANG) {
                sqlite3_result_text(context, "en", -1, SQLITE_STATIC);
              } else {
                sqlite3_result_text(context, "en_US.UTF-8", -1, SQLITE_STATIC);
              }
            } else {
              strncpy(env, p, ENV_LEN);
              if (code == ORAENV_LANG) {
                if ((p = strchr(env, '_')) != NULL) {
                  *p = '\0';
                }
              } else {
                if ((p = strchr(env, '.')) != NULL) {
                  *p = '\0';
                  strncat(env, ".UTF-8", ENV_LEN - strlen(env) - 1);
                }
              }
              sqlite3_result_text(context, env, -1, SQLITE_TRANSIENT);
            }
            break;
       case ORAENV_SESSIONID:
            // Return 0, always
            sqlite3_result_int(context, 0);
            break;
       case ORAENV_SID:
            sqlite3_result_int(context, getpid());
            break;
       case ORAENV_TERMINAL:
            p = ttyname(1);  // 1 is the file descriptor for stdout
            if (p) {
              sqlite3_result_text(context, p, -1, NULL);
            } else {
              sqlite3_result_null(context);
            }
            break;
       default:
            if (!legacy) {
              switch(code) {
                case ORAENV_ACTION:
                case ORAENV_AUDITED_CURSORID:
                case ORAENV_AUTHENTICATED_IDENTITY:
                case ORAENV_AUTHENTICATION_DATA:
                case ORAENV_AUTHENTICATION_METHOD:
                case ORAENV_CDB_NAME:
                case ORAENV_CLIENT_IDENTIFIER:
                case ORAENV_CLIENT_PROGRAM_NAME:
                case ORAENV_CURRENT_BIND:
                case ORAENV_CURRENT_SQL:
                case ORAENV_CURRENT_SQL1:
                case ORAENV_CURRENT_SQL2:
                case ORAENV_CURRENT_SQL3:
                case ORAENV_CURRENT_SQL4:
                case ORAENV_CURRENT_SQL5:
                case ORAENV_CURRENT_SQL6:
                case ORAENV_CURRENT_SQL7:
                case ORAENV_CURRENT_SQL_LENGTH:
                case ORAENV_DB_SUPPLEMENTAL_LOG_LEVEL:
                case ORAENV_DBLINK_INFO:
                case ORAENV_GLOBAL_CONTEXT_MEMORY:
                case ORAENV_GLOBAL_UID:
                case ORAENV_IP_ADDRESS:
                case ORAENV_MODULE:
                case ORAENV_NETWORK_PROTOCOL:
                case ORAENV_ORACLE_HOME:
                case ORAENV_PROXY_ENTERPRISE_IDENTITY:
                case ORAENV_PROXY_USER:
                case ORAENV_PROXY_USERID:
                case ORAENV_POLICY_INVOKER:
                case ORAENV_SCHEDULER_JOB:
                case ORAENV_SERVICE_NAME:
                case ORAENV_STATEMENTID:
                case ORAENV_UNIFIED_AUDIT_SESSIONID:
                     sqlite3_result_null(context);
                     break;
                case ORAENV_BG_JOB_ID:
                     if (1 == getppid()) {
                       sqlite3_result_int(context, getpid());
                     } else {
                       sqlite3_result_null(context);
                     }
                     break;
                case ORAENV_CON_ID:
                     sqlite3_result_int(context, 0);
                     break;
                case ORAENV_CURRENT_EDITION_ID:
                case ORAENV_SESSION_EDITION_ID:
                     sqlite3_result_int(context, sqlite3_libversion_number());
                     break;
                case ORAENV_CURRENT_EDITION_NAME:
                case ORAENV_SESSION_EDITION_NAME:
                     sprintf(env, "KSU library with SQLite %s",
                             (char *)sqlite3_libversion());
                     sqlite3_result_text(context, env, -1, SQLITE_TRANSIENT);
                     break;
                case ORAENV_OS_USER:
                case ORAENV_CURRENT_USER:
                case ORAENV_CURRENT_SCHEMA:
                case ORAENV_SESSION_USER:
                case ORAENV_ENTERPRISE_IDENTITY:
                     p = getenv("USER");
                     if (p) {
                       sqlite3_result_text(context, p, -1, NULL);
                     } else {
                       sqlite3_result_null(context);
                     } 
                     break;
                case ORAENV_CURRENT_SCHEMAID:
                case ORAENV_SESSION_USERID:
                case ORAENV_CURRENT_USERID:
                     sqlite3_result_int(context, (int)getuid());
                     break;
                case ORAENV_DATABASE_ROLE:
                     sqlite3_result_text(context,
                                         "PRIMARY", -1, SQLITE_STATIC);
                     break;
                case ORAENV_DB_DOMAIN:
                     if (getdomainname(env, ENV_LEN) == 0) {
                       env[ENV_LEN - 1] = '\0';
                       sqlite3_result_text(context, env, -1, SQLITE_TRANSIENT);
                     } else {
                       sqlite3_result_null(context);
                     }
                     break;
                case ORAENV_DB_NAME:
                case ORAENV_CON_NAME:
                case ORAENV_DB_UNIQUE_NAME:
                case ORAENV_INSTANCE_NAME:
                     thisdb = sqlite3_context_db_handle(context);
                     if (thisdb) {
                       v = (char *)sqlite3_db_filename(thisdb,
                                                (const char *)"main");
                       if (v == NULL){
                         sqlite3_result_text(context, ":memory:",
                                             -1, SQLITE_STATIC);
                       } else {
                         if ((p = strrchr(v, '/')) != (char *)NULL) {
                           p++;
                         } else {
                           p = v;
                         }
                         strncpy(env, p, ENV_LEN);
                         if ((p = strchr(env, '.')) != (char *)NULL) {
                           *p = '\0';
                         }
                         sqlite3_result_text(context, env,
                                             -1, SQLITE_TRANSIENT);
                       }
                     } else {
                       sqlite3_result_null(context);
                     }
                     break;
                case ORAENV_FG_JOB_ID:
                     if (1 == getppid()) {
                       sqlite3_result_null(context);
                     } else {
                       sqlite3_result_int(context, getpid());
                     }
                     break;
                case ORAENV_HOST:
                case ORAENV_SERVER_HOST:
                     if (gethostname(env, ENV_LEN) == 0) {
                       env[ENV_LEN - 1] = '\0';
                       sqlite3_result_text(context, env, -1, SQLITE_TRANSIENT);
                     } else {
                       sqlite3_result_null(context);
                     }
                     break;
                case ORAENV_IDENTIFICATION_TYPE:
                     sqlite3_result_text(context, "EXTERNAL",
                                         -1, SQLITE_STATIC);
                     break;
                case ORAENV_INSTANCE:
                     sqlite3_result_int(context, 1);
                     break;
                case ORAENV_IS_APPLY_SERVER:
                case ORAENV_IS_DG_ROLLING_UPGRADE:
                     sqlite3_result_text(context, "FALSE", -1, SQLITE_STATIC);
                     break;
                case ORAENV_NLS_CALENDAR:
                     sqlite3_result_text(context, "GREGORIAN",
                                         -1, SQLITE_STATIC);
                     break;
                case ORAENV_NLS_CURRENCY:
                     lc = localeconv();
                     if (lc) {
                       sqlite3_result_text(context, lc->int_curr_symbol,
                                           3, SQLITE_STATIC);
                     } else {
                       sqlite3_result_null(context);
                     }
                     break;
                case ORAENV_NLS_DATE_FORMAT:
                     p = ksu_short_date_fmt(env, &date_order);
                     if (p) {
                       i = 0; 
                       while (*p) {
                         if (*p == '%') {
                           env[i++] = *p;
                           while (*p && (*p != 'd')) {
                             p++;
                           }
                           env[i++] = 's';
                         } else {
                           env[i++] = *p;
                         }
                         p++;
                       }
                       env[i] = '\0';
                       switch (date_order) {
                         case KSU_DATE_ORDER_MDY:
                              sprintf(buff, env, "MM", "DD", "YYYY");
                              break;
                         case KSU_DATE_ORDER_DMY:
                              sprintf(buff, env, "DD", "MM", "YYYY");
                              break;
                         default: //  KSU_DATE_ORDER_YMD
                              sprintf(buff, env, "YYYY", "MM", "DD");
                              break;
                       }
                       sqlite3_result_text(context, buff, -1, SQLITE_TRANSIENT);
                     } else {
                       sqlite3_result_null(context);
                     }
                     break;
                case ORAENV_NLS_DATE_LANGUAGE:
                     p = getenv("LANG");
                     if (p == NULL) {
                       sqlite3_result_text(context, "en", -1, SQLITE_STATIC);
                     } else {
                       strncpy(env, p, ENV_LEN);
                       if ((p = strchr(env, '_')) != NULL) {
                         *p = '\0';
                       }
                       sqlite3_result_text(context, env, -1, SQLITE_TRANSIENT);
                     }
                     break;
                case ORAENV_NLS_SORT:
                     sqlite3_result_text(context, "BINARY", -1, SQLITE_STATIC);
                     break;
                case ORAENV_NLS_TERRITORY:
                     terr = ksu_territory();
                     if (terr != KSU_TERR_NOT_FOUND) {
                       sqlite3_result_text(context, ksu_terr_keyword(terr),
                                         -1, SQLITE_STATIC);
                     } else {
                       sqlite3_result_null(context);
                     }
                     break;
                case ORAENV_PLATFORM_SLASH:
                     // Probably doesn't compile on Windows ...
#ifdef _WIN32
                     sqlite3_result_text(context, "\\", -1, SQLITE_STATIC);
#else
                     sqlite3_result_text(context, "/", -1, SQLITE_STATIC);
#endif
                     break;
                default : // Not found
                     ksu_err_msg(context, KSU_ERR_INV_FIELD,
                                 what,
                                 (legacy ? "userenv": "sys_context"));
                     break;
              }
            } else {
              // Not supported in the legacy function
              ksu_err_msg(context, KSU_ERR_INV_FIELD, what, "userenv");
            }
            break;
     }
   } else {
     sqlite3_result_null(context);
   }
}

extern void     ora_sys_context(sqlite3_context * context,
                                int               argc,
                                sqlite3_value  ** argv) {
    char *namespace;
    char *parameter;
    int   len = 256;

    _ksu_check_arg_cnt(argc, 2, 3, "sys_context");
    _ksu_null_if_null_param(argc, argv);
    namespace = (char *)sqlite3_value_text(argv[0]);
    parameter = (char *)sqlite3_value_text(argv[1]);
    if (argc == 3) {
      if (sqlite3_value_type(argv[2]) == SQLITE_INTEGER) {
        len = sqlite3_value_int(argv[2]);
        if ((len < 1) || (len > 4000)) {
          len = 256;
        }
      }
    }
    if (strcasecmp(namespace, "USERENV") == 0) {
      ora_env(context, parameter, len, 0);
    } else if (strcasecmp(namespace, "SYS_SESSION_ROLES") == 0) {
      sqlite3_result_text(context, "FALSE", -1, SQLITE_STATIC);
    } else {
      ksu_err_msg(context, KSU_ERR_INV_FIELD, namespace, "sys_context");
    }
}
