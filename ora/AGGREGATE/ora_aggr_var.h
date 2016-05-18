#ifndef ORA_AGGR_VAR_H

#define ORA_AGGR_VAR_H

#include <ksu_common.h>

typedef struct {
  double       K;
  double       sum;
  double       sum_sqr;
  unsigned int n;
} CONTEXT_T;

extern void ora_var_step(sqlite3_context *context,
                         int              argc,
                         sqlite3_value  **argv);

#endif
