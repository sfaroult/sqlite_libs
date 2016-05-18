#include <stdio.h>
#include <math.h>
#include <ksu_common.h>

#include "my_aggr_stddev.h"

void my_stddev_pop_final(sqlite3_context *context) {
  CONTEXT_T *ctx;

  ctx = (CONTEXT_T *)sqlite3_aggregate_context(context,0);
  if (ctx) {
    if (ctx->n) {
      if (ctx->n == 1) {
        sqlite3_result_int(context, 0);
      } else {
        sqlite3_result_double(context,
              sqrt((ctx->sum_sqr - (ctx->sum * ctx->sum)/ctx->n) /
                   ctx->n));
     }
    } else {
      sqlite3_result_null(context);
    }
  } else {
    sqlite3_result_null(context);
  }
}
