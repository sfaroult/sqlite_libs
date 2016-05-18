#include <stdio.h>

#include "pg_aggr_covar.h"

void pg_covar_pop_final(sqlite3_context *context) {
  COVAR_CONTEXT_T *ctx;

  ctx = (COVAR_CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->n) {
      if (ctx->n == 1) {
        sqlite3_result_int(context, 0);
      } else {
        sqlite3_result_double(context,
              (ctx->sum_prod - (ctx->sum1 * ctx->sum2)/ctx->n) /
               ctx->n);
     }
    } else {
      sqlite3_result_null(context);
    }
  } else {
    sqlite3_result_null(context);
  }
}
