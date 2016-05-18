#include <stdio.h>

#include "pg_aggr_regr.h"

void pg_regr_avgy_final(sqlite3_context *context) {
  REGR_CONTEXT_T *ctx;

  ctx = (REGR_CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->n) {
      if (ctx->n == 1) {
        sqlite3_result_int(context, 0);
      } else {
        sqlite3_result_double(context, ctx->sum2/ctx->n);
     }
    } else {
      sqlite3_result_null(context);
    }
  } else {
    sqlite3_result_null(context);
  }
}
