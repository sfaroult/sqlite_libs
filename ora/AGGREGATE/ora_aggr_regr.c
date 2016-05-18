#include <stdio.h>

#include "ora_aggr_regr.h"

extern void ora_regr_step(sqlite3_context *context,
                          int              argc,
                          sqlite3_value  **argv) {
  REGR_CONTEXT_T *ctx;
  int        typ1;
  int        typ2;
  double     val1;
  double     val2;

  if (((typ1 = sqlite3_value_type(argv[0])) != SQLITE_NULL)
      && ((typ2 = sqlite3_value_type(argv[1])) != SQLITE_NULL)) {
    if (ksu_prm_ok(context, argc, argv, "covar",
                   KSU_PRM_NUMERIC, KSU_PRM_NUMERIC)) {
      ctx = (REGR_CONTEXT_T *)sqlite3_aggregate_context(context,
                            sizeof(REGR_CONTEXT_T));
      val1 = sqlite3_value_double(argv[0]);
      val2 = sqlite3_value_double(argv[1]);
      if (ctx) {
        ctx->sum1_sqr += (val1 * val1);
        ctx->sum2_sqr += (val2 * val2);
        ctx->sum1 += val1;
        ctx->sum2 += val2;
        ctx->sum_prod += (val1 * val2);
        (ctx->n)++;
      }
    }
  }
}
