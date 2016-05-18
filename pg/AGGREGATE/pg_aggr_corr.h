#ifndef PG_AGGR_CORR_H

#define PG_AGGR_CORR_H

#include <ksu_common.h>

typedef struct {
  double       sum1;
  double       sum1_sqr;
  double       sum2;
  double       sum2_sqr;
  double       sum_prod;
  unsigned int n;
} CORR_CONTEXT_T;

extern void pg_corr_step(sqlite3_context *context,
                         int              argc,
                         sqlite3_value  **argv);

#endif
