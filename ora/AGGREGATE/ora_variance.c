#include <stdio.h>

#include "ora_aggr_var.h"

void ora_variance_final(sqlite3_context *context) {
  CONTEXT_T *ctx;

  ctx = (CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->n > 1) {
      sqlite3_result_double(context,
              (ctx->sum_sqr - (ctx->sum * ctx->sum)/ctx->n) /
               (ctx->n - 1));
    } else if (ctx->n == 1) { 
      sqlite3_result_int(context, 0);
    } else {
      sqlite3_result_null(context);
    }
  } else {
    sqlite3_result_null(context);
  }
}
