#include <stdio.h>
#include <math.h>

#include "pg_aggr_corr.h"

void pg_corr_final(sqlite3_context *context) {
  CORR_CONTEXT_T *ctx;
  double          covar;
  double          stddev1;
  double          stddev2;

  ctx = (CORR_CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->n) {
      if (ctx->n == 1) {
        sqlite3_result_int(context, 0);
      } else {
        covar = (ctx->sum_prod - (ctx->sum1 * ctx->sum2)/ctx->n) / ctx->n;
        stddev1 = sqrt((ctx->sum1_sqr - (ctx->sum1 * ctx->sum1)/ctx->n) /
                   ctx->n);
        stddev2 = sqrt((ctx->sum2_sqr - (ctx->sum2 * ctx->sum2)/ctx->n) /
                   ctx->n);
        sqlite3_result_double(context, covar / (stddev1 * stddev2));
     }
    } else {
      sqlite3_result_null(context);
    }
  } else {
    sqlite3_result_null(context);
  }
}
