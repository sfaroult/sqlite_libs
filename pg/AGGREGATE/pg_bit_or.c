#include <stdio.h>
#include <ksu_common.h>

typedef struct {
  unsigned long  n;  // To know whether "bits" has been initialized
  sqlite3_int64  bits;
} CONTEXT_T;

extern void pg_bit_or_step(sqlite3_context *context,
                           int              argc,
                           sqlite3_value  **argv) {
  CONTEXT_T     *ctx;
  int            typ;
  sqlite3_int64  val;

  typ = sqlite3_value_type(argv[0]);
  if (typ != SQLITE_NULL) {
    if (typ != SQLITE_INTEGER) {
      ksu_err_msg(context, KSU_ERR_INV_DATATYPE, "bit_and");
      return;
    }
    val = sqlite3_value_int64(argv[0]);
    ctx = (CONTEXT_T *)sqlite3_aggregate_context(context, sizeof(CONTEXT_T));
    if (ctx) {
      if (ctx->n == 0) {
        ctx->bits = val;
      } else {
        ctx->bits |= val;
      }
      (ctx->n)++;
    }
  }
}

extern void pg_bit_or_final(sqlite3_context *context) {
  CONTEXT_T *ctx;

  ctx = (CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->n) {
      sqlite3_result_int64(context, ctx->bits);
    } else {
      sqlite3_result_null(context);
    }
  } else {
    sqlite3_result_null(context);
  }
}
