#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <ksu_common.h>

typedef struct {
  unsigned long  n;  // To know whether "bits" has been initialized
  int            boolval;
} CONTEXT_T;

extern void pg_bool_or_step(sqlite3_context *context,
                            int              argc,
                            sqlite3_value  **argv) {
  CONTEXT_T *ctx;
  int        typ;
  int        val;

  typ = sqlite3_value_type(argv[0]);
  if (typ != SQLITE_NULL) {
    if (typ == SQLITE_INTEGER) {
      val = sqlite3_value_int(argv[0]);
      if ((val != 0) && (val != 1)) {
        ksu_err_msg(context, KSU_ERR_INV_DATATYPE, "bool_or");
        return;
      }
    } else {
      char *p = (char *)sqlite3_value_text(argv[0]);
      int   len = strlen(p);

      if (len == 1) {
        if (toupper(*p) == 'T') {
          val = 1;
        } else if (toupper(*p) == 'F') {
          val = 0;
        } else {
          ksu_err_msg(context, KSU_ERR_INV_DATATYPE, "bool_or");
          return;
        }
      } else if (strcasecmp(p, "true") == 0) {
        val = 1;
      } else if (strcasecmp(p, "false") == 0) {
        val = 0;
      } else {
        ksu_err_msg(context, KSU_ERR_INV_DATATYPE, "bool_or");
        return;
      }
    }
    val = sqlite3_value_int64(argv[0]);
    ctx = (CONTEXT_T *)sqlite3_aggregate_context(context, sizeof(CONTEXT_T));
    if (ctx) {
      if (ctx->n == 0) {
        ctx->boolval = val;
      } else {
        ctx->boolval |= val;
      }
      (ctx->n)++;
    }
  }
}

extern void pg_bool_or_final(sqlite3_context *context) {
  CONTEXT_T *ctx;

  ctx = (CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->n) {
      sqlite3_result_int(context, ctx->boolval);
    } else {
      sqlite3_result_null(context);
    }
  } else {
    sqlite3_result_null(context);
  }
}
