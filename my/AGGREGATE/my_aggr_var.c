#include <stdio.h>

#include "my_aggr_var.h"
#include <ksu_my.h>

extern void my_var_step(sqlite3_context *context,
                        int              argc,
                        sqlite3_value  **argv) {
  CONTEXT_T *ctx;
  int        typ;
  double     val;

  if ((typ = sqlite3_value_type(argv[0])) != SQLITE_NULL) {
    ctx = (CONTEXT_T *)sqlite3_aggregate_context(context,
                          sizeof(CONTEXT_T));
    val = my_value_double(argv[0], (double)0);
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
