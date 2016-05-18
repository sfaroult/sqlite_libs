#include <stdio.h>

#include "pg_aggr_regr.h"

void pg_regr_count_final(sqlite3_context *context) {
  REGR_CONTEXT_T *ctx;

  ctx = (REGR_CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->n) {
      sqlite3_result_int(context, ctx->n);
    } else {
      sqlite3_result_null(context);
    }
  } else {
    sqlite3_result_null(context);
  }
}
