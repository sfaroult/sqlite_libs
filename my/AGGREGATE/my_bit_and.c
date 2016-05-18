#include <stdio.h>
#include <ksu_common.h>
#include <ksu_my.h>

typedef struct {
  unsigned long  n;  // To know whether "bits" has been initialized
  sqlite3_int64  bits;
} CONTEXT_T;

extern void my_bit_and_step(sqlite3_context *context,
                            int              argc,
                            sqlite3_value  **argv) {
  CONTEXT_T     *ctx;
  sqlite3_int64  val;

  if (sqlite3_value_type(argv[0]) != SQLITE_NULL) {
    val = my_value_int64(argv[0], 0);
    ctx = (CONTEXT_T *)sqlite3_aggregate_context(context, sizeof(CONTEXT_T));
    if (ctx) {
      if (ctx->n == 0) {
        ctx->bits = val;
      } else {
        ctx->bits &= val;
      }
      (ctx->n)++;
    }
  }
}

extern void my_bit_and_final(sqlite3_context *context) {
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
