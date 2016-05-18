#ifndef ORA_AGGR_REGR_H

#define ORA_AGGR_REGR_H

#include <ksu_common.h>

typedef struct {
  double       sum1;
  double       sum1_sqr;
  double       sum2;
  double       sum2_sqr;
  double       sum_prod;
  unsigned int n;
} REGR_CONTEXT_T;

extern void ora_regr_step(sqlite3_context *context,
                          int              argc,
                          sqlite3_value  **argv);

#endif
