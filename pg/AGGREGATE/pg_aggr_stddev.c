#include <stdio.h>
#include <math.h>
#include <ksu_common.h>

#include "pg_aggr_stddev.h"

extern void pg_stddev_step(sqlite3_context *context,
                           int             argc,
                           sqlite3_value **argv) {
  CONTEXT_T *ctx;
  int        typ;
  double     val;

  if ((typ = sqlite3_value_type(argv[0])) != SQLITE_NULL) {
    if ((typ != SQLITE_INTEGER) && (typ != SQLITE_FLOAT)) {
       ksu_err_msg(context, KSU_ERR_ARG_NOT_NUM, "stddev");
       return;
    }
    ctx = (CONTEXT_T *)sqlite3_aggregate_context(context,
                          sizeof(CONTEXT_T));
    val = sqlite3_value_double(argv[0]);
    if (ctx) {
       if (ctx->n == 0) {
          ctx->K = val;
       }
       ctx->sum += (val - ctx->K);
       ctx->sum_sqr += (val - ctx->K)
                     * (val - ctx->K);
       (ctx->n)++;
    }
  }
}
